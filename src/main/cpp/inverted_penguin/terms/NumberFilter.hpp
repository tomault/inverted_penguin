#ifndef __INVERTED_PENGUIN__TERMS__NUMBERFILTER_HPP__
#define __INVERTED_PENGUIN__TERMS__NUMBERFILTER_HPP__

#include <inverted_penguin/terms/DynamicTermStreamModifierWrapper.hpp>
#include <inverted_penguin/terms/TermStreamModifier.hpp>
#include <inverted_penguin/terms/UnicodeUtils.hpp>

namespace inverted_penguin {
  namespace terms {

    class NumberFilter {
    public:
      static constexpr bool isStateful() { return false; }
      
      template <typename S>
      Term next(TermStream<S>& stream) const {
	Term t = stream.self().next();
	while (t.notEmpty() && isAllDigits_(t.text)) {
	  t = stream.self().next();
	}
	return t;
      }

      Term apply(Term t) const {
	return isAllDigits_(t.text) ? Term::empty() : t;
      }

      bool reset() { return true; }

    private:
      static bool isAllDigits_(const std::string& s) {
	auto p = s.begin();
	uint32_t c;
	
	while (p != s.end()) {
	  std::tie(c, p) = decodeUtf8(p, s.end());
	  if ((c < '0') || (c > '9')) {
	    return false;
	  }
	}
	return true;
      }
    };

    typedef DynamicTermStreamModifierWrapper<NumberFilter> DynamicNumberFilter;
  }
}
#endif

