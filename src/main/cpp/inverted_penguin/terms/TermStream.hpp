#ifndef __INVERTED_PENGUIN__TERMS__TERMSTREAM_HPP__
#define __INVERTED_PENGUIN__TERMS__TERMSTREAM_HPP__

#include <inverted_penguin/terms/TermStream_.hpp>
#include <inverted_penguin/terms/ModifiedTermStream_.hpp>

namespace inverted_penguin {
  namespace terms {

    template <typename M> class TermStreamModifier;
    template <typename M1, typename M2> class TermStreamModifierSequence;

    template <typename S, typename M>
    ModifiedTermStream<S, M> operator+(
        const TermStream<S>& termStream,
	const TermStreamModifier<M>& modifier
    ) {
      return ModifiedTermStream<S, M>(termStream.self(), modifier.self());
    }

    template <typename S, typename M1, typename M2>
    auto operator+(const TermStream<S>& stream,
		   const TermStreamModifierSequence<M1, M2>& modifier) {
      return (stream + modifier.firstModifier()) + modifier.secondModifier();
    }

  }
}
#endif
