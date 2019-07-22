#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYPERSISTENTKEY_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYPERSISTENTKEY_HPP__

#include <inverted_penguin/core/dictionary/detail/ValueDictionaryKey.hpp>
#include <inverted_penguin/core/dictionary/detail/ValueDictionaryKeyUtils.hpp>
#include <memory>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {

	template <typename Allocator>
	class ValueDictionaryPersistentKey : Allocator {
	public:
	  ValueDictionaryPersistentKey(
	      const Allocator& allocator = Allocator()
	  ): Allocator(allocator), begin_(nullptr), end_(nullptr) {
	    // Intentionally left blank
	  }
	  
	  ValueDictionaryPersistentKey(
	      const uint8_t* begin, const uint8_t* end,
	      const Allocator& allocator = Allocator()
	  ):
	      Allocator(allocator), begin_(this->allocate(end - begin)),
	      end_(begin_ + (end - begin)) {
	    ::memcpy(begin_, begin, end - begin);
	  }

	  ValueDictionaryPersistentKey(
	      const ValueDictionaryKey& other,
	      const Allocator& allocator = Allocator()
	  ):
	      ValueDictionaryPersistentKey(other.begin(), other.end(),
					   allocator) {
	    // Intentionally left blank
	  }

	  // ValueDictionaryPersistentKey(
	  //     const ValueDictionaryPersistentKey& other
	  // ): ValueDictionaryPersistentKey(other.begin(), other.end(),
	  // 				  other.allocator()) {
	  // }

	  ValueDictionaryPersistentKey(const ValueDictionaryPersistentKey&) =
	      delete;

	  ValueDictionaryPersistentKey(ValueDictionaryPersistentKey&& other):
	      Allocator(std::move(other)), begin_(other.begin_),
	      end_(other.end_) {
	    other.begin_ = nullptr;
	    other.end_ = nullptr;
	  }

	  ~ValueDictionaryPersistentKey() { release_(); }

	  const Allocator& allocator() const { return *this; }
	  Allocator& allocator() { return *this; }
	  size_t size() const { return end_ - begin_; }
	  const uint8_t* begin() const { return begin_; }
	  const uint8_t* end() const { return end_; }

	  size_t hash() const { return key_utils::hash(begin(), end()); }
	  
	  int cmp(const ValueDictionaryPersistentKey& other) const {
	    return key_utils::compare(begin(), end(),
				      other.begin(), other.end());
	  }

	  ValueDictionaryKey wrap() const {
	    return ValueDictionaryKey(begin(), end());
	  }
	  
	  // ValueDictionaryPersistentKey& operator=(
	  //     const ValueDictionaryPersistentKey& other
	  // ) {
	  //   if (this != &other) {
	  //     release_();

	  //     Allocator::operator=(other);
	  //     begin_ = other.size() ? this->allocate(other.size()) : nullptr;
	  //     end_ = begin_ + other.size();
	  //     ::memcpy(begin_, other.begin(), other.size());
	  //   }
	  //   return *this;
	  // }

	  
	  ValueDictionaryPersistentKey& operator=(
	      const ValueDictionaryPersistentKey& other
	  ) = delete;

	  ValueDictionaryPersistentKey& operator=(
	      ValueDictionaryPersistentKey&& other
	  ) {
	    if (this != &other) {
	      release_();

	      Allocator::operator=(std::move(other));
	      begin_ = other.begin_;
	      end_ = other.end_;
	      other.begin_ = nullptr;
	      other.end_ = nullptr;
	    }
	    return *this;
	  }

	  bool operator==(const ValueDictionaryPersistentKey& other) const {
	    return key_utils::areEqual(begin(), end(),
				       other.begin(), other.end());
	  }

	  bool operator!=(const ValueDictionaryPersistentKey& other) const {
	    return key_utils::notEqual(begin(), end(),
				       other.begin(), other.end());
	  }

	  bool operator<(const ValueDictionaryPersistentKey& other) const {
	    return cmp(other) < 0;
	  }

	  bool operator>(const ValueDictionaryPersistentKey& other) const {
	    return cmp(other) > 0;
	  }

	  bool operator<=(const ValueDictionaryPersistentKey& other) const {
	    return cmp(other) <= 0;
	  }

	  bool operator>=(const ValueDictionaryPersistentKey& other) const {
	    return cmp(other) >= 0;
	  }

	  uint8_t operator[](size_t i) const { return begin_[i]; }
	  
	private:
	  uint8_t* begin_;
	  uint8_t* end_;

	  void release_() {
	    if (begin_) {
	      this->deallocate(begin_, end_ - begin_);
	    }
	  }	  
	};

	template <typename Allocator>
	inline std::ostream& operator<<(
	    std::ostream& out,
	    const ValueDictionaryPersistentKey<Allocator>& key
	) {
	  out.write((const char*)key.begin(), key.size());
	  return out;
	}
	
      }
    }
  }
}

namespace std {
  template <typename Allocator>
  struct hash<
      inverted_penguin::core::dictionary::detail::ValueDictionaryPersistentKey<
	  Allocator
      >
  > {
    typedef inverted_penguin::core::dictionary::detail::ValueDictionaryPersistentKey<Allocator>
            argument_type;
    typedef size_t result_type;

    result_type operator()(argument_type const& x) const {
      return x.hash();
    }
  };
}

#endif
