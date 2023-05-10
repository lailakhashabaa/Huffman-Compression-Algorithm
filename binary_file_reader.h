
#ifndef _BINARY_FILE_READER_H_
#define _BINARY_FILE_READER_H_

#include <fstream>
#include <string>


class BinaryFileReader
{
  public:
    
    BinaryFileReader(const std::string& fileName);
    ~BinaryFileReader();
    bool getNextBit();
    char getNextByte();
    void reset();
    void close();
    bool hasBits() const;
    bool hasBytes() const;

  private:
    std::ifstream file_;
    char currentByte_;
    int currentBit_;
    int maxBytes_;
    int numRead_;
    int paddingBits_;
    bool needsNextByte() const;
    void readNextByte();
};

#endif
