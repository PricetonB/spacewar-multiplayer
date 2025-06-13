//example scene


#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <cstddef>
#include <iostream>
#include <array>
#include <string>
#include <thread>
#include <SFML/Graphics.hpp>
#include "NetworkHandler.cpp"
#include <cstring>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <algorithm> //for any_of function





std::array<char, 200> buffer{};
std::unordered_map<int, std::vector<char>> componentMap;


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





void deserialize_components(const std::array<char, 200>& buffer, std::size_t received,
                            std::unordered_map<int, std::vector<char>>& componentMap) {
  std::size_t offset = 0;

  while (offset + sizeof(char) + sizeof(int32_t) <= received) {
    // Read componentID
    char componentID = buffer[offset];
    offset += sizeof(char);

    // Read length of buffer
    int32_t length;
    std::memcpy(&length, &buffer[offset], sizeof(int32_t));
    offset += sizeof(int32_t);

    // Bounds check
    if (offset + length > received) {
      std::cerr << "Invalid component buffer length: " << length << "\n";
      break;
    }

    // Extract component buffer
    std::vector<char> componentData(length);
    std::memcpy(componentData.data(), &buffer[offset], length);
    offset += length;

    // Store in map
    componentMap[static_cast<int>(componentID)] = std::move(componentData);
  }
}

bool buffer_has_data(const std::array<char, 200>& buffer) {
  return std::any_of(buffer.begin(), buffer.end(), [](char c) { return c != '\0'; });
}






//===============================================

void print_component_map(const std::unordered_map<int, std::vector<char>>& componentMap) {
    for (const auto& [componentID, data] : componentMap) {
        std::cout << "Component ID: " << static_cast<int>(componentID) 
                  << ", size: " << data.size();

        if (componentID == 1 && data.size() >= 8) { // Transform component
            int32_t x, y;
            std::memcpy(&x, &data[0], sizeof(x));
            std::memcpy(&y, &data[sizeof(x)], sizeof(y));
            std::cout << ", x: " << x << ", y: " << y;
        } else {
            std::cout << ", could not recognize transform component";
        }

        std::cout << "\n";
    }
}





//===============================================




int run() {


  //init
  NetworkHandler::init();
 
  
  sf::RenderWindow Window;
  Window.create(sf::VideoMode({500, 500}), "Space War", sf::State::Windowed);
  //Window.setVisible(false);  // hide the window

  std::size_t bytesReceived;

  //main loop
  while (true) {
    check_and_send_wasd(NetworkHandler::socket, NetworkHandler::serverIp, NetworkHandler::serverPort);
    NetworkHandler::receive_data(NetworkHandler::socket, buffer, bytesReceived); 
    if (buffer_has_data(buffer)) {
      deserialize_components(buffer, bytesReceived, componentMap);
      print_component_map(componentMap);

      buffer.fill('\0'); // clear the buffer only if it had data
    }



    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  return 0;
}
