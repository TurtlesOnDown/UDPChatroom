#include "Address.h"

Address::Address() {
  address = (127 << 24) | (0 << 16) | (0 << 8) | 1;
  port = 0;
}

Address::Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned int port) {
  address = (a << 24) | (b << 16) | (c << 8) | d;
  port = port;
}

Address::Address(unsigned int address, unsigned int port) {
  address = address;
  port = port;
}

unsigned int Address::GetAddress() const {
  return address;
}

unsigned char Address::GetA() const {
  return (unsigned char)(address >> 24);
}

unsigned char Address::GetB() const {
  return (unsigned char)(address >> 16);
}

unsigned char Address::GetC() const {
  return (unsigned char) (address >> 8);
}

unsigned char Address::GetD() const {
  return (unsigned char)address;
}

unsigned short Address::GetPort() const {
  return port;
}