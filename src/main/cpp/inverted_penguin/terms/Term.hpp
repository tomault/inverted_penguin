#ifndef __INVERTED_PENGUIN__TOKENS__TOKEN_HPP__
#define __INVERTED_PENGUIN__TOKENS__TOKEN_HPP__

#include <string>
#include <stddef.h>

namespace inverted_penguin {
  namespace tokens {

    struct Token {
      std::string text;   ///< Text that makes up token
      uint32_t position;  ///< Position in stream

      Token() : text(), position() { }
      Token(const std::string& t, uint32_t p) : text(t), position(p) { }
      Token(std::string&& t, uint32_t p) : text(std::move(t)), position(p) { }
      Token(const Token&) = default;
      Token(Token&&) = default;

      bool isEmpty() const { return text.empty(); }
      bool notEmpty() const { return !text.empty(); }
      
      Token& operator=(const Token&) = default;
      Token& operator=(Token&&) = default;

      bool operator==(const Token& other) const {
	return (position == other.position) && (text == other.text);
      }

      bool operator!=(const Token& other) const {
	return (position != other.position) || (text != other.text);
      }

      static Token empty() {
	static const Token ZERO("", 0);
	return ZERO;
      }

    };
    
  }
}

#endif
