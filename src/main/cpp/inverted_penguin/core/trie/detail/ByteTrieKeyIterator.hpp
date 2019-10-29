#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEKEYITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEKEYITERATOR_HPP__

#include <inverted_penguin/core/detail/ByteTrieBaseIterator.hpp>
#include <vector>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename KeyAllocator>
      class ByteTrieKeyIterator :
	  public ByteTrieBaseIterator<
	      Value, KeyAllocator, ByteTrieKeyIterator<Value, KeyAllocator>
	  > {
      public:
	typedef ByteTrieBaseIterator<
	    Value, KeyAllocator, ByteTrieKeyIterator<Value, KeyAllocator>
	> ParentType;
	using ParentType::Key;
	using ParentType::Location;

	typedef Key value_type;
	typedef Key reference;

      protected:
	using ParentType::createKey_;

      public:
	ByteTrieKeyIterator(const KeyAllocator& keyAllocator = KeyAllocator()):
	    ByteTrieKeyIterator(nullptr, keyAllocator) {
	}
	
	ByteTrieKeyIterator(const uint8_t* base,
			    const KeyAllocator& keyAllocator = KeyAllocator()):
	    ParentType(base, keyAllocator) {
	}
	
	ByteTrieKeyIterator(const uint8_t* base,
			    const std::vector<Location>& path,
			    const KeyAllocator& keyAllocator = KeyAllocator()):
	    ParentType(base, path, keyAllocator) {
	}
	
	ByteTrieKeyIterator(const uint8_t* base, std::vector<Location>&& path,
			    const KeyAllocator& allocator = KeyAllocator()):
	    ParentType(base, std::move(path), allocator) {
	}
	
	ByteTrieKeyIterator(const ByteTrieKeyIterator&) = default;
	ByteTrieKeyIterator(ByteTrieKeyIterator&&) = default;
	
	ByteTrieKeyIterator(const ParentType& other): ParentType(other) { }
	
	ByteTrieKeyIterator(ParentType&& other):
	    ParentType(std::move(other)) {
	}

	reference operator*() const { return createKey_(); }

	ByteTrieKeyIterator& operator=(const ByteTrieConstIterator&) =
            default;
	
	ByteTrieKeyIterator& operator=(ByteTrieConstIterator&&) = default;
	
	ByteTrieKeyIterator& operator=(const ParentType& other) {
	  ParentType::operator=(other);
	  return *this;
	}
	
	ByteTrieKeyIterator& operator=(ParentType&& other) {
	  ParentType::operator=(std::move(other));
	  return *this;
	}
      };

    }
  }
}

#endif

