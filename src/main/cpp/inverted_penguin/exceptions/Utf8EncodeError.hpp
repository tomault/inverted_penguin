#ifndef __INVERTED_PENGUIN__TOKENS__UTF8ENCODEERROR_HPP__
#define __INVERTED_PENGUIN__TOKENS__UTF8ENCODEERROR_HPP__

#include <inverted_penguin/exceptions/InvertedPenguinException.hpp>
#include <stddef.h>

namespace inverted_penguin {
  namespace exceptions {

    class Utf8EncodeError : public InvertedPenguinException {
    public:
      Utf8EncodeError(const std::string& details);
      virtual ~Utf8EncodeError();

      static Utf8EncodeError codepointTooLarge(uint32_t c);
    };
    
  }
}

#endif

