# IoT실습 Final Project - Video Chat

## Prerequisite

* OpenCV (version 4.0 or later)
  + https://docs.google.com/document/d/1ffBeqqe-u4aS8eg_0GVLhRWgm8Lgk1veTTccP_BPyuo/edit?usp=sharing
  + please follow the above document.

 
* Boost.asio
  + https://int-i.github.io/cpp/2020-09-19/ubuntu-boost/
  + please follow the above document.

## How to compile

*  Clone the repository
  
   $ git clone https://github.com/bbibbd/VideoChat
   
   $ cd VideoChat

* Compile the chat_server.cpp

  $ g++ chat_server.cpp -o chat_server -lboost_system -lpthread

* Compile the chat_client.cpp

  $ g++ chat_client.cpp -o chat_client -lboost_system -lpthread -std=c++11 $(pkg-config --libs --cflags opencv4)

## How to use
  * server
    
    $ ./chat_server

  * client
    + Webvam Mode
      
      $ ./chat_client IP_ADDR PORT_NUM USERNAME WEBCAM_INDEX
    + Video Mode
   
      $ ./chat_client IP_ADDR PORT_NUM USERNAME VIDEO_FILE

    + if you want to run with your own webcam, please remove '#define VIDEO' in the client code and compile it again.
   

## Linitations
  * Although the server is designed to accommodate more than two clients, the client code is limited to only two people to chat.
  * Voice calls are not supported.

## 



