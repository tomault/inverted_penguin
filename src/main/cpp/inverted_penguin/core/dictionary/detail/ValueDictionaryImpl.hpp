#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYIMPL_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYIMPL_HPP__

#include <inverted_penguin/core/dictionary/detail/ValueDictionaryKey.hpp>
#include <inverted_penguin/core/dictionary/detail/ValueDictionaryIterators.hpp>
#include <pistis/memory/AlignmentUtils.hpp>
#include <iostream>
#include <tuple>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {

	template <typename Value>
	class ValueDictionaryImpl {
	public:
	  typedef ValueDictionaryKey Key;
	  typedef ValueDictionaryConstIterator<Value> Iterator;
	  typedef ValueDictionaryKeyIterator<Value> KeyIterator;
	  typedef ValueDictionaryValueIterator<Value> ValueIterator;
	  
	public:
	  ValueDictionaryImpl(const uint8_t* base):
	      base_(base), keyEnds_((const uint32_t*)(base_ + 4)),
	      values_(pistis::memory::alignTo(
			  keyEnds_ + *(const uint32_t*)base, alignof(Value)
		      )),
	      keys_((const uint8_t*)(values_ + *(const uint32_t*)base)) {
	  }
	  ValueDictionaryImpl(const ValueDictionaryImpl&) = delete;
	  ValueDictionaryImpl(ValueDictionaryImpl&& other):
	      base_(other.base_), keyEnds_(other.keyEnds_),
	      values_(other.values_), keys_(other.keys_) {
	    other.base_ = nullptr;
	    other.keyEnds_ = nullptr;
	    other.values_ = nullptr;
	    other.keys_ = nullptr;
	  }

	  const uint8_t* base() const { return base_; }
	  uint32_t size() const { return *(const uint32_t*)base_; }
	  Iterator begin() const { return Iterator(this, 0); }
	  Iterator end() const { return Iterator(this, size()); }
	  Iterator position(uint32_t index) const {
	    return Iterator(this, index);
	  }
	  Key keyAt(uint32_t i) const { return Key(keyStart_(i), keyEnd_(i)); }
	  const Value& valueAt(uint32_t i) const { return values_[i]; }

	  std::tuple<Iterator, Iterator> prefix(const uint8_t* key,
						const uint8_t* keyEnd) const {
	    uint32_t start, end;
	    std::tie(start, end) = findPrefix_(key, keyEnd);
	    return std::make_tuple(position(start), position(end));
	  }

	  ValueDictionaryImpl& operator=(const ValueDictionaryImpl&) = delete;
	  ValueDictionaryImpl& operator=(ValueDictionaryImpl&& other) {
	    if (this != &other) {
	      base_ = other.base_;
	      keyEnds_ = other.keyEnds_;
	      values_ = other.values_;
	      keys_ = other.keys_;

	      other.base_ = nullptr;
	      other.keyEnds_ = nullptr;
	      other.values_ = nullptr;
	      other.keys_ = nullptr;
	    }
	    return *this;
	  }
	  
	protected:
	  const uint8_t* base_;
	  const uint32_t* keyEnds_;
	  const Value* values_;
	  const uint8_t* keys_;

	  const uint8_t* keyStart_(uint32_t index) const {
	    return (index > 0) ? keys_ + keyEnds_[index - 1] + 1 : keys_;
	  }

	  const uint8_t* keyEnd_(uint32_t index) const {
	    return keys_ + keyEnds_[index] + 1;
	  }

	  size_t keyLength_(uint32_t index) const {
	    return (index > 0) ? keyEnds_[index] - keyEnds_[index - 1]
	                       : keyEnds_[0] + 1;
	  }

	  std::tuple<uint32_t, uint32_t> findPrefix_(
	      const uint8_t* key, const uint8_t* keyEnd
	  ) const {
	    uint32_t i = 0;
	    uint32_t j = size();
	    const uint8_t* k = key;

	    while ((k < keyEnd) && ((j - i) > 1)) {
	      i = lowerBound_(i, j, *k, k - key);
	      j = upperBound_(i, j, *k, k - key);
	      ++k;
	    }

	    
	    // If there is one candidate left, and that candidate is shorter
	    // than the target prefix or its first keyEnd - k bytes are not
	    // equal to the prefix starting at k, then there is no match
	    if (((j - i) == 1) && (k < keyEnd) &&
		((keyLength_(i) < (keyEnd - key)) ||
		 ::memcmp(k, keyStart_(i) + (k - key), keyEnd - k))) {
	      j = i;
	    }

	    return std::make_tuple(i, j);
	  }

	  uint32_t lowerBound_(uint32_t start, uint32_t end, uint8_t key,
			       uint32_t d) const {
	    uint64_t i = start;
	    uint64_t j = end;

	    while (i < j) {
	      // i and j 64 bit integers in range 0 to (2 ^ 32 - 1), so ok.
	      const uint32_t k = (uint32_t)((i + j) >> 1);

	      if ((keyLength_(k) <= d) || (keyStart_(k)[d] < key)) {
		i = k + 1;
	      } else {
		j = k;
	      }
	    }
	    return i;
	  }

	  uint32_t upperBound_(uint32_t start, uint32_t end, uint8_t key,
			       uint32_t d) const {
	    uint64_t i = start;
	    uint64_t j = end;

	    while (i < j) {
	      // i and j 64 bit integers in range 0 to (2 ^ 32 - 1), so ok.
	      const uint32_t k = (uint32_t)((i + j) >> 1);
	      if ((keyLength_(k) <= d) || (keyStart_(k)[d] <= key)) {
		i = k + 1;
	      } else {
		j = k;
	      }
	    }
	    return i;
	  }
	};
	
      }
    }
  }
}
#endif

