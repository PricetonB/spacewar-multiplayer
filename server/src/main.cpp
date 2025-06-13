//SFML UDP SERVER CODE 
//connect - tcp - used by sf::tcpsocket to connect to listening server
//listen - tcp - used by sf::tcplistener on server to listen for connections
//accept - tcp - used by sf::tcplistener on server to accept new connections
//bind - udp - used by sf::udpsocket to bind to socket on client and server
//send - used by udp and tcp on server and client to send data
//receive - used by udp and tcp on server and client to reveive data
// with udp only reveiving sockets need to bind. if send only just call send 

#include <SFML/Network.hpp>
#include <cstddef>
#include <iostream>
#include <array>
#include <thread>
#include <unordered_map>













struct PlayerState {
    int32_t x = 0;
    int32_t y = 0;
};





//========================================================



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




bool receive_data(sf::UdpSocket& socket, std::array<char, 200>& receiving_buffer, std::unordered_map<std::string, unsigned short>& ipMap, std::size_t& received) {
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







//========================================================

std::string pack_transform_component(int32_t x, int32_t y) {
    std::string buffer;
    char componentID = 1;
    int32_t length = sizeof(x) + sizeof(y); // 8 bytes

    buffer.append(&componentID, 1);
    buffer.append(reinterpret_cast<const char*>(&length), sizeof(length));
    buffer.append(reinterpret_cast<const char*>(&x), sizeof(x));
    buffer.append(reinterpret_cast<const char*>(&y), sizeof(y));

    return buffer;
}









//========================================================

void handle_input(const std::string& input,
                  PlayerState& player,
                  sf::UdpSocket& socket,
                  std::unordered_map<std::string, unsigned short>& ipMap) {

    if (input == "W") {
        player.y += 1;
    } else if (input == "S") {
        player.y -= 1;
    } else if (input == "A") {
        player.x -= 1;
    } else if (input == "D") {
        player.x += 1;
    } else {
        std::cout << "input not recognized in handle input \n";
        return; // unhandled input, ignore
    }

    std::cout << "Updated position: x=" << player.x << " y=" << player.y << "\n";

    std::string message = pack_transform_component(player.x, player.y);

    // Send updated position to all clients
    send_data_to_party(socket, message, ipMap);
}


//========================================================








int main() {
  PlayerState playerState{};
  
  const unsigned short serverPort = 54000;
  const sf::IpAddress serverIp(127,0,0,1); // replace with your server's actual LAN IP
  sf::UdpSocket socket;
  std::unordered_map<std::string, unsigned short> ip_port_Map;
  std::string confirmationMessage("i have received a message");
  std::array<char, 200> buffer;
  std::size_t received;


  //init
  if (socket.bind(serverPort) != sf::Socket::Status::Done) {
      std::cerr << "Failed to bind server UDP socket\n";
      return 1;
  }
  socket.setBlocking(false);
  std::cout << "Server listening on port " << serverPort << "...\n";


 //main loop 
  while (true) {
    if (receive_data(socket, buffer, ip_port_Map, received)) {
      std::string input(buffer.data(), received);
      handle_input(input, playerState, socket, ip_port_Map);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  return 0;
}





