#ifndef __INVERTED_PENGUIN__TERMS__TERM_HPP__
#define __INVERTED_PENGUIN__TERMS__TERM_HPP__

#include <string>
#include <stddef.h>

namespace inverted_penguin {
  namespace terms {

    struct Term {
      std::string text;   ///< Text that makes up term
      uint32_t position;  ///< Position in stream

      Term() : text(), position() { }
      Term(const std::string& t, uint32_t p) : text(t), position(p) { }
      Term(std::string&& t, uint32_t p) : text(std::move(t)), position(p) { }
      Term(const Term&) = default;
      Term(Term&&) = default;

      bool isEmpty() const { return text.empty(); }
      bool notEmpty() const { return !text.empty(); }
      
      Term& operator=(const Term&) = default;
      Term& operator=(Term&&) = default;

      bool operator==(const Term& other) const {
	return (position == other.position) && (text == other.text);
      }

      bool operator!=(const Term& other) const {
	return (position != other.position) || (text != other.text);
      }

      static Term empty() {
	static const Term ZERO("", 0);
	return ZERO;
      }

    };
    
  }
}

#endif
