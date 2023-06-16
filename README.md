# IoT실습 Final Project - Video Chat

## Prerequisite

* OpenCV (4.0 이상)
  + https://docs.google.com/document/d/1ffBeqqe-u4aS8eg_0GVLhRWgm8Lgk1veTTccP_BPyuo/edit?usp=sharing
  + You can build it according to the above document.

 
* Boost.asio
  + https://int-i.github.io/cpp/2020-09-19/ubuntu-boost/
  + You can build it according to the above document.

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

    + if you want to run with your own webcam, please remove '#ifdef VIDEO' in the client code and compile it again.
   

## Linitations
  * Although the server is designed to accommodate more than two clients, the client code is limited to only two people to chat.
  * Voice calls are not supported.



  
    

## 
* [모인활 End TO End Driving 설치 및 실행 매뉴얼](https://docs.google.com/document/d/1DgSWTMIb2Inw2ieUT4Eole1-8TkLoTGepQfHu_LNIB0/edit) - 한동계정 로그인 필수
* [모인활 발표 PPT](https://docs.google.com/presentation/d/1KOP4jEZU1FWkqvOB553XU_9S5Ag6gN7BOnG9Mlm8jhc/edit?usp=sharing) - 한동계정 로그인 필수
* 시뮬레이터 다운로드 링크 [Windows](https://s3-us-west-1.amazonaws.com/udacity-selfdrivingcar/Term1-Sim/term1-simulator-windows.zip) / [Mac](https://s3-us-west-1.amazonaws.com/udacity-selfdrivingcar/Term1-Sim/term1-simulator-mac.zip) / [Linux](https://s3-us-west-1.amazonaws.com/udacity-selfdrivingcar/Term1-Sim/term1-simulator-windows.zip)
* [Audi A2D2 Dataset](https://www.a2d2.audi/a2d2/en.html)
* [동영상발표 링크](https://youtu.be/Jibx5pM_qdk)

## Usage
* 기본적인 사용법 및 argument들은 다음과 같습니다.
![usage](https://github.com/bbibbd/JoyAI_FINAL/blob/main/usage.png?raw=true)
* 더 자세한 설명은 발표 동영상 또는 설치 메뉴얼을 참고해주시면 됩니다.



