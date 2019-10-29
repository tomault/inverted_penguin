#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBASEITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBASEITERATOR_HPP__

#include <inverted_penguin/core/detail/ByteTrieBucketNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderKey.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderKeyBuilder.hpp>
#include <inverted_penguin/core/detail/ByteTrieDenseNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieLocation.hpp>
#include <inverted_penguin/core/detail/ByteTrieNodePosition.hpp>
#include <inverted_penguin/core/detail/ByteTrieSparseNode.hpp>
#include <iterator>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename KeyAllocator,
		typename DerivedIterator>
      class ByteTrieBaseIterator : KeyAllocator {
      public:
	typedef ByteTrieBuilderKey<KeyAllocator> Key;
	typedef ByteTrieBuilderKeyBuilder<KeyAllocator> KeyBuilder;
	typedef ByteTrieNodePosition Position;
	typedef ByteTrieLocation Location;

	typedef ByteTrieShortBucketNode<Value> ShortBucketNode;
	typedef ByteTrieLongBucketNode<Value> LongBucketNode;
	typedef ByteTrieSparseNode<Value> SparseNode;
	typedef ByteTrieDenseNode<Value> DenseNode;

	typedef std::forward_iterator_category iterator_category;
	typedef ptrdiff_t difference_type;

	static constexpr const uint32_t TYPE_MASK =
	    ByteTrieNode<Value>::TYPE_MASK;

      public:
	DerivedIterator& operator++() {
	  if (base_ && path_.size()) {
	    Location loc = path_.back();
	    Position next = invoke_(loc.node, [&loc](const auto& node) {
	        return node.next(loc.position);
	    });
	    Position end = invoke_(loc.node(), [](const auto& node) {
	      return node.end();
	    });
	    path_.pop_back();
	    
	    while (next == end) {
	      if (path_.empty()) {
		return self_();  // At the end
	      }
	      loc = path_.back();
	      next = invoke_(loc.node, [&loc](const auto& node) {
		  return node.next(loc.position);
	      });
	      end = invoke_(loc.node, [](const auto& node) {
		  return node.end();
	      });
	      path_.pop_back();
	    }

	    assert(!(path_.empty() && (next == end)));
	    if (next.type() == Position::AT_VALUE) {
	      path_.emplace_back(loc.node, next);
	    } else {
	      assert(next.type() == Position::AT_CHILD);
	      path_.emplace_back(loc.node, next);

	      // Find first value
	      uint32_t offset = invoke_(loc.node, [next](const auto& node) {
		return node.childAt(next.index());
	      });
	      next = invoke_(offset, [](const auto& node) {
		return node.begin();
	      });
	      while (next.type() == Position::AT_CHILD) {
		path_.emplace_back(offset, next);
		offset = invoke_(offset, [next](const auto& node) {
		  return node.childAt(next.index());
		});
		next = invoke_(offset, [](const auto& node) {
		  return node.begin();
		});
	      }
	      assert(next.type() == Position::AT_VALUE);
	      path_.emplace_back(offset, next);
	    }
	  }
	  return self_();
	}

	DerivedIterator  operator++(int) {
	  DerivedIterator tmp(self_());
	  ++tmp;
	  return std::move(tmp);
	}

	bool operator==(const ByteTrieBaseIterator& other) const {
	  return (base_ == other.base_) && (path_ == other.path_);
	}

	bool operator!=(const ByteTrieBaseIterator& other) const {
	  return (base_ != other.base_) || (path != other.path_);
	}

      protected:
	const uint8_t* base_;
	std::vector<Location> path_;

	ByteTrieBaseIterator(const KeyAllocator& keyAllocator):
	    KeyAllocator(keyAllocator), base_(nullptr), path_() { }
	
	ByteTrieBaseIterator(const uint8_t* base,
			     const KeyAllocator& keyAllocator):
	    KeyAllocator(keyAllocator), base_(base), path_() {
	}

	ByteTrieBaseIterator(const uint8_t* base,
			     const std::vector<Location>& path,
			     const KeyAllocator& keyAllocator):
	    KeyAllocator(keyAllocator), base_(base), path_(path) {
	}
	
	ByteTrieBaseIterator(const uint8_t* base,
			     std::vector<Location>&& path,
			     const KeyAllocator& keyAllocator):
	    KeyAllocator(keyAllocator), base_(base), path_(std::move(path)) {
	}
	
	ByteTrieBaseIterator(const ByteTrieBaseIterator&) = default;
	ByteTrieBaseIterator(ByteTrieBaseIterator&&) = default;

	const DerivedIterator& self_() const {
	  return static_cast<const DerivedIterator&>(*this);
	}

	const DerivedIterator& self_() {
	  return static_cast<DerivedIterator&>(*this);
	}

	template <typename Function>
	auto invoke_(uint32_t nodeOffset, Function f) {
	  switch(nodeOffset & TYPE_MASK) {
	    case ShortBucketNode::NODE_TYPE:
	      return f(ShortBucketNode(base_, nodeOffset & ~TYPE_MASK));

	    case LongBucketNode::NODE_TYPE:
	      return f(LongBucketNode(base_, nodeOffset & ~TYPE_MASK));

	    case SparseNode::NODE_TYPE:
	      return f(SparseNode(base_, nodeOffset & ~TYPE_MASK));

	    case DenseNode::NODE_TYPE:
	      return f(DenseNode(base_, nodeOffset & ~TYPE_MASK));

	    default:
	      assert(false);
	  }
	}

	Key createKey_() const {
	  KeyBuilder keyBuilder(path_.size(), (KeyAllocator&)*this);

	  for (const Location& loc : path) {
	    invoke_(loc.node, [&loc, &keyBuilder](const auto& node) {
	      node.writeKey(keyBuilder, loc.position);
	    });
	  }

	  return keyBuilder.done();
	}

	const Value& getValue_() const {
	  Location loc = path.back();
	  return invoke_(loc.node, [&loc](const auto& node) {
	    return node.valueAt(loc.position);
	  });
	}
      };
      
    }
  }
}
#endif
