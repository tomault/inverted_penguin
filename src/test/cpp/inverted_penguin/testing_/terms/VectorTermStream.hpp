#ifndef __INVERTED_PENGUIN__TESTING__TERMS__VECTORTERMSTREAM_HPP__
#define __INVERTED_PENGUIN__TESTING__TERMS__VECTORTERMSTREAM_HPP__

#include <inverted_penguin/terms/TermStream.hpp>
#include <inverted_penguin/terms/DynamicTermStreamWrapper.hpp>
#include <iostream>
#include <initializer_list>
#include <vector>
#include <stdint.h>

namespace inverted_penguin {
  namespace terms {

    class VectorTermStream :
        public inverted_penguin::terms::TermStream<VectorTermStream> {
    public:
      typedef inverted_penguin::terms::Term Term;
	
    public:
      VectorTermStream(const std::vector<Term>& terms,
		       bool canBeReset = true):
	  terms_(terms), current_(terms_.begin()), canReset_(canBeReset) {
      }
      VectorTermStream(std::vector<Term>&& terms,
		       bool canBeReset = true):
	  terms_(std::move(terms)), current_(terms_.begin()),
	  canReset_(canBeReset) {
      }
      VectorTermStream(const std::initializer_list<Term>& terms,
		       bool canBeReset = true):
	terms_(terms), current_(terms_.begin()), canReset_(canBeReset) {
      }
      VectorTermStream(const VectorTermStream& other):
	  terms_(other.terms_),
	  current_(terms_.begin() + (other.current_ - other.terms_.begin())),
	  canReset_(other.canReset_) {
      }
	
      VectorTermStream(VectorTermStream&&) = default;

      bool empty() const { return terms_.empty(); }
      size_t size() const { return terms_.size(); }
      const Term& termAt(uint32_t i) const { return terms_[i]; }
	
      Term next() {
	if (current_ == terms_.end()) {
	  return Term::empty();
	} else {
	  return *current_++;
	}
      }

      bool reset() {
	if (canReset_) {
	  current_ = terms_.begin();
	  return true;
	} else {
	  return false;
	}
      }

      VectorTermStream& operator=(const VectorTermStream& other) {
	if (this != &other) {
	  terms_ = other.terms_;
	  current_ = terms_.begin() + (other.current_ - other.terms_.begin());
	  canReset_ = other.canReset_;
	}
	return *this;
      }
	
      VectorTermStream& operator=(VectorTermStream&&) = default;

    private:
      std::vector<Term> terms_;
      std::vector<Term>::const_iterator current_;
      bool canReset_;
    };

    typedef inverted_penguin::terms::DynamicTermStreamWrapper<
	        VectorTermStream
            >
            DynamicVectorTermStream;

  }
}
#endif
