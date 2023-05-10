#include <iostream>
#include "binary_file_writer.h"

using namespace std;
BinaryFileWriter::BinaryFileWriter(const std::string& fileName)
    : file_(fileName, ios::binary), currentByte_('\0'), currentBit_(7)
{
}

BinaryFileWriter::~BinaryFileWriter()
{
    close();
}

void BinaryFileWriter::close()
{
    if (!file_.is_open())
        return;
    if (currentBit_ != 7) {
        char paddingBitsAsChar = static_cast<char>(currentBit_ + 1);
        writeCurrentByte();
        currentByte_ = paddingBitsAsChar;
        writeCurrentByte();
    } else {
        currentByte_ = 0;
        writeCurrentByte();
    }
    file_.close();
}

void BinaryFileWriter::writeBit(bool bit)
{
    currentByte_ = currentByte_ | (static_cast<char>(bit) << currentBit_);
    --currentBit_;
    if (currentBit_ == -1)
        writeCurrentByte();
}

void BinaryFileWriter::writeByte(char byte)
{
    for (int i = 0; i < 8; ++i)
        writeBit(((byte >> (7 - i)) & 1) == 1);
}

void BinaryFileWriter::writeCurrentByte()
{
    currentBit_ = 7;
    file_.write(&currentByte_, 1);
    currentByte_ = '\0';
}
