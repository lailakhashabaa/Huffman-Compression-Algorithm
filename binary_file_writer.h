
#ifndef _BINARY_FILE_WRITER_H_
#define _BINARY_FILE_WRITER_H_

#include <fstream>
#include <string>


class BinaryFileWriter
{
  public:
    BinaryFileWriter(const std::string& fileName);
    ~BinaryFileWriter();
    void writeBit(bool bit);
    void writeByte(char byte);
    void close();

  private:
    std::ofstream file_; 
    char currentByte_;
    int currentBit_;
    void writeCurrentByte();
};
#endif
