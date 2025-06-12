//SFML UDP CLIENT CODE 
//connect - tcp - used by sf::tcpsocket to connect to listening server
//listen - tcp - used by sf::tcplistener on server to listen for connections
//accept - tcp - used by sf::tcplistener on server to accept new connections
//bind - udp - used by sf::udpsocket to bind to socket on client and server
//send - used by udp and tcp on server and client to send data
//receive - used by udp and tcp on server and client to reveive data
// with udp only reveiving sockets need to bind. if send only just call send 

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <iostream>
#include <array>
#include <string>
#include <thread>
#include <SFML/Graphics.hpp>








//===================================================





void send_data(sf::UdpSocket& socket, const std::string& message, const sf::IpAddress& ip, const unsigned short port) {
      if (socket.send(message.c_str(), message.size(), ip, port) != sf::Socket::Status::Done) {
            std::cerr << "Failed to send message to server\n";
        }
        std::cout << "Sent: " << message << "\n";
}





//===================================================
















void receive_data(sf::UdpSocket& socket, std::array<char, 200>& receiving_buffer) {
  std::size_t received;
  unsigned short senderPort;
  std::optional<sf::IpAddress> optionalServerIp;

  sf::Socket::Status result = socket.receive(receiving_buffer.data(),
                                             receiving_buffer.size(),
                                             received, optionalServerIp,
                                             senderPort);
  if (result == sf::Socket::Status::Done) {
    std::string reply(receiving_buffer.data(), received);
    std::cout << "Received from server: " << reply << "\n";
    std::cout << "Size in bytes:" << received << "\n";
    std::cout << "====================================== \n";
  }
}


void check_and_send_wasd(sf::UdpSocket& socket, const sf::IpAddress& ip, unsigned short port) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        send_data(socket, "W", ip, port);
        std::cout << "w was pressed \n";
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        send_data(socket, "A", ip, port);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        send_data(socket, "S", ip, port);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        send_data(socket, "D", ip, port);
    }
}









int main() {
  const unsigned short serverPort = 54000;
  const sf::IpAddress serverIp(127,0,0,1); // replace with your server's actual LAN IP
  sf::UdpSocket socket;
 

  //init

  if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Status::Done) {
      std::cerr << "Failed to bind socket\n";
  } else {
      std::cout << "Bound to port: " << socket.getLocalPort() << "\n";
  }



  socket.setBlocking(false);
  std::cout << "connection complete \n";
  
  
  sf::RenderWindow Window;
  Window.create(sf::VideoMode({500, 500}), "Space War", sf::State::Windowed);
  //Window.setVisible(false);  // hide the window
  


  //main loop
  std::array<char, 200> buffer{};
  
  while (true) {
    check_and_send_wasd(socket, serverIp, serverPort);
    receive_data(socket, buffer); //if not data keeps running
    buffer.fill('\0');
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  return 0;
}
