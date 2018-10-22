#ifndef __INVERTED_PENGUIN__TOKENS__TOKENSTREAMMODIFIER__HPP__
#define __INVERTED_PENGUIN__TOKENS__TOKENSTREAMMODIFIER__HPP__

namespace inverted_penguin {
  namespace tokens {

    template <typename DerivedModifier>
    class TokenStreamModifier {
    public:
      typedef DerivedModifier DerivedModifierType;

      const DerivedModifier& self() const {
	return static_cast<const DerivedModifier&>(*this);
      }
      DerivedModifier& self() { return static_cast<DerivedModifier&>(*this); }
    };
    
  }
}

#endif

