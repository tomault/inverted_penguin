#ifndef __INVERTED_PENGUIN__TESTING___TERMS__PREFIXPREPENDER_HPP__
#define __INVERTED_PENGUIN__TESTING___TERMS__PREFIXPREPENDER_HPP__

#include <inverted_penguin/terms/DynamicTermStreamModifierWrapper.hpp>
#include <inverted_penguin/terms/TermStreamModifier.hpp>
#include <string>

namespace inverted_penguin {
  namespace terms {

    class PrefixPrepender : public TermStreamModifier<PrefixPrepender> {
    public:
      static constexpr bool isStateful() { return false; }

    public:
      PrefixPrepender(const std::string& prefix): prefix_(prefix) { }

      template <typename S>
      Term next(S& stream) const { return this->apply(stream.next()); }

      Term apply(const Term& t) const {
	return t ? Term(prefix_ + t.text, t.position) : t;
      }

      bool reset() { return true; }
    
    private:
      std::string prefix_;
    };

    typedef DynamicTermStreamModifierWrapper<PrefixPrepender>
            DynamicPrefixPrepender;

  }
}
#endif

