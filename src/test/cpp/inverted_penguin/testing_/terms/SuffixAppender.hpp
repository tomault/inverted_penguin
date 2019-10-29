#ifndef __INVERTED_PENGUIN__TESTING___TERMS__SUFFIXAPPENDER_HPP__
#define __INVERTED_PENGUIN__TESTING___TERMS__SUFFIXAPPENDER_HPP__

#include <inverted_penguin/terms/DynamicTermStreamModifierWrapper.hpp>
#include <inverted_penguin/terms/TermStreamModifier.hpp>
#include <string>

namespace inverted_penguin {
  namespace terms {

    class SuffixAppender : public TermStreamModifier<SuffixAppender> {
    public:
      static constexpr bool isStateful() { return false; }

    public:
      SuffixAppender(const std::string& suffix): suffix_(suffix) { }

      template <typename S>
      Term next(S& stream) const { return this->apply(stream.next()); }

      Term apply(const Term& t) const {
	return t ? Term(t.text + suffix_, t.position) : t;
      }

      bool reset() { return true; }
    
    private:
      std::string suffix_;
    };

    typedef DynamicTermStreamModifierWrapper<SuffixAppender>
            DynamicSuffixAppender;

  }
}
#endif
