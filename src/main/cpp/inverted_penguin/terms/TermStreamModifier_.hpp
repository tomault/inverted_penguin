#ifndef __INVERTED_PENGUIN__TERMS__TERMSTREAMMODIFIER__HPP__
#define __INVERTED_PENGUIN__TERMS__TERMSTREAMMODIFIER__HPP__

namespace inverted_penguin {
  namespace terms {

    template <typename DerivedModifier>
    class TermStreamModifier {
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

