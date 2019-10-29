#ifndef __INVERTED_PENGUIN__TERMS__STREAMS__WHITESPACETOKENIZER_HPP__
#define __INVERTED_PENGUIN__TERMS__STREAMS__WHITESPACETOKENIZER_HPP__

#include <inverted_penguin/StaticConfiguration.hpp>
#include <tuple>
#include <ctype.h>

namespace inverted_penguin {
  namespace terms {
    namespace streams {

      template <typename InputBuffer>
      class WhitespaceTokenizer : public TokenStream<WhitespaceTokenizer> {
      public:
	WhitespaceTokenizer(InputBuffer& buffer):
	    buffer_(buffer), begin_(nullptr), end_(nullptr_), current_(nullptr),
	    position_(0) {
	}

	Term next() {
	  // Fill the buffer if needed
	  if ((current_ == end_) && !fillBuffer_()) {
	    return Term::empty();
	  }

	  // Skip whitespace to term
	  while (::isspace_(current_)) {
	    ++current_;
	    if ((current_ == end_) && !fillBuffer_()) {
	      return Term::empty();
	    }
	  }

	  const char* start = current_;
	  while (!::isspace(current_)) {
	    ++current_;
	    if (current_ >= end_) {
	      size_t offset = start_ - begin_;
	      if (!offset) {
		// Term does not fit in buffer
		throw TermTooLongError(PISTIS_EX_HERE);
	      }
	      if (!fillBuffer(end_ - start_)) {
		// At EOF
		return Term(std::string(begin_ + offset, end_), position_++);
	      }
	      start = begin_ + offset;
	      assert(current_ < end_);
	    }
	  }

	  return Term(std::string(start, current_), current_++);
	}

	bool reset() {
	  if (!buffer_.reset()) {
	    return false;
	  } else {
	    begin_ = nullptr;
	    end_ = nullptr;
	    current_ = nullptr;
	    position_ = 0;
	    return true;
	  }
	}
	
      private:
	InputBuffer& buffer_;
	const char* begin_;
	const char* current_;
	const char* end_;
	uint32_t position_;

	bool fillBuffer_(size_t numToKeep = 0) {
	  const size_t offset = ((current_ + numToKeep) >= end_ ?
	                             ((current_ + numToKeep) - end_) : 0;
	  buffer_.fill();
	  std::tie(begin_, end_) = buffer_.bounds();
	  current_ = begin_ + offset;
	  return begin_ < end_;
	}
      };
      
    }
  }
}
#endif
