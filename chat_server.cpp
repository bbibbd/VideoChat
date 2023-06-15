// g++ chat_server.cpp -o chat_server -lboost_system -lpthread $(pkg-config --libs --cflags opencv4)

//#include <opencv2/opencv.hpp>
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <vector>

using boost::asio::ip::tcp;

struct Client {
    std::shared_ptr<tcp::socket> socket;
    std::string username;
    Client(std::shared_ptr<tcp::socket> sock, const std::string& name) : socket(sock), username(name) {}
};

int main(int argc, char* argv[]) {
    boost::asio::io_service io_service;

    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 1234));
    std::vector<std::shared_ptr<Client>> clients;

    for (;;) {
        std::shared_ptr<tcp::socket> socket = std::make_shared<tcp::socket>(io_service);
        acceptor.accept(*socket);

        // read username
        std::array<char, 50> username_buffer;
        boost::asio::read(*socket, boost::asio::buffer(username_buffer));
        std::string username(username_buffer.data());

        auto client_ptr = std::make_shared<Client>(socket, username);
        clients.push_back(client_ptr);
        std::cout << "User Connected: " << username << std::endl;

        // thread for sending the video to all clients
        std::thread([client_ptr, &clients]() {
            try {
                while (true) {
                    std::array<char, 8> header;
                    boost::asio::read(*(client_ptr->socket), boost::asio::buffer(header));

                    // Assuming that the header contains the size of the frame.
                    int frame_size = std::stoi(std::string(header.data(), header.size()));

                    std::vector<char> frame_data(frame_size);
                    boost::asio::read(*(client_ptr->socket), boost::asio::buffer(frame_data));

                    // Send the frame to all other clients.
                    for (const auto& client : clients) {
                        if (client != client_ptr) {
                            boost::asio::write(*(client->socket), boost::asio::buffer(header));
                            boost::asio::write(*(client->socket), boost::asio::buffer(frame_data));
                        }
                    }
                }
            } catch (boost::system::system_error& e) {
                std::cerr << "Exception in thread for user " << client_ptr->username << ": " << e.what() << std::endl;
                auto it = std::find(clients.begin(), clients.end(), client_ptr);
                if (it != clients.end()) {
                    clients.erase(it);
                    std::cout << "Client " << client_ptr->username << " disconnected." << std::endl;
                }
            }
        }).detach();
    }

    return 0;
}