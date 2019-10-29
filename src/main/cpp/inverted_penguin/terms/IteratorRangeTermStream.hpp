#ifndef __INVERTED_PENGUIN__TERMS__ITERATORRANGETERMSTREAM_HPP__
#define __INVERTED_PENGUIN__TERMS__ITERATORRANGETERMSTREAM_HPP__

#include <inverted_penguin/terms/DynamicTermStreamWrapper.hpp>
#include <inverted_penguin/terms/TermStream.hpp>

namespace inverted_penguin {
  namespace terms {

    /** @brief TermStream whose content is a sequence of strings defined
     *         by a pair of iterators.
     */
    template <typename Iterator>
    class IteratorRangeTermStream:
        public TermStream< IteratorRangeTermStream<Iterator> > {
    public:
      IteratorRangeTermStream(const Iterator& start, const Iterator& end):
	  start_(start), end_(end), current_(start), position_(0) {
	// Intentionally left blank 
      }

      Term next() {
	if (current_ == end_) {
	  return Term::empty();
	} else {
	  Term t(*current_, position_);
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

    template <typename Iterator>
    using DynamicIteratorRangeTermStream =
        DynamicTermStreamWrapper< IteratorRangeTermStream<Iterator> >;
  }
}

#endif
