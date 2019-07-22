#ifndef __INVERTED_PENGUIN__EXCEPTIONS__UTF8DECODEERROR_HPP__
#define __INVERTED_PENGUIN__EXCEPTIONS__UTF8DECODEERROR_HPP__

#include <inverted_penguin/exceptions/InvertedPenguinException.hpp>
#include <vector>
#include <stdint.h>

namespace inverted_penguin {
  namespace exceptions {

    class Utf8DecodeError : public InvertedPenguinException {
    public:
      Utf8DecodeError(const std::string& details);
      virtual ~Utf8DecodeError();

      template <typename Iterator>
      static Utf8DecodeError prematureEndOfSequence(const Iterator& start,
						    const Iterator& end) {
	std::vector<uint8_t> sequence(start, end);
	return Utf8DecodeError(createDetailsFromSequence_(
	    "UTF-8 encoded character sequence [%s] ended prematurely",
	    sequence
	));
      }

      template <typename Iterator>
      static Utf8DecodeError illegalTrailingByte(const Iterator& start,
						 const Iterator& end) {
	std::vector<uint8_t> sequence(start, end);
	return Utf8DecodeError(createDetailsFromSequence_(
	    "Illegal trailing byte in UTF-8 character sequence [%s]", sequence
        ));
      }

      static Utf8DecodeError illegalStartByte(uint32_t c);
      static Utf8DecodeError emptySequence();

    private:
      static std::string createDetailsFromSequence_(
	  const std::string& pattern,
	  const std::vector<uint8_t>& sequence
      );
    };
  }
}

#endif
