#ifndef __INVERTED_PENGUIN__TOKENS__UNICODEUTILS_HPP__
#define __INVERTED_PENGUIN__TOKENS__UNICODEUTILS_HPP__

#include <inverted_penguin/tokens/Utf8DecodeError.hpp>
#include <inverted_penguin/tokens/Utf8EncodeError.hpp>

#include <exception>
#include <iterator>
#include <tuple>
#include <stddef.h>

namespace inverted_penguin {
  namespace tokens {
    namespace detail {
      
      template <typename Iterator>
      std::tuple<uint32_t, Iterator> decodeTrailingUtf8(const Iterator& start,
							const Iterator& end,
							uint32_t c,
							uint32_t n) {
	Iterator p(start);
	while (n) {
	  ++p;
	  if (p == end) {
	    throw Utf8DecodeError::prematureEndOfSequence(start, p);
	  } else {
	    uint32_t b = *p;
	    if ((b < 0x80) || (b > 0xBF)) {
	      throw Utf8DecodeError::illegalTrailingByte(start, p);
	    }
	    c = (c << 6) | (b & 0x3F);
	    --n;
	  }
	}
	return std::tuple<uint32_t, Iterator>(c, ++p);
      }
    }

    template <typename Iterator>
    std::tuple<uint32_t, Iterator> decodeUtf8(const Iterator& start,
					      const Iterator& end) {
      if (start == end) {
	throw Utf8DecodeError::emptySequence();
      } else {
	uint32_t c = *start;
	if (c < 0x80) {
	  Iterator next(start);
	  return std::tuple<uint32_t, Iterator>(c, ++next);
	} else if (c < 0xC0) {
	  throw Utf8DecodeError::illegalStartByte(c);
	} else if (c < 0xE0) {
	  return detail::decodeTrailingUtf8(start, end, c & 0x1F, 1);
	} else if (c < 0xF0) {
	  return detail::decodeTrailingUtf8(start, end, c & 0x0F, 2);
	} else if (c < 0xF8) {
	  return detail::decodeTrailingUtf8(start, end, c & 0x07, 3);
	} else if (c < 0xFC) {
	  return detail::decodeTrailingUtf8(start, end, c & 0x03, 4);
	} else if (c < 0xFE) {
	  return detail::decodeTrailingUtf8(start, end, c & 0x01, 5);
	} else {
	  throw Utf8DecodeError::illegalStartByte(c);
	}
      }
    }

    template <typename Iterator>
    Iterator encodeUtf8(Iterator p, uint32_t c) {
      if (c < 0x80) {
	*p = c; ++p;
      } else if (c < 0x800) {
	*p = 0xC0 | (c >> 6); ++p;
	*p = 0x80 | (c & 0x3F); ++p;
      } else if (c < 0x10000) {
	*p = 0xE0 | (c >> 12); ++p;
	*p = 0x80 | ((c >> 6) & 0x3F); ++p;
	*p = 0x80 | (c & 0x3F); ++p;
      } else if (c < 0x200000) {
	*p = 0xF0 | (c >> 18); ++p;
	*p = 0x80 | ((c >> 12) & 0x3F); ++p;
	*p = 0x80 | ((c >>  6) & 0x3F); ++p;
	*p = 0x80 | (c & 0x3F); ++p;
      } else if (c < 0x4000000) {
	*p = 0xF8 | (c >> 24); ++p;
	*p = 0x80 | ((c >> 18) & 0x3F); ++p;
	*p = 0x80 | ((c >> 12) & 0x3F); ++p;
	*p = 0x80 | ((c >>  6) & 0x3F); ++p;
	*p = 0x80 | (c & 0x3F); ++p;
      } else if (c < 0x80000000) {
	*p = 0xFC | (c >> 30); ++p;
	*p = 0x80 | ((c >> 24) & 0x3F); ++p;
	*p = 0x80 | ((c >> 18) & 0x3F); ++p;
	*p = 0x80 | ((c >> 12) & 0x3F); ++p;
	*p = 0x80 | ((c >>  6) & 0x3F); ++p;
	*p = 0x80 | (c & 0x3F); ++p;
      } else {
	throw Utf8EncodeError::codepointTooLarge(c);
      }
      return p;
    }

    template <typename DecodeIterator, typename EncodeIterator,
	      typename Function>
    EncodeIterator transformUtf8Sequence(const DecodeIterator& start,
					 const DecodeIterator& end,
					 const EncodeIterator& out,
					 const Function& f) {
      DecodeIterator p(start);
      EncodeIterator q(out);
      uint32_t c;
      
      while (p != end) {
	std::tie(c, p) = decodeUtf8(p, end);
	q = encodeUtf8(q, f(c));
      }
      return q;
    }

    uint32_t toLowerCase(uint32_t c);
  }
}
#endif

