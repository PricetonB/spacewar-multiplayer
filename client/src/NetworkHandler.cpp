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





namespace NetworkHandler {
  const unsigned short serverPort = 54000;
  const sf::IpAddress serverIp(127,0,0,1); // replace with your server's actual LAN IP
  sf::UdpSocket socket;



//===================================================



void init() {

  if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Status::Done) {
      std::cerr << "Failed to bind socket\n";
  } else {
      std::cout << "Bound to port: " << socket.getLocalPort() << "\n";
  }


  socket.setBlocking(false);
  std::cout << "connection complete \n";
 
}



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


}


