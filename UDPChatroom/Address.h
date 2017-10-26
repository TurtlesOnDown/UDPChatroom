

class Address
{
public:

  Address();

  Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned int port);

  Address(unsigned int address, unsigned int port);

  unsigned int GetAddress() const;
  void SetAddress(unsigned int addr, unsigned int prt) { address = addr; port = prt; };

  unsigned char GetA() const;
  unsigned char GetB() const;
  unsigned char GetC() const;
  unsigned char GetD() const;

  unsigned short GetPort() const;

private:

  unsigned int address;
  unsigned int port;
};
