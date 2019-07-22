#ifndef __INVERTED_PENGUIN__TERMS__TERMSTREAM__HPP__
#define __INVERTED_PENGUIN__TERMS__TERMSTREAM__HPP__

namespace inverted_penguin {
  namespace terms {

    template <typename S>
    class TermStream {
    public:
      const S& self() const { return static_cast<const S&>(*this); }
      S& self() { return static_cast<S&>(*this); }
    };
    
  }
}
#endif

