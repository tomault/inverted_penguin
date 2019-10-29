#ifndef __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIELOCATION_HPP__
#define __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIELOCATION_HPP__

#include <inverted_penguin/core/ByteTrieNodePosition.hpp>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      struct ByteTrieLocation {
      private:
	static constexpr const uint32_t TYPE_MASK = 0x3;

      public:
	typedef ByteTrieNodePosition Position;
	
	uint32_t node;      ///< Offset from trie base, plus type bits
	Position position;  ///< Position

	ByteTrieLocation(): node(0), position() { }
	ByteTrieLocation(uint32_t n, Position p): node(n), position(p) { }

	uint32_t nodeType() const { return node & TYPE_MASK; }
	uint32_t nodeOffset() const { return node & ~TYPE_MASK; }
      };
      
    }
  }
}

#endif

