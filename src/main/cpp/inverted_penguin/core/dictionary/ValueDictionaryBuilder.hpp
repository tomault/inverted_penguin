#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__VALUEDICTIONARYBUILDER_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__VALUEDICTIONARYBUILDER_HPP__

#include <inverted_penguin/core/dictionary/ValueDictionary.hpp>
#include <inverted_penguin/core/dictionary/detail/ValueDictionaryBuilderIterators.hpp>
#include <inverted_penguin/core/dictionary/detail/ValueDictionaryKey.hpp>
#include <inverted_penguin/core/dictionary/detail/ValueDictionaryPersistentKey.hpp>
#include <inverted_penguin/core/dictionary/detail/ValueDictionaryWriter.hpp>
#include <inverted_penguin/exceptions/DictionaryTooLargeError.hpp>

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <assert.h>
#include <stdint.h>
#include <string.h>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {

      template <typename Value,
		typename KeyAllocator = std::allocator<uint8_t>,
		typename DictionaryAllocator = std::allocator<uint8_t> >
      class ValueDictionaryBuilder : DictionaryAllocator {
      public:
	typedef Value ValueType;
	typedef KeyAllocator KeyAllocatorType;
	typedef DictionaryAllocator DictionaryAllocatorType;
	
	typedef detail::ValueDictionaryBuilderConstIterator<
	    Value,
	    ValueDictionaryBuilder<Value, KeyAllocator, DictionaryAllocator>
	> ConstIterator;

	typedef detail::ValueDictionaryBuilderIterator<
	    Value,
	    ValueDictionaryBuilder<Value, KeyAllocator, DictionaryAllocator>
	> Iterator;
	
	typedef detail::ValueDictionaryBuilderKeyIterator<
	    ValueDictionaryBuilder<Value, KeyAllocator, DictionaryAllocator>
	> KeyIterator;
	
	typedef detail::ValueDictionaryBuilderConstValueIterator<
	    Value,
	    ValueDictionaryBuilder<Value, KeyAllocator, DictionaryAllocator>
	> ConstValueIterator;

	typedef detail::ValueDictionaryBuilderValueIterator<
	    Value,
	    ValueDictionaryBuilder<Value, KeyAllocator, DictionaryAllocator>
	> ValueIterator;

	typedef detail::ValueDictionaryKey Key;

	static constexpr const size_t MAX_DICT_SIZE =
	    std::min((size_t)std::numeric_limits<uint32_t>::max(),
		     (size_t)std::numeric_limits<ptrdiff_t>::max());
	static constexpr const size_t MAX_KEY_SIZE = MAX_DICT_SIZE;
	static constexpr const size_t MAX_DICT_SIZE_IN_BYTES =
	    (size_t)std::numeric_limits<uint32_t>::max() + 1;

      protected:
	typedef detail::ValueDictionaryPersistentKey<KeyAllocator>
	        PersistentKey;
	
      public:
	ValueDictionaryBuilder(const DictionaryAllocator& allocator =
			           DictionaryAllocator()):
	   ValueDictionaryBuilder(allocator, allocator) {
	}
	
	ValueDictionaryBuilder(const DictionaryAllocator& dictionaryAllocator,
			       const KeyAllocator& keyAllocator):
	    DictionaryAllocator(dictionaryAllocator),
	    keyAllocator_(keyAllocator), keys_(), items_() {
	}
	
	ValueDictionaryBuilder(const ValueDictionaryBuilder&) = delete;
	ValueDictionaryBuilder(ValueDictionaryBuilder&&) = default;
	
	const DictionaryAllocator& dictionaryAllocator() const {
	  return static_cast<const DictionaryAllocator&>(*this);
	}

	DictionaryAllocator& dictionaryAllocator() {
	  return static_cast<DictionaryAllocator&>(*this);
	}
	
	const KeyAllocator& keyAllocator() const { return keyAllocator_; }
	KeyAllocator& keyAllocator() { return keyAllocator_; }
	
	size_t size() const { return keys_.size(); }
	ConstIterator begin() const {
	  return ConstIterator(const_cast<ValueDictionaryBuilder*>(this), 0);
	}
	Iterator begin() { return Iterator(this, 0); }
	ConstIterator end() const {
	  return ConstIterator(const_cast<ValueDictionaryBuilder*>(this),
			       size());
	}
	Iterator end() { return Iterator(this, size()); }
	ConstIterator cbegin() const { return begin(); }
	ConstIterator cend() const { return end(); }

	Key keyAt(size_t i) const { return keys_[i].wrap(); }

	const Value& valueAt(size_t i) const {
	  auto p = items_.find(keyAt(i));
	  assert(p != items_.end());
	  return p->second;
	}

	Value& valueAt(size_t i) {
	  auto p = items_.find(keyAt(i));
	  assert(p != items_.end());
	  return p->second;
	}

	ConstIterator find(const uint8_t* key, const uint8_t* keyEnd) const {
	  assert((keyEnd - key) <= MAX_KEY_SIZE);
	  ptrdiff_t i = findKey_(key, keyEnd);
	  return ConstIterator(const_cast<ValueDictionaryBuilder*>(this),
			       (i >= 0) ? i : size());
	}

	Iterator find(const uint8_t* key, const uint8_t* keyEnd) {
	  assert((keyEnd - key) <= MAX_KEY_SIZE);
	  ptrdiff_t i = findKey_(key, keyEnd);
	  return Iterator(this, (i >= 0) ? i : size());
	}

	std::tuple<ConstIterator, ConstIterator> prefix(
	    const uint8_t* key,	const uint8_t* keyEnd
	) const {
	  assert((keyEnd - key) <= MAX_KEY_SIZE);
	  ptrdiff_t i, j;
	  std::tie(i, j) = findPrefix_(key, keyEnd);
	  if (i == j) {
	    return std::make_tuple(end(), end());
	  } else {
	    assert(i >= 0);
	    assert(i < j);
	    return std::make_tuple(position_(i), position_(j));
	  }
	}

	bool has(const uint8_t* key, const uint8_t* keyEnd) const {
	  assert((keyEnd - key) <= MAX_KEY_SIZE);
	  return items_.find(Key(key, keyEnd)) != items_.end();
	}
	
	Optional<Value> get(const uint8_t* key, const uint8_t* keyEnd) const {
	  assert((keyEnd - key) <= MAX_KEY_SIZE);
	  auto p = items_.find(Key(key, keyEnd));
	  return (p != items_.end()) ?
	      Optional<Value>(p->second) :
	      Optional<Value>();
	}

	void add(const uint8_t* key, const uint8_t* keyEnd,
		 const Value& value) {
	  assert((keyEnd - key) <= MAX_KEY_SIZE);
	  auto p = items_.find(Key(key, keyEnd));

	  if (p != items_.end()) {
	    p->second = value;
	  } else {
	    insert_(key, keyEnd, [&value](){ return value; });
	  }
	}

	template <typename Function>
	const Value& getOrAdd(const uint8_t* key, const uint8_t* keyEnd,
			      Function makeValue) {
	  assert((keyEnd - key) <= MAX_KEY_SIZE);
	  auto p = items_.find(Key(key, keyEnd));

	  if (p == items_.end()) {
	    p = insert_(key, keyEnd, makeValue);
	  }
	  return p->second;
	}

	size_t serializedSizeInBytes() const {
	  detail::ValueDictionaryWriter<Value> writer;
	  return writer.dataSize(size(), begin());
	}
	
	template <typename Allocator>
	InMemoryValueDictionary<Value, Allocator> build(
	    Allocator& allocator
	) {
	  detail::ValueDictionaryWriter<Value> writer;
	  const size_t allocationSize = writer.dataSize(size(), begin());
	  if (allocationSize > MAX_DICT_SIZE_IN_BYTES) {
	    throw exceptions::DictionaryTooLargeError(PISTIS_EX_HERE);
	  }
	  
	  uint8_t* const data = allocator.allocate(allocationSize);
	  uint8_t* p = data;
	  
	  size_t numWritten = writer.write(size(), begin(),
					   [&p](const void* data, size_t n) {
	    ::memcpy(p, data, n);
	    p += n;
	    return n;
	  });

	  assert(numWritten == allocationSize);
	  return InMemoryValueDictionary<Value, Allocator>(data,
							   allocationSize,
							   allocator);
	}

	auto build() { return build(this->dictionaryAllocator()); }

	template <typename Writer>
	size_t write(Writer writeData) const {
	  detail::ValueDictionaryWriter<Value> writer;
	  return writer.write(size(), begin(), writeData);
	}

	ValueDictionaryBuilder& operator=(const ValueDictionaryBuilder&) =
	    delete;
	
	ValueDictionaryBuilder& operator=(ValueDictionaryBuilder&&) = default;
	
      protected:
	KeyAllocator keyAllocator_;
	std::vector<PersistentKey> keys_;
	std::unordered_map<Key, Value> items_;

	ConstIterator position_(ptrdiff_t i) const {
	  return ConstIterator(const_cast<ValueDictionaryBuilder*>(this),
			       i >= 0 ? i : size());
	}

	template <typename Function>
	typename std::unordered_map<Key, Value>::iterator insert_(
	    const uint8_t* key, const uint8_t* keyEnd, Function makeValue
	) {
	  assert(size() < MAX_DICT_SIZE);

	  PersistentKey newKey(key, keyEnd, keyAllocator());
	  const ptrdiff_t i = findKey_(key, keyEnd);
	  assert(i < 0);

	  auto p = items_.emplace(Key(newKey.begin(), newKey.end()),
				  makeValue());
	  keys_.insert(keys_.begin() - i - 1, std::move(newKey));
	  return p.first;
	}

	ptrdiff_t findKey_(const uint8_t* key, const uint8_t* keyEnd) const {
	  ptrdiff_t i, j;
	  std::tie(i, j) = findPrefix_(key, keyEnd);

	  assert(i <= j);
	  assert(((i == j) && (i < 0)) || ((i < j) && (i >= 0)));
	  
	  if ((i < j) && ((keyEnd - key) < keys_[i].size())) {
	    i = -i - 1;
	  }
	  return i;
	}

	std::tuple<ptrdiff_t, ptrdiff_t> findPrefix_(
	    const uint8_t* key, const uint8_t* keyEnd
	) const {
	  if (key == keyEnd) {
	    return std::make_tuple((ptrdiff_t)-1, (ptrdiff_t)-1);
	  } else {
	    const ptrdiff_t keyLength = keyEnd - key;
	    ptrdiff_t depth = 0;
	    ptrdiff_t i = 0;
	    ptrdiff_t j = size();

	    while (depth < keyLength) {
	      i = lowerBound_(i, j, key[depth], depth);
	      j = upperBound_(i, j, key[depth], depth);
	      assert(i <= j);
	      
	      if (i == j) {
		i = -i - 1;
		return std::make_tuple(i, i);
	      }
	      ++depth;
	    }

	    return std::make_tuple(i, j);
	  }
	}

	ptrdiff_t lowerBound_(const ptrdiff_t start, const ptrdiff_t end,
			      const uint8_t key, const ptrdiff_t depth) const {
	  ptrdiff_t i = start;
	  ptrdiff_t j = end;

	  while (i < j) {
	    ptrdiff_t k = ((size_t)i + (size_t)j) >> 1;
	    const PersistentKey& candidate = keys_[k];
	    
	    if ((depth >= candidate.size()) || (candidate[depth] < key)) {
	      i = k + 1;
	    } else {
	      j = k;
	    }
	  }
	  
	  return i;
	}

	ptrdiff_t upperBound_(const ptrdiff_t start, const ptrdiff_t end,
			      const uint8_t key, const ptrdiff_t depth) const {
	  ptrdiff_t i = start;
	  ptrdiff_t j = end;

	  while (i < j) {
	    ptrdiff_t k = ((size_t)i + (size_t)j) >> 1;
	    const PersistentKey& candidate = keys_[k];
	    
	    if ((depth >= candidate.size()) || (candidate[depth] <= key)) {
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
#endif
