#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERITERATORLOCATION_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERITERATORLOCATION_HPP__

#include <inverted_penguin/core/detail/ByteTrieBuilderNode.hpp>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, BaseAllocator>
      struct ByteTrieBuilderIteratorLocation {
	ByteTrieBuilderNode<Value, BaseAllocator>* node;
	typename ByteTrieBuilderNode<Value, BaseAllocator>::Location location;

	ByteTrieBuilderIteratorLocation() { }
	ByteTrieBuilderIteratorLocation(
	    ByteTrieBuilderNode<Value, BaseAllocator>* n,
	    const typename ByteTrieBuilderNode<
	        Value, BaseAllocator
	    >::Location& l
	):
	    node(n), location(l) {
	}
      };
      
    }
  }
}
#endif
