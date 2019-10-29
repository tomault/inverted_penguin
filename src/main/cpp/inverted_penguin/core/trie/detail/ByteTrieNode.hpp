#ifndef __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIENODE_HPP__
#define __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIENODE_HPP__

#include <inverted_penguin/core/detail/ByteTrieNodePosition.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderKeyBuilder.hpp>
#include <limits>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value>
      class ByteTrieNode {
      public:
	typedef ByteTrieNodePosition Position;
	typedef ByteTrieBuilderKeyBuilder KeyBuilder;

	static constexpr const uint32_t TYPE_MASK = Location::TYPE_MASK;
	static constexpr const uint32_t NODE_ALIGNMENT = 4;
	static constexpr const size_t VALUE_ALIGNMENT = alignof(Value);
	static constexpr const size_t MAX_TRIE_SIZE =
	    std::numeric_limits<uint32_t>::max();

      public:
	ByteTrieNode(const uint8_t* base, uint32_t nodeOffset):
	    base_(base), start_(base + nodeOffset) {
	}

	uint32_t trieOffsetOnly() const { return (uint32_t)(start_ - base_); }
	Position end() const { return Position(); }

      protected:
	const uint8_t* base_;
	const uint8_t* start_;

	template <typename T>
	static constexpr T* alignTo_(T* p, size_t alignment) {
	  return (p + (alignment - 1)) & ~alignment;
	}

	static constexpr uint32_t alignSize_(uint32_t offset,
					     uint32_t alignment) {
	  return (offset + (alignment - 1)) & ~alignment;
	}

	static constexpr uint32_t pad_(uint32_t offset, uint32_t alignment) {
	  return (alignment - 1) - ((offset - 1) & (alignment - 1));
	}

	static int16_t searchForKey_(const uint8_t* keys, uint16_t n,
				     uint8_t k) const {
	  int16_t i = 0;
	  int16_t j = n; // Safe because n <= 256

	  while (i < j) {
	    int16_t k = (i + j) >> 1; // Safe because i,j in [0, 256]
	    if (keys[k] < k) {
	      i = k + 1;
	    } else if (k < keys[k]) {
	      j = k;
	    } else {
	      return i;
	    }
	  }
	  return -i - 1;
	}
	
	static int16_t searchForKeyAfter_(const uint8_t* keys, uint16_t n,
					  uint8_t k) const {
	  int16_t i = 0;
	  int16_t j = n; // Safe because n <= 256

	  while (i < j) {
	    int16_t k = (i + j) >> 1; // Safe because i,j in [0, 256]
	    if (keys[k] <= k) {
	      i = k + 1;
	    } else {
	      j = k;
	    }
	  }
	  return i;
	}

	template <typename Writer>
	static size_t writePad_(Writer writer, size_t numWritten,
				size_t alignment) {
	  static const uint8_t buffer[16] = { 0, 0, 0, 0, 0, 0, 0, 0,
					      0, 0, 0, 0, 0, 0, 0, 0 };
	  const size_t nToPad = (-numWritten) & (alignment - 1);
	  size_t n = nToPad;

	  while (n) {
	    const size_t nToWrite = (n < sizeof(buffer)) ? n : sizeof(buffer);
	    writer(buffer, nToWrite);
	    n -= nToWrite;
	  }

	  return nToPad;
	}

	
      };
      
    }
  }
}
#endif
