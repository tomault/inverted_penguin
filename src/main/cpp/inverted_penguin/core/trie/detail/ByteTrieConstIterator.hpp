#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIECONSTITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIECONSTITERATOR_HPP__

#include <inverted_penguin/core/detail/ByteTrieBaseIterator.hpp>
#include <vector>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename KeyAllocator>
      class ByteTrieConstIterator :
	  public ByteTrieBaseIterator<
	      Value, KeyAllocator, ByteTrieConstIterator<Value, KeyAllocator>
	  > {
      public:
	typedef ByteTrieBaseIterator<
	    Value, KeyAllocator, ByteTrieConstIterator<Value, KeyAllocator>
	> ParentType;
	using ParentType::Key;
	using ParentType::Location;

	struct KeyValue {
	  Key key;
	  const Value& value;

	  KeyValue(Key&& k, const Value& v): key(std::move(k)), value(v) { }
	};
	
	typedef KeyValue value_type;
	typedef KeyValue reference;

      protected:
	using ParentType::createKey_;
	using ParentType::getValue_;

      public:
	ByteTrieConstIterator(
	    const KeyAllocator& keyAllocator = KeyAllocator()
	): ByteTrieConstIterator(nullptr, keyAllocator) {
	}
	
	ByteTrieConstIterator(
	    const uint8_t* base,
	    const KeyAllocator& keyAllocator = KeyAllocator()
	): ParentType(base, keyAllocator) {
	}
	
	ByteTrieConstIterator(
	    const uint8_t* base, const std::vector<Location>& path,
	    const KeyAllocator& keyAllocator = KeyAllocator()
	): ParentType(base, path, keyAllocator) {
	}
	
	ByteTrieConstIterator(
	    const uint8_t* base,
	    std::vector<Location>&& path,
	    const KeyAllocator& keyAllocator = KeyAllocator()
	): ParentType(base, std::move(path), keyAllocator) {
	}
	
	ByteTrieConstIterator(const ByteTrieConstIterator&) = default;
	ByteTrieConstIterator(ByteTrieConstIterator&&) = default;
	ByteTrieConstIterator(const ParentType& other):
	    ParentType(other) {
	}
	ByteTrieConstIterator(ParentType&& other):
	    ParentType(std::move(other)) {
	}

	reference operator*() const {
	  return KeyValue(createKey_(), getValue_());
	}

	ByteTrieConstIterator& operator=(const ByteTrieConstIterator&) =
            default;
	ByteTrieConstIterator& operator=(ByteTrieConstIterator&&) = default;
	ByteTrieConstIterator& operator=(const ParentType& other) {
	  ParentType::operator=(other);
	  return *this;
	}
	ByteTrieConstIterator& operator=(ParentType&& other) {
	  ParentType::operator=(std::move(other));
	  return *this;
	}
      };
      
    }
  }
}
#endif

