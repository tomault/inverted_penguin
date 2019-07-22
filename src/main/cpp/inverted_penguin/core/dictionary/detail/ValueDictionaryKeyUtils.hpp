#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYKEYUTILS_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYKEYUTILS_HPP__

#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {
	namespace key_utils {

	  inline size_t hash(const uint8_t* begin, const uint8_t* end) {
	    size_t code = 5381;
	    for (auto p = begin; p != end; ++p) {
	      code = ((code << 5) + code) + *p;
	    }
	    return code;
	  }

	  inline bool areEqual(const uint8_t* left, const uint8_t* leftEnd,
			       const uint8_t* right, const uint8_t* rightEnd) {
	    const ptrdiff_t leftSize = leftEnd - left;
	    const ptrdiff_t rightSize = rightEnd - right;

	    return (leftSize == rightSize) &&
	           ((left == right) || !::memcmp(left, right, leftSize));
	  }

	  inline bool notEqual(const uint8_t* left, const uint8_t* leftEnd,
			       const uint8_t* right, const uint8_t* rightEnd) {
	    const ptrdiff_t leftSize = leftEnd - left;
	    const ptrdiff_t rightSize = rightEnd - right;

	    return (leftSize != rightSize) ||
	           ((left != right) && ::memcmp(left, right, leftSize));
	  }
	  
	  inline ptrdiff_t compare(const uint8_t* left, const uint8_t* leftEnd,
				   const uint8_t* right,
				   const uint8_t* rightEnd) {
	    if (left == right) {
	      return leftEnd - rightEnd;
	    } else {
	      const ptrdiff_t leftSize = leftEnd - left;
	      const ptrdiff_t o = leftSize - (rightEnd - right);
	      return o ? o : ::memcmp(left, right, leftSize);
	    }
	  }
	  
	}
      }
    }
  }
}
#endif
