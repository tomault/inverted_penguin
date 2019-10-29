#ifndef __INVERTED_PENGUIN__TERMS__LOWERCASEMAPPER_HPP__
#define __INVERTED_PENGUIN__TERMS__LOWERCASEMAPPER_HPP__

#include <inverted_penguin/terms/DynamicTermStreamModifierWrapper.hpp>
#include <inverted_penguin/terms/TermStreamModifier.hpp>
#include <inverted_penguin/terms/UnicodeUtils.hpp>
#include <iterator>
#include <vector>

namespace inverted_penguin {
  namespace terms {

    class LowerCaseMapper : public TermStreamModifier<LowerCaseMapper> {
    public:
      static constexpr bool isStateful() { return false; }
      
      template <typename S>
      Term next(TermStream<S>& stream) const {
	return this->apply(stream.self().next());
      }

      Term apply(const Term& t) const {
	if (t.isEmpty()) {
	  return t;
	} else {
	  std::vector<char> lowered;

	  lowered.reserve(t.text.size());
	  transformUtf8Sequence(t.text.begin(), t.text.end(),
				std::back_inserter(lowered),
				[](uint32_t c) { return toLowerCase(c); });
	  return Term(std::string(lowered.begin(), lowered.end()),
		      t.position);
	}
      }

      bool reset() { return true; }
    };

    typedef DynamicTermStreamModifierWrapper<LowerCaseMapper>
            DynamicLowerCaseMapper;
  }
}

#endif

