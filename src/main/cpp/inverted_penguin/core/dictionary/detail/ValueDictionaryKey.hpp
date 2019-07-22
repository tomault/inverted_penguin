#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYKEY_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYKEY_HPP__

#include <inverted_penguin/core/dictionary/detail/ValueDictionaryKeyUtils.hpp>
#include <functional>
#include <ostream>
#include <string.h>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {

	class ValueDictionaryKey {
	public:
	  ValueDictionaryKey(): begin_(nullptr), end_(nullptr) { }
	  ValueDictionaryKey(const uint8_t* begin, const uint8_t* end):
	      begin_(begin), end_(end) {
	  }

	  size_t size() const noexcept { return end_ - begin_; }
	  const uint8_t* begin() const noexcept { return begin_; }
	  const uint8_t* end() const noexcept { return end_; }

	  size_t hash() const noexcept {
	    return key_utils::hash(begin(), end());
	  }

	  ptrdiff_t cmp(const ValueDictionaryKey& other) const {
	    return key_utils::compare(begin(), end(),
				      other.begin(), other.end());
	  }

	  bool operator==(const ValueDictionaryKey& other) const {
	    return key_utils::areEqual(begin(), end(),
				       other.begin(), other.end());
	  }

	  bool operator!=(const ValueDictionaryKey& other) const {
	    return key_utils::notEqual(begin(), end(),
				       other.begin(), other.end());
	  }

	  bool operator<(const ValueDictionaryKey& other) const {
	    return cmp(other) < 0;
	  }

	  bool operator>(const ValueDictionaryKey& other) const {
	    return cmp(other) > 0;
	  }

	  bool operator<=(const ValueDictionaryKey& other) const {
	    return cmp(other) <= 0;
	  }

	  bool operator>=(const ValueDictionaryKey& other) const {
	    return cmp(other) >= 0;
	  }

	  uint8_t operator[](size_t i) const { return begin_[i]; }
	private:
	  const uint8_t* begin_;
	  const uint8_t* end_;
	};

	inline std::ostream& operator<<(std::ostream& out,
					const ValueDictionaryKey& key) {
	  out.write((const char*)key.begin(), key.size());
	  return out;
	}
	
      }
    }
  }
}

namespace std {
  template <>
  struct hash<inverted_penguin::core::dictionary::detail::ValueDictionaryKey> {
    typedef inverted_penguin::core::dictionary::detail::ValueDictionaryKey
            argument_type;
    typedef size_t result_type;

    result_type operator()(argument_type const& x) const {
      return x.hash();
    }
  };
}
#endif

