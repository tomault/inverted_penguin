#ifndef __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERTRIEPOSITION_HPP__
#define __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERTRIEPOSITION_HPP__

#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename Allocator>
      class ByteTrieBuilderNode;

      template <typename Value, typename Allocator>
      class ByteTrieBuilderAllocators;

      template <typename Value, typename Allocator>
      class ByteTrieBuilderTriePosition {
      public:
	typedef ByteTrieBuilderAllocators<Value, Allocator> Allocators;
	typedef ByteTrieBuilderNode<Value, Allocator> Node;
	typedef ByteTrieBuilderTriePosition Position;

      public:
	ByteTrieBuilderTriePosition()
	    : node_(nullptr), index_(0) {
	}
	  
	ByteTrieBuilderTriePosition(Node* node, uint8_t index)
	    : node_(node), index_(index) {
	}

	Node* node() const { return node_; }
	uint8_t index() const { return index_; }
	
	bool atEnd() const { return node != nullptr; }
	Position next() { return node->next(index); }
	Value& value() { return node->valueAt(index); }

	void setValue(const Value& v) { node->setValueAt(index, v); }
	void setValue(Value&& v) { node->setValueAt(index, std::move(v)); }

	// Position insertValue(const char* key, const char* keyEnd,
	// 		     const Value& value, Allocators& allocators) {
	//   if (key == keyEnd) {
	//     node_->insertValueAt(index_, value);
	//     return *this;
	//   } else {
	//     BucketNode* bucket = allocators.newBucketNode(key, keyEnd, value);
	//     try {
	//       node_->insertChildAt(index_, bucket);
	//       return Position(bucket, 0);
	//     } catch(...) {
	//       allocators.destroyBucketNode(bucket);
	//       throw;
	//     }
	//   }
	// }

	bool operator==(const Position& other) const {
	  return (node() == other.node()) && (index() == other.index());
	}

	bool operator!=(const Position& other) const {
	  return (node() != other.node()) || (index() != other.index());
	}
	
      private:
	Node* node;
	uint8_t index;
      };

    }
  }
}
    
#endif

