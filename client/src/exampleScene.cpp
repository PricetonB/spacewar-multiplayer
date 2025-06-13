//example scene


#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <iostream>
#include <array>
#include <string>
#include <thread>
#include <SFML/Graphics.hpp>
#include "NetworkHandler.cpp"

void check_and_send_wasd(sf::UdpSocket& socket, const sf::IpAddress& ip, unsigned short port) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        NetworkHandler::send_data(socket, "W", ip, port);
        std::cout << "w was pressed \n";
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        NetworkHandler::send_data(socket, "A", ip, port);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        NetworkHandler::send_data(socket, "S", ip, port);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        NetworkHandler::send_data(socket, "D", ip, port);
    }
}






int run() {


  //init
  NetworkHandler::init();
 
  
  sf::RenderWindow Window;
  Window.create(sf::VideoMode({500, 500}), "Space War", sf::State::Windowed);
  //Window.setVisible(false);  // hide the window
  

  //main loop
  std::array<char, 200> buffer{};
  
  while (true) {
    check_and_send_wasd(NetworkHandler::socket, NetworkHandler::serverIp, NetworkHandler::serverPort);
    NetworkHandler::receive_data(NetworkHandler::socket, buffer); //if not data keeps running
    buffer.fill('\0');
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  return 0;
}
