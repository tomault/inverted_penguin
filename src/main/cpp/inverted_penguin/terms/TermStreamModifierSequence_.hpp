#ifndef __INVERTED_PENGUIN__TERMS__TERMSTREAMMODIFIERSEQUENCE__HPP__
#define __INVERTED_PENGUIN__TERMS__TERMSTREAMMODIFIERSEQUENCE__HPP__

#include <inverted_penguin/terms/Term.hpp>
#include <inverted_penguin/terms/TermStreamModifier.hpp>

namespace inverted_penguin {
  namespace terms {
    
    template <typename S> class TermStream;

    /** @brief Applies SecondModifier to the output of FirstModifier */
    template <typename FirstModifier, typename SecondModifier>
    class TermStreamModifierSequence
        : public TermStreamModifier<
                     TermStreamModifierSequence<FirstModifier,
						SecondModifier>
                 >,
	  private FirstModifier,
	  private SecondModifier {
    public:
      typedef TermStreamModifierSequence<FirstModifier, SecondModifier>
              DerivedModifierType;
      typedef FirstModifier FirstModifierType;
      typedef SecondModifier SecondModifierType;

    public:
      TermStreamModifierSequence(const FirstModifier& first,
				 const SecondModifier& second):
  	  FirstModifier(first), SecondModifier(second) {
      }

      const FirstModifier& firstModifier() const {
	return static_cast<const FirstModifier&>(*this);
      }
      
      const SecondModifier& secondModifier() const {
	return static_cast<const SecondModifier&>(*this);
      }
      
      const TermStreamModifierSequence& self() const { *this; }
      TermStreamModifierSequence& self() { return *this; }

      static constexpr bool isStateful() {
	return FirstModifier::isStateful() || SecondModifier::isStateful();
      }

      template <typename S>
      Term next(TermStream<S>& stream) const {
	Term t = stream.self().next();
	while (t.notEmpty() && ((t = apply(t)).isEmpty())) {
	  t = stream.self().next();
	}
	return t;
      }

      Term apply(const Term& t) const {
	return SecondModifier::apply(FirstModifier::apply(t));
      }
    };
    
  }
}

#endif
