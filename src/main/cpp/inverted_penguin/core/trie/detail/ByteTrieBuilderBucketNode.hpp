#ifndef __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUCKETNODE_HPP__
#define __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUCKETNODE_HPP__

#include <inverted_penguin/core/detail/ByteTrieBuilderAllocators.hpp>
#include <inverted_penguin/core/detail/ByteTrieBucketNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderKey.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderKeyValue.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderTriePosition.hpp>
#include <pistis/exceptions/IllegalStateError.hpp>
#include <algorithm>
#include <functional>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <assert.h>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderBucketNode :
	  public ByteTrieBuilderNode<Value, BaseAllocator> {
      public:
	typedef ByteTrieBuilderNode<Value, BaseAllocator> ParentType;
	using typename ParentType::Allocators;
	using typename ParentType::Node;
	using typename ParentType::Position;

	static constexpr const uint32_t NODE_TYPE =
	    ByteTrieShortBukcetNode<Value>::NODE_TYPE;
	static constexpr const uint32_t MAX_BUCKET_SIZE = 16;

	typedef ByteTrieBuilderKey<typename Allocators::ByteAllocator> KeyType;
	typedef ByteTrieBuilderKeyValue<Value> KeyValue;
	
      public:
	ByteTrieBuilderBucketNode(): values_() { }
	ByteTrieBuilderBucketNode(const uint8_t* key, const uint8_t* keyEnd,
				  const Value& v);
	ByteTrieBuilderBucketNode(std::vector<KeyValue>&& values):
	    values_(std::move(values)) {
	  ::assert(values_.size() <= MAX_BUCKET_SIZE);
	}
	ByteTrieBuilderBucketNode(const ByteTrieBuilderBucketNode&) = delete;
	virtual ~ByteTrieBuilderBucketNode() { }

	virtual uint32_t nodeType() const override { return NODE_TYPE; }
	virtual uint32_t numChildren() const override { return 0; }
	virtual uint32_t numValues() const override { return values_.size(); }
	virtual Position begin() const override {
	  return Position(0, Position::AT_VALUE);
	}
	virtual Position end() const override { return Position(); }

	bool hasLongKeys() const { return hasLongKeys_(); }
	
	virtual Position next(Position p) const override {
	  assert(p.type() == Position::AT_VALUE);
	  const uint8_t nextIndex = index + 1;
	  return (nextIndex < values_.size()) ?
	      Position(nextIndex, Position::AT_VALUE) :
	      Position();
	}
	
	virtual void writeKey(std::ostream& out, Position p) const override {
	  assert(p.type() == Position::AT_VALUE);
	  const auto& key = values_[p.index()].key();
	  out.write(key.begin(), key.size());
	}
	
	Position find(const uint8_t* key, const uint8_t* keyEnd) const {
	  int16_t p = this->find_(key, keyEnd);
	  return (p >= 0) ? Position(p, Position::AT_VALUE) : Position();
	}

	std::tuple<Position, Position> prefix(
	    const uint8_t* key, const uint8_t* keyEnd
	) const {
	  uint8_t l, h;

	  std::tie(l, h) = findPrefix_(key, keyEnd);
	  return std::make_tuple(makePosition_(l), makePosition_(h));
	}

	virtual Value valueAt(uint8_t index) const override {
	  return values_[index].value();
	}

	virtual void setValueAt(uint8_t index, const Value& v) override {
	  values_[index].setValue(v);
	}

	virtual void setValueAt(uint8_t index, Value&& v) override {
	  values_[index].setValue(std::move(v));
	}

	virtual Node* add(const uint8_t* key, const uint8_t* keyEnd,
			  const Value& value, Allocators& allocators) {
	  assert(key < keyEnd);
	  int16_t p = find_(key, keyEnd);
	  if (p >= 0) {
	    values_[p].setValue(value);
	  } else {
	    KeyType k = allocators.createKey(key, keyEnd);
	    values_.insert(values_.begin() - p - 1,
			   KeyValue(std::move(k), value));
	    return split_(this, allocators);
	  }
	}

	virtual size_t computeTreeDataSize() const override {
	  return ByteTrieBucketNode<Value>::computeTreeDataSize(
	      values_.size(), values_.begin()
	  );
	}

	virtual size_t write(
	    uint32_t baseOffset, // Not used
	    const std::function<void (const void*, size_t)>& writeData
	) override {
	  if (hasLongKeys_()) {
	    return ByteTrieLongBucketNode<Value>::writeTreeData(
		values_.size(),	values_.begin(), writeData
	    );
	  } else {
	    return ByteTrieBucketNode<Value>::writeTreeData(
		values_.size(), values_.begin(), writeData
	    );
	  }
	}

	virtual uint32_t serializedTypeCode() const {
	  return hasLongKeys_() ? ByteTrieLongBucketNode::NODE_TYPE
	                        : ByteTrieShortBukcetNode::NODE_TYPE;
	}

	virtual void destroy(Allocators& allocators) override {
	  for (auto& p : values_) {
	    p.destroy(allocators);
	  }
	  values_.clear();
	}

	ByteTrieBuilderBucketNode& operator=(
	    const ByteTrieBuilderBucketNode&
	) = delete;
	
      private:
	std::vector<KeyValue> values_;

	Position makePosition_(uint8_t index) {
	  return (index < values_.size()) ?
	      Position(index, Position::AT_VALUE) :
	      Position();
	}
	
	int16_t find_(cont uint8_t* key, const uint8_t* keyEnd) const {
	  uint8_t l, h;

	  std::tie(l, h) = findPrefix_(key, keyEnd);
	  if (((h - l) != 1) || (values_[l].key().size() != (keyEnd - key))) {
	    return -l - 1;
	  } else {
	    return l;
	  }
	}
	
	std::tuple<uint8_t, uint8_t> findPrefix_(const uint8_t* key,
						 const uint8_t* keyEnd) const {
	  const uint8_t* k = key;
	  uint8_t l = 0;
	  uint8_t h = values_.size();
	  uint32_t d = 0;

	  while ((k < keyEnd) && ((h - l) > 1)) {
	    l = lowerBound_(*k, l, h, d);
	    h = upperBound_(*k, l, h, d);
	    ++k; ++d;
	  }

	  if ((l < h) && (k < keyEnd)) {
	    const auto& target = values_[l].key();
	    if (target.size() < (keyEnd - key)) {
	      l = h; // l comes before key and h comes after it
	    } else {
	      int order = ::memcmp(target.begin() + d, k, keyEnd - k);
	      if (order < 0) {
		l = h; // l comes before key and h comes after it
	      } else {
		h = l;  // l comes after key and h after l
	      } // else l begins with the key
	    }
	  }
	  
	  return std::make_tuple(l, h);
	}

	uint8_t lowerBound_(uint8_t k, uint8_t l, uint8_t h, uint32_t d) const {
	  while (l < h) {
	    const uint16_t mid = ((uint16_t)l + (uint16_t)h) >> 1;
	    const auto& current = values_[mid].key();
	    if ((current.size() < d) || (current[d] < k)) {
	      l = mid + 1;
	    } else {
	      h = mid;
	    }
	  }
	  return l;
	}

	uint8_t upperBound_(uint8_t k, uint8_t l, uint8_t h, uint32_t d) const {
	  while (l < h) {
	    const uint16_t mid = ((uint16_t)l + (uint16_t)h) >> 1;
	    const auto& current = values_[mid].key();
	    if ((current.size() < d) || (current[d] <= k)) {
	      l = mid + 1;
	    } else {
	      h = mid;
	    }
	  }
	  return l;
	}

	static Node* split_(ByteTrieBuilderBucketNode* original,
			    Allocators& allocators) {
	  if (original->values_.size() <= MAX_BUCKET_SIZE) {
	    return original;
	  } else {
	    Node* newNode = split_(original->values_.begin(),
				   original->values_.end(), allocators, 0);
	    allocators.destroyBucketNode(original);
	    return newNode;
	  }
	}

	static Node* split_(const std::vector<KeyValue>::const_iterator& start,
			    const std::vector<KeyValue>::const_iterator& end,
			    Allocators& allocators, size_t offset) {
	  // Create a new ByteTrieBuilderSparseNode by splitting the
	  // (key, value) pairs in this node by breaking that list
	  // into ranges that share the same start byte.  For each such
	  // range, do the following:
	  // - Put length one keys into a set whose values will be kept
	  //   inside the new ByteTrieBuilderSparseNode itself
	  // - If the size of the range less its size one key (there can
	  //   only be no or one such key) fits in a bucket node, create
	  //   a new ByteTrieBuilderBucketNode to hold them.  Otherwise,
	  //   call split_ recursively on this range to create a
	  //   new ByteTrieBuilderSparseNode for it.  Put the new node into
	  //   a list from the first byte to that node.;
	  // Use the two lists created above to create a new
	  // ByteTrieBuilderSparseNode and return it
	  std::unordered_map<uint8_t, Node*> newChildren;
	  std::unordered_map<uint8_t, Value> newValues;
	  auto i = start;

	  ::assert((end - start) > MAX_BUCKET_SIZE);
	  while (i != end) {
	    auto j = i + 1;
	    const KeyType& firstKey = i->key();
	    const uint8_t b = firstKey[offset];
	    uint8_t j = upperBound_(b, i, values_.size(), offset);

	    // Since the keys are sorted, if there is a key whose length is
	    // equal to (offset + 1), it must be at the beginning
	    const size_t firstKeyLength = firstKey.size();
	    if (firstKey.size() == (offset + 1)) {
	      newValues.insert(std::make_pair(b, i->value());
	      ++i;
	    }

	    if ((j - i) > MAX_BUCKET_SIZE) {
	      Node* const child = split_(i, j, allocators, offset + 1);
	      newChildren.insert(std::make_pair(b, child));
	    } else if (i < j) {
	      std::vector<KeyValue> tmp;
	      while (i < j) {
		const KeyType& key = i->key();
		KeyType newKey = allocators.createKey(key.begin() + offset + 1,
						      key.end());
		tmp.emplace_back(std::move(newKey), std::get<1>(*i));
		++i;
	      }

	      Node* bucket = allocators.newBucketNode(std::move(tmp));
	      newChildren.insert(std::make_pair(b, bucket));
	    }

	    i = j;
	  }

	  return allocators.newSparseNode(std::move(newChildren),
					  std::move(newValues));
	}

	bool hasLongKeys_() const {
	  size_t totalSize = 0;
	  for (auto kv = values_.begin();
	       (kv != values_.end()) && (totalSize < 256);
	       ++kv) {
	    totalSize += kv->key().size();
	  }
	  return totalSize < 256;
	}
	
      };
    }
  }
}
#endif
