#ifndef __INVERTED_PENGUIN__TOKENS__TOKENSTREAMMODIFIERSEQUENCE__HPP__
#define __INVERTED_PENGUIN__TOKENS__TOKENSTREAMMODIFIERSEQUENCE__HPP__

#include <inverted_penguin/tokens/Token.hpp>
#include <inverted_penguin/tokens/TokenStreamModifier.hpp>

namespace inverted_penguin {
  namespace tokens {
    template <typename S> class TokenStream;

    /** @brief Applies SecondModifier to the output of FirstModifier */
    template <typename FirstModifier, typename SecondModifier>
    class TokenStreamModifierSequence
        : public TokenStreamModifier<
                     TokenStreamModifierSequence<FirstModifier,
						 SecondModifier>
                 >,
	  private FirstModifier,
	  private SecondModifier {
    public:
      typedef TokenStreamModifierSequence<FirstModifier, SecondModifier>
              DerivedModifierType;
      typedef FirstModifier FirstModifierType;
      typedef SecondModifier SecondModifierType;

    public:
      TokenStreamModifierSequence(const FirstModifier& first,
				  const SecondModifier& second):
  	  FirstModifier(first), SecondModifier(second) {
      }

      const FirstModifier& firstModifier() const {
	return static_cast<const FirstModifier&>(*this);
      }
      const SecondModifier& secondModifier() const {
	return static_cast<const SecondModifier&>(*this);
      }
      const TokenStreamModifierSequence& self() const { *this; }
      TokenStreamModifierSequence& self() { return *this; }

      template <typename S>
      Token next(TokenStream<S>& stream) const {
	Token t = stream.self().next();
	while (t.notEmpty() && ((t = apply(t)).isEmpty())) {
	  t = stream.self().next();
	}
	return t;
      }

      Token apply(const Token& t) const {
	return SecondModifier::apply(FirstModifier::apply(t));
      }
    };
    
  }
}

#endif

