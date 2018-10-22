#ifndef __INVERTED_PENGUIN__TOKENS__TOKENSTREAMMODIFIER_HPP__
#define __INVERTED_PENGUIN__TOKENS__TOKENSTREAMMODIFIER_HPP__

#include <inverted_penguin/tokens/TokenStreamModifier_.hpp>
#include <inverted_penguin/tokens/TokenStreamModifierSequence_.hpp>

namespace inverted_penguin {
  namespace tokens {

    template <typename FirstModifier, typename SecondModifier>
    auto operator+(const TokenStreamModifier<FirstModifier>& first,
		   const TokenStreamModifier<SecondModifier>& second) {
      return TokenStreamModifierSequence<FirstModifier, SecondModifier>(
	  first.self(), second.self()
      );
    }

  }
}

#endif

