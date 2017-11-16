#include <functional>

class Address
{
  friend std::hash<Address>;
public:

  Address();

  Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short prt);

  Address(unsigned int address, unsigned short prt);

  unsigned int GetAddress() const;
  void SetAddress(unsigned int addr, unsigned short prt) { address = addr; port = prt; };
  void SetAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short prt);

  unsigned char GetA() const;
  unsigned char GetB() const;
  unsigned char GetC() const;
  unsigned char GetD() const;

  unsigned short GetPort() const;

  bool operator==(const Address &) const;

private:

  unsigned int address;
  unsigned short port;
};

namespace std {
  template<>
  struct hash<Address>
  {
    std::size_t operator()(const Address& a) const {
      return a.address ^ a.port;
    }
  };
}
