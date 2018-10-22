#include "Utf8DecodeError.hpp"
#include <iomanip>
#include <sstream>

using namespace inverted_penguin::tokens;

Utf8DecodeError::Utf8DecodeError(const std::string& details)
    : InvertedPenguinException(details) {
  // Intentionally left blank
}

Utf8DecodeError::~Utf8DecodeError() {
  // Intentionally left blank
}

Utf8DecodeError Utf8DecodeError::illegalStartByte(uint32_t c) {
  std::ostringstream msg;
  msg << "UTF8DecodeError: Illegal start byte 0x" << std::hex << std::setw(2)
      << std::setfill('0') << c;
  return Utf8DecodeError(msg.str());
}

std::string Utf8DecodeError::createDetailsFromSequence_(
    const std::string& pattern, const std::vector<uint8_t>& sequence
) {
  auto i = pattern.find("%s");
  decltype(i) last = 0;
  std::ostringstream msg;

  msg << "Utf8DecodeError: ";

  while (i != std::string::npos) {
    if (last < i) {
      msg << pattern.substr(last, i - last);
    }
    for (auto j = sequence.begin(); j != sequence.end(); ++j) {
      if (j != sequence.begin()) {
	msg << ", ";
      }
      msg << "0x" << std::hex << std::setw(2) << std::setfill('0') << *j;
    }
    last = i + 2;
    i = pattern.find("%s", last);
  }
  
  if (last < pattern.size()) {
    msg << pattern.substr(last);
  }

  return msg.str();
}

Utf8DecodeError Utf8DecodeError::emptySequence() {
  return Utf8DecodeError("Utf8DecodeError: Cannot decode empty sequence");
}
