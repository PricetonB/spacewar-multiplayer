//SFML UDP SERVER CODE 
//connect - tcp - used by sf::tcpsocket to connect to listening server
//listen - tcp - used by sf::tcplistener on server to listen for connections
//accept - tcp - used by sf::tcplistener on server to accept new connections
//bind - udp - used by sf::udpsocket to bind to socket on client and server
//send - used by udp and tcp on server and client to send data
//receive - used by udp and tcp on server and client to reveive data
// with udp only reveiving sockets need to bind. if send only just call send 

#include <SFML/Network.hpp>
#include <iostream>
#include <array>
#include <thread>
#include <unordered_map>



void send_data(sf::UdpSocket& socket, const std::string& message, const sf::IpAddress& ip, const unsigned short port) {
      if (socket.send(message.c_str(), message.size(), ip, port) != sf::Socket::Status::Done) {
            std::cerr << "Failed to send message to server\n";
        }
        std::cout << "Sent: " << message << "\n";
        std::cout << "================================ \n";
}

//========================================================




void send_data_to_party(sf::UdpSocket& socket, const std::string& message, std::unordered_map<std::string, unsigned short>& ipMap) {
  for (const auto& [key, port] : ipMap) {
    auto delimiter = key.find(':');
    if (delimiter == std::string::npos) {
        std::cerr << "Invalid key format: " << key << "\n";
        continue;
    }

    std::string ipStr = key.substr(0, delimiter);
    std::string portStr = key.substr(delimiter + 1);

    std::optional<sf::IpAddress> resolved = sf::IpAddress::resolve(ipStr);
    unsigned short actualPort = static_cast<unsigned short>(std::stoi(portStr));

    if (resolved) {
        send_data(socket, message, *resolved, actualPort);
    } else {
        std::cerr << "Failed to resolve IP address: " << ipStr << "\n";
    }
  }
}





//========================================================




bool receive_data(sf::UdpSocket& socket, std::array<char, 200>& receiving_buffer, std::unordered_map<std::string, unsigned short>& ipMap) {
  std::size_t received;
  unsigned short senderPort;
  std::optional<sf::IpAddress> optionalClientIp;

  sf::Socket::Status result = socket.receive(receiving_buffer.data(),
                                             receiving_buffer.size(),
                                             received, optionalClientIp,
                                             senderPort);
  if (result == sf::Socket::Status::Done) {

    
    if (optionalClientIp) {  
      sf::IpAddress convertedClientIp = *optionalClientIp;
      std::string key = convertedClientIp.toString() + ":" + std::to_string(senderPort);

      if (ipMap.find(key) == ipMap.end()) {
          ipMap[key] = senderPort;
          std::cout << "New client added: " << key << ":" << senderPort << "\n";
      }
      else {
          std::cout << "existing client: " << key << ":" << senderPort << "\n";
      }
    
    } else {
      std::cerr << "Received data but no client IP address\n";
    }
    std::string reply(receiving_buffer.data(), received);
    std::cout << "Received from server: " << reply << "\n";
    std::cout << "Size in bytes:" << received << "\n";
    return true;

  }
  else {
    return false;
  }
}







int main() {
  
  const unsigned short serverPort = 54000;
  const sf::IpAddress serverIp(127,0,0,1); // replace with your server's actual LAN IP
  sf::UdpSocket socket;
  std::unordered_map<std::string, unsigned short> ip_port_Map;
  std::string confirmationMessage("i have received a message");
  std::array<char, 200> buffer;


  //init
  if (socket.bind(serverPort) != sf::Socket::Status::Done) {
      std::cerr << "Failed to bind server UDP socket\n";
      return 1;
  }
  socket.setBlocking(false);
  std::cout << "Server listening on port " << serverPort << "...\n";


 //main loop 
  while (true) {
    if (receive_data(socket, buffer, ip_port_Map)) {
      //this is where we will update our component data based on received data
      send_data_to_party(socket, confirmationMessage, ip_port_Map);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  return 0;
}





