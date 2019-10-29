#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEVALUEITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEVALUEITERATOR_HPP__

#include <inverted_penguin/core/detail/ByteTrieBaseIterator.hpp>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {
      
      template <typename Value, typename KeyAllocator>
      class ByteTrieValueIterator :
	  public ByteTrieBaseIterator<
	      Value, KeyAllocator, ByteTrieValueIterator<Value, KeyAllocator>
	  > {
      public:
	typedef ByteTrieBaseIterator<
	    Value, KeyAllocator, ByteTrieValueIterator<Value, KeyAllocator>
	> ParentType;
	using ParentType::Location;

	typedef Value value_type;
	typedef const Value& reference;

      protected:
	using ParentType::getValue_;

      public:
	ByteTrieValueIterator(
	    const KeyAllocator& keyAllocator = Keyallocator()
	): ByteTrieValueIterator(nullptr, keyAllocator) {
	}

	ByteTrieValueIterator(
	    const uint8_t* base,
	    const KeyAllocator& keyAllocator = Keyallocator()
	): ParentType(base, keyAllocator) {
	}
	
	ByteTrieValueIterator(const uint8_t* base,
			      const std::vector<Location>& path,
			      const KeyAllocator& keyAllocator =
			          KeyAllocator()):
	    ParentType(base, path, keyAllocator) {
	}
	ByteTrieValueIterator(const uint8_t* base,
			      std::vector<Location>&& path,
			      const KeyAllocator& keyAllocator =
			          KeyAllocator()):
f	    ParentType(base, std::move(path), keyAllocator) {
	}
	
	ByteTrieValueIterator(const ByteTrieValueIterator&) = default;
	ByteTrieValueIterator(ByteTrieValueIterator&&) = default;
	ByteTrieValueIterator(const ParentType& other):
	    ParentType(other) {
	}
	ByteTrieValueIterator(ParentType&& other):
	  ParentType(std::move(other)) {
	}

	reference operator*() const { return getValue_(); }

	ByteTrieValueIterator& operator=(const ByteTrieValueIterator&) =
            default;
	ByteTrieValueIterator& operator=(ByteTrieValueIterator&&) = default;
	ByteTrieValueIterator& operator=(const ParentType& other) {
	  ParentType::operator=(other);
	  return *this;
	}
	ByteTrieValueIterator& operator=(ParentType&& other) {
	  ParentType::operator=(std::move(other));
	  return *this;
	}
      };

    }
  }
}
#endif
