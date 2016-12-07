#ifndef _UTILS_H_
#define _UTILS_H_

#include <crypto++/filters.h>

namespace CryptoPP {
    
    class VectorSink : public Bufferless<Sink> {
    public:

      VectorSink(std::vector<uint8_t>& out)
        : _out(&out) {
      }

      size_t Put2(const byte *inString, size_t length, int /*messageEnd*/, bool /*blocking*/) {
        _out->insert(_out->end(), inString, inString + length);
        return 0;
      }

    private:  
      std::vector<uint8_t>* _out;
    };

}

inline void PrintBin(const BinaryData& in)
{
    for (auto data : in)  
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)data << " ";
    std::cout << std::endl;
}

#endif
