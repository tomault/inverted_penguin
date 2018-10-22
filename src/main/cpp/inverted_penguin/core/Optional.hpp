#ifndef __INVERTED_PENGUIN__CORE__OPTIONAL_HPP__
#define __INVERTED_PENGUIN__CORE__OPTIONAL_HPP__

#include <pistis/typeutil/Optional.hpp>
namespace inverted_penguin {
  namespace core {

    template <typename T>
    using Optional = pistis::typeutil::Optional<T>;

  }
}

#endif
