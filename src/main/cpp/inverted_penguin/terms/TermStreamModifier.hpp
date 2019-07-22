#ifndef __INVERTED_PENGUIN__TERMS__TERMSTREAMMODIFIER_HPP__
#define __INVERTED_PENGUIN__TERMS__TERMSTREAMMODIFIER_HPP__

#include <inverted_penguin/terms/TermStreamModifier_.hpp>
#include <inverted_penguin/terms/TermStreamModifierSequence_.hpp>

namespace inverted_penguin {
  namespace terms {

    template <typename FirstModifier, typename SecondModifier>
    auto operator+(const TermStreamModifier<FirstModifier>& first,
		   const TermStreamModifier<SecondModifier>& second) {
      return TermStreamModifierSequence<FirstModifier, SecondModifier>(
	  first.self(), second.self()
      );
    }

  }
}

#endif

