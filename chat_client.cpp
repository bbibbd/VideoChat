#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <mutex>

#define FPS 30

using boost::asio::ip::tcp;

cv::Mat frame[2];
std::mutex frameMutex;

void receiveFrames(tcp::socket& socket, const std::string& userName) {
    while (true) {
        std::array<char, 8> header;
        boost::system::error_code error;
        try {
            boost::asio::read(socket, boost::asio::buffer(header), error);
            if (error) {
                if (error != boost::asio::error::eof) {
                    // 예외 처리 코드 추가
                    std::cerr << "Error occurred while reading header: " << error.message() << std::endl;
                }
                cv::Mat sample_frame = cv::imread("my_photo.jpg");
                cv::imshow(userName, sample_frame);
            } else {
                int frame_size = std::stoi(std::string(header.data(), header.size()));
                std::vector<char> frame_data(frame_size);
                boost::asio::read(socket, boost::asio::buffer(frame_data));

                cv::Mat decoded_frame = cv::imdecode(cv::Mat(frame_data), cv::IMREAD_COLOR);

                {
                    std::lock_guard<std::mutex> lock(frameMutex);
                    frame[1] = decoded_frame.clone();
                }
            }
        } catch (const std::exception& ex) {
            // 예외 처리 코드 추가
            std::cerr << "Exception occurred while receiving frames: " << ex.what() << std::endl;
            break;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "usage: ./chat_client IP_ADDR PORT_NUM USERNAME VIDEO_FILE" << std::endl;
        exit(-1);
    }

    std::string ip_addr = argv[1];
    std::string port = argv[2];
    std::string userName = argv[3];
    std::string fileName = argv[4];

    for (int i = 0; i < 2; i++) {
        frame[i] = cv::imread("my_photo.jpg", 1);
    }

    cv::VideoCapture cap(fileName);
        if (!cap.isOpened()) {
        std::cerr << "Failed to open video file" << std::endl;
        return -1;
    }

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(ip_addr, port);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);

    std::array<char, 50> nameBuffer = {};
    std::copy(userName.begin(), userName.end(), nameBuffer.begin());
    try {
        boost::asio::write(socket, boost::asio::buffer(nameBuffer));
    } catch (const std::exception& ex) {
        // 예외 처리 코드 추가
        std::cerr << "Exception occurred while writing username: " << ex.what() << std::endl;
        return -1;
    }

    std::thread receive_thread([&]() {
        receiveFrames(socket, userName);
    });

    while (true) {
        cap >> frame[0];

        double fps = cap.get(cv::CAP_PROP_FPS);
        int delay = 1000 / FPS;

        for (int i = 0; i < 2; i++) {
            resize(frame[i], frame[i], cv::Size(640, 480));
        }

        cv::Size frameSize = frame[0].size();
        cv::Size textSize = cv::getTextSize(userName, cv::FONT_HERSHEY_SIMPLEX, 1.0, 2, nullptr);
        int textX = (frameSize.width - textSize.width) / 2;
        int textY = frameSize.height - 20;

        int boxWidth = textSize.width + 20;
        int boxHeight = textSize.height + 10;
        int boxX = (frameSize.width - boxWidth) / 2;
        int boxY = textY - textSize.height - 5;

        cv::Scalar backgroundColor(50, 50, 50, 0.7);

        cv::rectangle(frame[0], cv::Point(boxX, boxY), cv::Point(boxX + boxWidth, boxY + boxHeight), backgroundColor, -1);
        cv::putText(frame[0], userName, cv::Point(textX, textY), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);

        cv::Mat img_result;
        cv::hconcat(frame[0], frame[1], img_result);

        cv::moveWindow("video chatting2", 150, 200);
        cv::imshow("video chatting2", img_result);

        std::vector<uchar> buffer;
        cv::imencode(".jpg", frame[0], buffer);

        std::string header = std::to_string(buffer.size());
        header.resize(8, ' ');

        try {
            boost::asio::write(socket, boost::asio::buffer(header));
            boost::asio::write(socket, boost::asio::buffer(buffer));
        } catch (const std::exception& ex) {
            // 예외 처리 코드 추가
            std::cerr << "Exception occurred while sending frame: " << ex.what() << std::endl;
            break;
        }

        if (cv::waitKey(delay) == 27) { // ESC 키를 누르면 종료
            break;
        }
    }

    receive_thread.join();
    return 0;
}

