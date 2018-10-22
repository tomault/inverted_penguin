#ifndef __INVERTED_PENGUIN__TOKENS__TOKENSTREAM_HPP__
#define __INVERTED_PENGUIN__TOKENS__TOKENSTREAM_HPP__

#include <inverted_penguin/tokens/TokenStream_.hpp>
#include <inverted_penguin/tokens/ModifiedTokenStream_.hpp>

namespace inverted_penguin {
  namespace tokens {
    template <typename M> class TokenStreamModifier;
    template <typename M1, typename M2> class TokenStreamModifierSequence;

    template <typename S, typename M>
    ModifiedTokenStream<S, M> operator+(
        const TokenStream<S>& tokenStream,
	const TokenStreamModifier<M>& modifier
    ) {
      return ModifiedTokenStream<S, M>(tokenStream.self(), modifier.self());
    }

    template <typename S, typename M1, typename M2>
    auto operator+(const TokenStream<S>& stream,
		   const TokenStreamModifierSequence<M1, M2>& modifier) {
      return (stream + modifier.firstModifier()) + modifier.secondModifier();
    }

  }
}
#endif
