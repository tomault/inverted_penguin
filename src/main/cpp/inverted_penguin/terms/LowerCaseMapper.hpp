#ifndef __INVERTED_PENGUIN__TOKENS__LOWERCASEMAPPER_HPP__
#define __INVERTED_PENGUIN__TOKENS__LOWERCASEMAPPER_HPP__

#include <inverted_penguin/tokens/TokenStreamModifier.hpp>
#include <inverted_penguin/tokens/UnicodeUtils.hpp>
#include <iterator>
#include <vector>

namespace inverted_penguin {
  namespace tokens {

    class LowerCaseMapper : public TokenStreamModifier<LowerCaseMapper> {
    public:
      template <typename S>
      Token next(TokenStream<S>& stream) const {
	return this->apply(stream.self().next());
      }

      Token apply(const Token& t) const {
	if (t.isEmpty()) {
	  return t;
	} else {
	  std::vector<char> lowered;

	  lowered.reserve(t.text.size());
	  transformUtf8Sequence(t.text.begin(), t.text.end(),
				std::back_inserter(lowered),
				[](uint32_t c) { return toLowerCase(c); });
	  return Token(std::string(lowered.begin(), lowered.end()),
		       t.position);
	}
      }
    };
    
  }
}

#endif

