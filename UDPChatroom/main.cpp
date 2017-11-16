#include <iostream>
#include <string>
#include <unordered_map>
#include <thread>

#include "Socket.h"

struct fRecieve {
  Address serverAddress;
  fRecieve(Address a) : serverAddress(a) {};
  void operator()(Socket &s);
};

struct fHost {
  unsigned short port;
  std::unordered_map<Address, std::string> connectedClients;
  fHost(unsigned short p) : port(p) {};
  void operator()(Socket &s);
};

// returns a vector of strings from a string seperated by a delimitor
std::vector<std::string> tokenize(const std::string &, char);

bool programShutdown = false;

std::string USERNAME;
unsigned short MYPORT = 0;

int main() {
  // create socket
  Address targetAddr;
  Socket mySocket;

  const size_t messageSize = 255;
  char message[messageSize];
  // Get Username
  std::cout << "Enter username: ";
  std::getline(std::cin, USERNAME);

  // Decide if hosting or connecting
  unsigned char choice;
  std::cout << "Host(h) or connect(c)? :";
  std::cin >> choice;
  std::cin.ignore(10000, '\n');

  std::thread receiver;
  mySocket.Open();

    // hosting
  if (choice == 'h') {
    // Get port to host on
    std::cout << "Enter port: ";
    std::cin >> MYPORT;
    std::cout << "Running server..." << std::endl;
    // set addres to send to
    targetAddr.SetAddress(127, 0, 0, 1, MYPORT);
    // start thread
    receiver = std::thread(fHost{ MYPORT }, mySocket);

    // connect to ourselves
    mySocket.Send(targetAddr, ('n' + USERNAME).c_str(), USERNAME.size() + 1);
  }
    // connecting
  else if (choice == 'c') {
    std::string IPstring;
    unsigned short targetPort;

    // get IP and port to connect to
    std::cout << "Enter IP: ";
    std::getline(std::cin, IPstring);
    std::cout << "Enter port: ";
    std::cin >> targetPort;
    // Repeat back to user
    std::cout << "Connecting to " << IPstring << ":" << targetPort << std::endl;

    // Tokenize IP
    std::vector<std::string> IPAddr = tokenize(IPstring, '.');
    // set address to send to
    targetAddr.SetAddress(stoi(IPAddr[0]), stoi(IPAddr[1]), stoi(IPAddr[2]), stoi(IPAddr[3]), targetPort);
    // start thread
    receiver = std::thread(fRecieve{ targetAddr }, mySocket);
  }
  
  std::cin.ignore(1000, '\n');

  // start up send loop
  while (!programShutdown) {
    // get new message
    std::string newMessage;
    std::getline(std::cin, newMessage);
    // if message is quit, then quit
    if (newMessage[0] == ':' && newMessage[1] == 'q') {
      programShutdown = true;
      break;
    }
    // parse message
    newMessage = 'm' + newMessage;
    // send message
    mySocket.Send(targetAddr, newMessage.c_str(), newMessage.size());
  }

  receiver.join();

  mySocket.Close();
  return 0;
}

void fHost::operator()(Socket &s) {
  const size_t messageSize = 255;
  char message[messageSize];
  Address newAddress;
  Address ourAddress(127, 0, 0, 1, port);

  s.Bind(port);

  // listen for new clients
  while (!programShutdown) // change this conditional
  {
    int bytes_back = s.Receive(newAddress, message, messageSize);
    if (bytes_back > 0) {
      // check if message is a new client
      if (message[0] == 'n') {
        // if new client, add to client queue
        // TODO: send ack back
        std::string recvName;
        // TODO check if packet from proper program
        // display message on server machine
        for (auto i = 1; i < bytes_back; i++) {
          recvName += message[i];
        }
        connectedClients[newAddress] = recvName;
        std::cout << recvName << " has joined. (" << (int)newAddress.GetA() << (int)newAddress.GetB() << (int)newAddress.GetC() << (int)newAddress.GetD() << ":" << newAddress.GetPort()<< ")" << std::endl;
      }
      //if old client, check message and display
      if (message[0] == 'm') {
        std::string recvMessage;
        // if proper message, send message to client list
        // display message on server machine
        for (auto i = 1; i < bytes_back; i++) {
          recvMessage += message[i];
        }
        recvMessage = connectedClients[newAddress] + ": " + recvMessage;
        for (auto each : connectedClients) {
          if (!(each.first == ourAddress)) s.Send(each.first, ('m' + recvMessage).c_str(), recvMessage.size() + 1);
        }
        std::cout << recvMessage << std::endl;
      }
      
    }
  }

}

void fRecieve::operator()(Socket &s) {
  const size_t messageSize = 255;
  char message[messageSize];
  Address newAddress;

  s.Bind(MYPORT);

  s.Send(serverAddress, ('n' + USERNAME).c_str(), USERNAME.size() + 1);

  while (!programShutdown) // change this conditional
  {
    int bytes_back = s.Receive(newAddress, message, messageSize);
    if (bytes_back > 0) {
      // TODO make sure packet is from proper program
      // TODO recieve ack's
      if (message[0] == 'm') {
        std::string recvMessage;
        // display message on client machine
        for (auto i = 1; i < bytes_back; i++) {
          recvMessage += message[i];
        }
        
        std::cout << recvMessage << std::endl;
      }
    }
  }
}

std::vector<std::string> tokenize(const std::string &input, char delim) {
  std::string newString = input;
  // parse input into tokens
  std::vector<std::string> tokens;
  while (newString.size()) {
    if (newString.find(delim) == std::string::npos) {
      tokens.push_back(newString);
      newString.clear();
    }
    else {
      tokens.push_back(newString.substr(0, newString.find(delim)));
      newString = newString.substr(newString.find(delim) + 1);
    }
  }
  return tokens;
}