#ifndef __INVERTED_PENGUIN__CORE__BYTETRIE_HPP__
#define __INVERTED_PENGUIN__CORE__BYTETRIE_HPP__

#include <inverted_penguin/core/Optional.hpp>
#include <inverted_penguin/core/detail/ByteTrieBucketNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieDenseNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieIterators.hpp>
#include <inverted_penguin/core/detail/ByteTrieLocation.hpp>
#include <inverted_penguin/core/detail/ByteTrieNodePosition.hpp>
#include <inverted_penguin/core/detail/ByteTrieSparseNode.hpp>
#include <pistis/typeutil/LambdaOverload.pp>
#include <memory>
#include <tuple>
#include <vector>

#include <assert.h>
#include <stdint.h>
#include <string.h>

namespace inverted_penguin {
  namespace core {

    template <typename Value, typename KeyAllocator = std::allocator<uint8_t> >
    class ByteTrie {
    public:
      typedef detail::ByteTrieIterator<Value, KeyAllocator> ConstIterator;
      typedef ConstIterator Iterator;
      typedef detail::ByteTrieKeyIterator<Value, KeyAllocator> KeyIterator;
      typedef detail::ByteTrieValueIterator<Value, KeyAllocator> ValueIterator;

    protected:
      typedef detail::ByteTrieDenseNode<Value> DenseNode;
      typedef detail::ByteTrieSparseNode<Value> SparseNode;
      typedef detail::ByteTrieShortBukcetNode<Value> BucketNode;
      typedef detail::ByteTrieLongBucketNode<Value> LongBucketNode;
      typedef detail::ByteTrieNodePosition Position;
      typedef detail::ByteTrieLocation Location;
      
      static constexpr const uint32_t TYPE_MASK =
	  ByteTrieNode<Value>::TYPE_MASK;
      
    public:
      uint32_t numValues() const {
	uint32_t total = 0;
	walk_([&total](const auto& node) { total += node.numValues(); });
	return total;
      }
      
      uint32_t numNodes() const {
	uint32_t total = 0;
	walk_([&total](const auto& node) { ++total; });
	return total;
      }
      
      uint32_t numBuckets() const {
	using pistis::typeutil::overloadLambda;
	uint32_t total = 0;
	walk_(overloadLambda([&total](const BucketNode&) { ++total; },
			     [&total](const LongBucketNode&) { ++total; },
			     [](const SparseNode&) { },
			     [](const DenseNode&) { }));
	return total;
      }
      
      size_t sizeInBytes() const {
	size_t total = 0;
	walk_([&total](const auto& node) { total += node.sizeInBytes(); });
	return total;
      }

      ConstIterator begin() const {
	return ConstIterator(base_, pathToFirstValue_(), keyAllocator);
      }
      ConstIterator end() const {
	return ConstIterator(base_, nullptr, keyAllocator);
      }
      ConstIterator cbegin() const { return begin(); }
      ConstIterator cend() const { return end(); }

      ConstIterator find(const uint8_t* key, const uint8_t* keyEnd) const {
	return ConstIterator(base_, pathToKey_(key, keyEnd), keyAllocator);
      }
      
      ConstIterator find(const char* key) {
	return find((const uint8_t*)key, (const uint8_t*)key + ::strlen(key));
      }

      std::tuple<ConstIterator, ConstIterator> prefix(
	  const uint8_t* key, const uint8_t* keyEnd
      ) const {
	std::vector<Location> start, end;

	std::tie(start, end) = pathToPrefix_(key, keyEnd);
	return std::make_tuple(
	     ConstIterator(base_, std::move(start), keyAllocator),
	     ConstIterator(base_, std::move(end), keyAllocator)
	);
      }

      std::tuple<ConstIterator, ConstIterator> prefix(const char* key) const {
	return prefix((const uint8_t*)key, (const uint8_t*)key + ::strlen(key));
      }

      bool has(const uint8_t* key, const uint8_t* keyEnd) const {
	const auto searchBucket = 
	    [](const auto& node, const uint8_t* k, const uint8_t* keyEnd) {
	      return node.find(k, keyEnd) != node.end();
	    };
	const auto searchInnerNode =
	    [](const auto& node, uint8_t k) {
	      return node.findValue(k) != node.end();
	    };
	const auto failAtInnerNode =
	    [](const auto& node, const uint8_t* key, const uint8_t* keyEnd) {
	      return false;
	    };
	const auto doNothing = [](const auto& parent, Position p) { };
	const auto failOnEmptyKey = []() { return false; }

	return descend_(key, keyEnd, searchBucket, searchInnerNode,
			failAtInnerNode, doNothing, failOnEmptyKey);
      }
      
      bool has(const char* key) const {
	return has((const uint8_t*)key, (const uint8_t*)key + ::strlen(key));
      }

      Optional<Value> get(const uint8_t* key, const uint8_t* keyEnd) const {
	const auto searchBucket =
	    [](const auto& node, const uint8_t* k, const uint8_t* keyEnd) {
	      Position p = node.find(k, keyEnd);
	      if (p != node.end()) {
		return Optional<Value>(node.valueAt(p.index()));
	      } else {
		return Optional<Value>();
	      }
	    };
	const auto searchInnerNode =
	    [](const auto& node, uint8_t k) {
	      Position p = node.findValue(k);
	      if (p != node.end()) {
		return Optional<Value>(node.valueAt(p.index()));
	      } else {
		return Optional<Value>();
	      }
	    };
	const auto failAtInnerNode =
	    [](const auto& node, const uint8_t* k, const uint8_t* keyEnd) {
	      return Optional<Value>();
	    };
	const auto doNothing = [](const auto& parent, Position p) { };
	const auto failOnEmptyKey = []() { return Optional<Value>(); }

	return descend_(key, keyEnd, searchBucket, searchInnerNode,
			failAtInnerNode, doNothing, failOnEmptyKey);
      }

      Optional<Value> get(const char* key) const {
	return get((const uint8_t*)key, (const uint8_t*)key + ::strlen(key));
      }

    protected:
      const uint8_t* base_;
      uint32_t rootOffset_;
      KeyAllocator keyAllocator_;
      
      ByteTrie(const uint8_t* base, const KeyAllocator& keyAllocator):
	  base_(base), rootOffset_(*(uint32_t*)base),
	  keyAllocator_(keyAllocator) {
      }

      template <typename Function>
      void walk_(Function f) {
	std::vector<uint32_t> stack{ rootOffset_; }

	while (stack.size()) {
	  const uint32_t o = stack.back();
	  stack.pop_back();

	  switch (o & TYPE_MASK_) {
	    case BucketNode::NODE_TYPE:
	      f(BucketNode(base_, o & ~TYPE_MASK_));
	      break;

	    case LongBucketNode::NODE_TYPE:
	      f(LongBucketNode(base_, o & ~TYPE_MASK));
	      break;

	    case SparseNode::NODE_TYPE: {
	      SparseNode node(base_, o & ~TYPE_MASK);
	      f(node);
	      node.forEachChild([&stack](const uint8_t* base,
					 uint32_t offset) {
		  stack.push_back(offset);
	      });
	      break;
	    }

	    case DenseNode::NODE_TYPE: {
	      DenseNode node(base_, o & ~TYPE_MASK);
	      f(node);
	      node.forEachChild([&stack](const uint8_t* base,
					 uint32_t offset) {
		  stack.push_back(offset);
	      });
	      break;
	    }

	    default:
	      assert(false);
	  }
	}
      }

      template <typename EndAtBucketFn, typename EndAtInnerNodeFn,
		typename EndInsideKeyFn, typename DescendToChildFn,
		typename EmptyKeyFn>
      auto descend_(const uint8_t* const key, const uint8_t* const keyEnd,
		    EndAtBucketFn endAtBucket, EndAtInnerNodeFn endAtInnerNode,
		    EndInsideKeyFn endInsideKey,
		    DescendToChildFn descendToChild,
		    EmptyKeyFn handleEmptyKey) const {
	if (k >= keyEnd) {
	  return handleEmptyKey();
	} else {
	  const uint8_t* k = key;
	  uint32_t o = rootOffset_;

	  while (k < keyEnd) {
	    const uint32_t nodeType = o & TYPE_MASK;
	    const uint32_t nodeOffset = o & ~TYPE_MASK;
	    
	    switch (nodeType) {
	      case BucketNode::NODE_TYPE:
		return endAtBucket(BucketNode(base_, nodeOffset), k, keyEnd);

	      case LongBucketNode::NODE_TYPE:
		return endAtBucket(LongBucketNode(base_, nodeOffset), k,
				   keyEnd);

	      case SparseNode::NODE_TYPE: {
		SparseNode node(base_, nodeOffset);
		if ((keyEnd - k) == 1) {
		  return endAtInnerNode(node, *k);
		} else {
		  Position p = node.findChild(*k);
		  if (p == node.end()) {
		    return endInsideKey(node, key, keyEnd);
		  } else {
		    descendToChild(node, p.index());
		    o = node.childAt(p.index());
		    ++k;
		  }
		}
		break;
	      }
		
	      case DenseNode::NODE_TYPE: {
		DenseNode node(base_, nodeOffset);
		if ((keyEnd - k) == 1) {
		  return endAtInnerNode(node, *k);
		} else {
		  Position p = node.findChild(*k);
		  if (p == node.end()) {
		    return endInsideKey(node, key, keyEnd);
		  } else {
		    descendToChild(node, p);
		    o = node.childAt(p.index());
		    ++k;
		  }
		}
		break;
	      }
		
	      default:
		assert(false);  // Should never get here
	    }
	  }

	  // Should never get here if the loop above is correct
	  assert(false);
	}
      }

      std::vector<Location> pathToFirstValue_() const {
	uint32_t o = root_;
	std::vector<Location> path;
	Position p;

	while (o) {
	  const uint32_t nodeOffset = o & ~TYPE_MASK;
	  switch (o & TYPE_MASK) {
	    case BucketNode::NODE_TYPE:
	      p = BucketNode(base_, nodeOffset).begin();
	      assert(p.type() == Position::AT_VALUE);
	      
	      path.emplace_back(nodeOffset, p);
	      o = 0;
	      break;

	    case LongBucketNode::NODE_TYPE:
	      p = LongBucketNode(base_, nodeOffset).begin();
	      assert(p.type() == Position::AT_VALUE);

	      path.emplace_back(nodeOffset, p);
	      o = 0;
	      break;

	    case SparseNode::NODE_TYPE: {
	      SparseNode node(base_, nodeOffset);
	      p = node.begin();
	      assert(p.type() != Position::AT_END);
	      
	      path.emplace_back(nodeOffset, p);
	      o = (p.type() == Position::AT_CHILD) ?
		      node.childAt(p.index()) : 0;
	    }

	    case DenseNode::NODE_TYPE: {
	      DenseNode node(base_, nodeOffset);
	      p = node.begin();
	      assert(p.type() != Position::AT_END);

	      path.emplace_back(nodeOffset, p);
	      o = (p.type() == Position::AT_CHILD) ?
		      node.childAt(p.index()) : 0;
	    }

	    default:
	      assert(false);
	  }
	}

	return std::move(path);
      }

      std::vector<Location> pathToKey_(const uint8_t* key,
				       const uint8_t* keyEnd) const {
	std::vector<Location> path;
	const auto searchBucket =
	    [&path](const auto& node, const uint8_t* key,
		    const uint8_t* keyEnd) {
	      Position p = node.find(key, keyEnd);
	      if (p == node.end()) {
		return std::vector<Location>();
	      } else {
		path.emplace_back(node.trieOffset(), p);
		return std::move(path);
	      }
	    };
	const auto searchInnerNode =
	    [&path](const auto& node, uint8_t key) {
	      Position p = node.findValue(key);
	      if (p == node.end()) {
		return std::vector<Location>();
	      } else {
		path.emplace_back(node.trieOffset(), p);
		return std::move(path);
	      }
	    };
	const auto failAtInnerNode =
	    [](const auto& node, const uint8_t* key, const uint8_t* keyEnd) {
	      return std::vector<Location>();
	    };
	const auto descendToChild =
	    [&path](const auto& node, Position child) {
	      path.emplace_back(node.trieOffset(), child);
	    };
	const auto failOnEmptyKey = []() { return std::vector<Location>(); }

	return descend_(key, keyEnd, searchBucket, searchInnerNode,
			failAtInnerNode, descendToChild, failOnEmptyKey);
      }

      std::tuple<std::vector<Location>, std::vector<Location>> pathToPrefix_(
	  const uint8_t* key, const uint8_t* keyEnd
      ) const {
	std::vector<Location> beginPath;
	std::vector<Location> endPath;
	const auto searchBucket =
	    [this, &beginPath, &endPath](const auto& node, const uint8_t* k,
					 const uint8_t* keyEnd) {
	      Position s, e;
	      std::tie(s, e) = node.prefix(k, keyEnd);
	      if (s == e) {
		return std::make_tuple(std::vector<Location>(),
				       std::vector<Location>());
	      } else {
		beginPath.emplace_back(node.trieOffset(), s);
		if (e != node.end()) {
		  endPath.emplace_back(node.trieOffset(), e);
		} else {
		  setToNextValue_(endPath);
		}
		return std::make_tuple(beginPath, endPath);
	      }
	    };
	const auto searchInnerNode =
	  [this, &beginPath, &endPath](const auto& node, const uint8_t k) {
	      Position p = node.findKey(k);
	      if (p == node.end()) {
		return std::make_tuple(std::vector<Location>(),
				       std::vector<Location>());
	      } else {
		beginPath.emplace_back(node.trieOffset(), p);

		Position next = node.after(p);
		if (next != node.end()) {
		  endPath.emplace_back(node.trieOffset(), end);
		} else {
		  setToNextValue_(endPath);
		}
		return std::make_tuple(beginPath, endPath);
	      }
	    };
	const auto failAtInnerNode =
	    [](const auto& node, const uint8_t* k, const uint8_t* keyEnd) {
	      return std::make_tuple(std::vector<Location>(),
				     std::vector<Location>());
	    };
	const auto descendToChild =
	    [&beginPath, &endPath](const auto& parent, Position child) {
	      beginPath.emplace_back(parent.trieOffset(), child);
	      endPath.emplace_back(parent.trieOffset(), child);
	    };
	const auto handleEmptyKey =
	    []() {
	      return std::make_tuple(std::vector<Location>(),
				     std::vector<Location>());
	    };
	
	return descend_(key, keyEnd, searchBucket, searchInnerNode,
			failAtInnerNode, descendToChild, handleEmptyKey);
      }

      void setToNextValue_(std::vector<Location>& path) {
	while (path.size()) {
	  switch (path.back().nodeType()) {
	    case BucketNode::NODE_TYPE:
	    case LongBucketNode::NODE_TYPE:
	      // Should never happen
	      assert(false);

	    case SparseNode::NODE_TYPE: {
	      const SparseNode node(base_, path.back().nodeOffset());
	      if (setToNextValue_(path, node)) {
		return;
	      }
	      break;
	    }

	    case DenseNode::NODE_TYPE: {
	      const DenseNode node(base_, path.back().nodeOffset());
	      if (setToNextValue_(path, node)) {
		return;
	      }
	      break;
	    }

	    default:
	      assert(false);
	  }
	}
      }

      template <typename Node>
      bool setToNextValue_(std::vector<Location>& path, const Node& node) {
	Position next = node.after(path.back().position);
	if (next == node.end()) {
	  path.pop_back();
	  return false;
	} else {
	  path.back().position = next;
	  return true;
	}
      }
    };

  }
}
#endif
