#ifndef __INVERTED_PENGUIN__TOKENS__MODIFIEDTOKENSTREAM__HPP__
#define __INVERTED_PENGUIN__TOKENS__MODIFIEDTOKENSTREAM__HPP__

#include <inverted_penguin/tokens/Token.hpp>

namespace inverted_penguin {
  namespace tokens {

    template <typename S, typename M>
    class ModifiedTokenStream
        : public TokenStream< ModifiedTokenStream<S, M> > {
    public:
      ModifiedTokenStream(const S& base, const M& modifier):
  	  base_(base), modifier_(modifier) {
      }

      const S& base() const { return base_; }
      const M& modifier() const { return modifier_; }

      Token next() { return modifier_.next(base_); }
      bool reset() { return base_.reset(); }

    private:
      S base_;
      M modifier_;
    };

  }
}

#endif
