#include "UnicodeUtils.hpp"

using namespace inverted_penguin::terms;

uint32_t inverted_penguin::terms::toLowerCase(uint32_t c) {
  // This covers LATIN-1
  // TODO: Extend to handle all of unicode
  if (((c >= 'A') && (c <= 'Z')) || ((c >= 0xC0) && (c <= 0xD6)) ||
      ((c >= 0xD8) && (c <= 0xDE))) {
    return c + 0x20;
  }
  return c;
}
