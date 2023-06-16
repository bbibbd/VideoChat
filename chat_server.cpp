// g++ chat_server.cpp -o chat_server -lboost_system -lpthread $(pkg-config --libs --cflags opencv4)

#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <vector>
#include <mutex> // 추가: 쓰레드 동기화를 위한 뮤텍스

using boost::asio::ip::tcp;

struct Client {
    std::shared_ptr<tcp::socket> socket;
    std::string username;
    Client(std::shared_ptr<tcp::socket> sock, const std::string& name) : socket(sock), username(name) {}
};

std::vector<std::shared_ptr<Client>> clients;
std::mutex clientsMutex; // 추가: clients 벡터 동기화를 위한 뮤텍스

void handleClient(std::shared_ptr<Client> client) {
    try {
        while (true) {
            std::array<char, 8> header;
            boost::asio::read(*(client->socket), boost::asio::buffer(header));

            // Assuming that the header contains the size of the frame.
            int frame_size = std::stoi(std::string(header.data(), header.size()));

            std::vector<char> frame_data(frame_size);
            boost::asio::read(*(client->socket), boost::asio::buffer(frame_data));

            // Send the frame to all other clients.
            std::lock_guard<std::mutex> lock(clientsMutex); // 추가: clients 벡터 접근 동기화
            for (const auto& otherClient : clients) {
                if (otherClient != client) {
                    boost::asio::write(*(otherClient->socket), boost::asio::buffer(header));
                    boost::asio::write(*(otherClient->socket), boost::asio::buffer(frame_data));
                }
            }
        }
    } catch (boost::system::system_error& e) {
        std::cerr << "Exception in thread for user " << client->username << ": " << e.what() << std::endl;

        std::lock_guard<std::mutex> lock(clientsMutex); // 추가: clients 벡터 접근 동기화
        auto it = std::find(clients.begin(), clients.end(), client);
        if (it != clients.end()) {
            clients.erase(it);
            std::cout << "Client " << client->username << " disconnected." << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    boost::asio::io_service io_service;
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 1234));

    for (;;) {
        std::shared_ptr<tcp::socket> socket = std::make_shared<tcp::socket>(io_service);
        acceptor.accept(*socket);

        // read username
        std::array<char, 50> username_buffer;
        boost::asio::read(*socket, boost::asio::buffer(username_buffer));
        std::string username(username_buffer.data());

        auto client_ptr = std::make_shared<Client>(socket, username);

        std::lock_guard<std::mutex> lock(clientsMutex); // 추가: clients 벡터 접근 동기화
        clients.push_back(client_ptr);
        std::cout << "User Connected: " << username << std::endl;

        std::thread(handleClient, client_ptr).detach();
    }

    return 0;
}
