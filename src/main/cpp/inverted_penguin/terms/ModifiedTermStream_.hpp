#ifndef __INVERTED_PENGUIN__TERMS__MODIFIEDTERMSTREAM__HPP__
#define __INVERTED_PENGUIN__TERMS__MODIFIEDTERMSTREAM__HPP__

#include <inverted_penguin/terms/Term.hpp>

namespace inverted_penguin {
  namespace terms {

    template <typename S, typename M>
    class ModifiedTermStream
        : public TermStream< ModifiedTermStream<S, M> > {
    public:
      ModifiedTermStream(const S& base, const M& modifier):
  	  base_(base), modifier_(modifier) {
      }

      const S& base() const { return base_; }
      const M& modifier() const { return modifier_; }

      Term next() { return modifier_.next(base_); }
      bool reset() { return base_.reset(); }

    private:
      S base_;
      M modifier_;
    };

  }
}

#endif
