#ifndef __INVERTED_PENGUIN__TOKENS__ITERATORRANGETOKENSTREAM_HPP__
#define __INVERTED_PENGUIN__TOKENS__ITERATORRANGETOKENSTREAM_HPP__

#include <inverted_penguin/tokens/TokenStream.hpp>

namespace inverted_penguin {
  namespace tokens {

    /** @brief TokenStream whose content is a sequence of strings defined
     *         by a pair of iterators.
     */
    template <typename Iterator>
    class IteratorRangeTokenStream
        : public TokenStream< IteratorRangeTokenStream<Iterator> > {
    public:
      IteratorRangeTokenStream(const Iterator& start, const Iterator& end):
	  start_(start), end_(end), current_(start), position_(0) {
	// Intentionally left blank 
      }

      Token next() {
	if (current_ == end_) {
	  return Token::empty();
	} else {
	  Token t(*current_, position_);
	  ++current_;
	  ++position_;
	  return std::move(t);
	}
      }

      bool reset() {
	current_ = start_;
	position_ = 0;
	return true;
      }
      
    private:
      Iterator start_;
      Iterator end_;
      Iterator current_;
      uint32_t position_;
    };
    
  }
}

#endif

