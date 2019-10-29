#ifndef __INVERTED_PENGUIN__TERMS__KSTEMMAPPER_HPP__
#define __INVERTED_PENGUIN__TERMS__KSTEMMAPPER_HPP__

#include <inverted_penguin/terms/DynamicTermStream.hpp>
#include <inverted_penguin/terms/DynamicTermStreamModifierWrapper.hpp>
#include <inverted_penguin/terms/TermStream.hpp>
#include <inverted_penguin/terms/TermStreamModifier.hpp>
#include <inverted_penguin/terms/detail/KrovetzStemmer.hpp>

#include <iostream>

namespace inverted_penguin {
  namespace terms {

    class KStemMapper : public TermStreamModifier<KStemMapper> {
    public:
      static constexpr bool isStateful() { return false; }
      
      template <typename S>
      Term next(TermStream<S>& stream) const {
	return this->apply(stream.self().next());
      }

      Term apply(const Term& t) const {
	if ((t.text.size() < 2) ||
	    (t.text.size() >= (detail::KrovetzStemmer::MAX_WORD_LENGTH - 1))) {
	  return t;
	} else {
	  char buffer[detail::KrovetzStemmer::MAX_WORD_LENGTH * 2];
	  int n = kStemmer_.kstem_stem_tobuffer(t.text.c_str(), buffer, true);
	  return n ? Term(std::string(buffer, n - 1), t.position) : t;
	}
      }

      bool reset() { return true; }
      
    private:
      detail::KrovetzStemmer kStemmer_;
    };

    typedef DynamicTermStreamModifierWrapper<KStemMapper> DynamicKStemMapper;
  }
}
#endif
