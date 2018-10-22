#ifndef __INVERTED_PENGUIN__TOKENS__KSTEMMAPPER_HPP__
#define __INVERTED_PENGUIN__TOKENS__KSTEMMAPPER_HPP__

#include <inverted_penguin/tokens/TokenStream.hpp>
#include <inverted_penguin/tokens/TokenStreamModifier.hpp>
#include <inverted_penguin/tokens/detail/KrovetzStemmer.hpp>

#include <iostream>

namespace inverted_penguin {
  namespace tokens {

    class KStemMapper : public TokenStreamModifier<KStemMapper> {
    public:
      template <typename S>
      Token next(TokenStream<S>& stream) const {
	return this->apply(stream.self().next());
      }

      Token apply(const Token& t) const {
	if ((t.text.size() < 2) ||
	    (t.text.size() >= (detail::KrovetzStemmer::MAX_WORD_LENGTH - 1))) {
	  return t;
	} else {
	  char buffer[detail::KrovetzStemmer::MAX_WORD_LENGTH * 2];
	  int n = kStemmer_.kstem_stem_tobuffer(t.text.c_str(), buffer, true);
	  return n ? Token(std::string(buffer, n - 1), t.position) : t;
	}
      }

    private:
      detail::KrovetzStemmer kStemmer_;
    };

  }
}
#endif
