#include "Utf8EncodeError.hpp"
#include <iomanip>
#include <sstream>

using namespace inverted_penguin::tokens;

Utf8EncodeError::Utf8EncodeError(const std::string& details)
    : InvertedPenguinException(details) {
}

Utf8EncodeError::~Utf8EncodeError() {
  // Intentionally left blank
}

Utf8EncodeError Utf8EncodeError::codepointTooLarge(uint32_t c) {
  std::ostringstream msg;
  msg << "UTF8EncodeError: Codepoint (0x" << std::hex << c
      << ") is too large to encode.  Maximum value is 0x80000000";
  return Utf8EncodeError(msg.str());
}
