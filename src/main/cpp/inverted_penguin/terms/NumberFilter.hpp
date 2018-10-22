#ifndef __INVERTED_PENGUIN__TOKENS__NUMBERFILTER_HPP__
#define __INVERTED_PENGUIN__TOKENS__NUMBERFILTER_HPP__

#include <inverted_penguin/tokens/TokenStreamModifier.hpp>
#include <inverted_penguin/tokens/UnicodeUtils.hpp>

namespace inverted_penguin {
  namespace tokens {

    class NumberFilter {
    public:
      template <typename S>
      Token next(TokenStream<S>& stream) const {
	Token t = stream.self().next();
	while (t.notEmpty() && isAllDigits_(t.text)) {
	  t = stream.self().next();
	}
	return t;
      }

      Token apply(Token t) const {
	return isAllDigits_(t.text) ? Token::empty() : t;
      }

    private:
      static bool isAllDigits_(const std::string& s) {
	auto p = s.begin();
	uint32_t c;
	
	while (p != s.end()) {
	  std::tie(c, p) = decodeUtf8(p);
	  if ((c < '0') || (c > '9')) {
	    return false;
	  }
	}
	return true;
      }
    };
    
  }
}
#endif

