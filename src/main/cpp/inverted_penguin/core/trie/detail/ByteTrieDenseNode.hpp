#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEDENSENODE_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEDENSENODE_HPP__

#include <inverted_penguin/core/detail/ByteTrieNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderKeyBuilder.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderSparseArray.hpp>

#include <algorithm>
#include <functional>
#include <memory>

#include <assert.h>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value>
      class ByteTrieDenseNode : public ByteTrieNode<Value> {
      public:
	typedef ByteTrieNode<Value> ParentType;
	using typename ParentType::Position;
	using typename ParentType::KeyBuilder;
	using ParentType::VALUE_ALIGNMENT;
	using ParentType::MAX_TRIE_SIZE;
	using ParentType::TYPE_MASK;

	using ParentType::end;
	using ParentType::alignSize_;
	using ParentType::pad_;
	using ParentType::writePad_;
	using ParentType::searchForKey_;
	using ParentType::searchForKeyAfter_;

	static constexpr const uint16_t MIN_CHILDREN = 224;
	static constexpr const uint32_t CHILD_OFFSET_SIZE = sizeof(uint32_t);
	static constexpr const uint32_t CHILD_OFFSET_ALIGNMENT =
	    std::max(alignof(uint32_t), VALUE_ALIGNMENT);
	static constexpr const uint32_t CHILD_ARRAY_SIZE =
	    256 * CHILD_OFFSET_SIZE;
	static_assert(VALUE_ALIGNMENT <= CHILD_ARRAY_SIZE);

	static constexpr uint32_t NODE_TYPE = 0;
	
      public:
	ByteTrieDenseNode(const uint8_t* base, uint32_t nodeOffset):
	    ParentType(base, nodeOffset) {
	}

	uint16_t numValues() const { return (uint16_t)this->start_[0] + 1; }
	uint16_t numChildren() const { return (uint16_t)this->start_[1] + 1; }
	size_t sizeInBytes() const {
	  return alignSize_(valueDataOffset_() + numValues() * sizeof(Value),
			    NODE_ALIGNMENT);
	}
	uint32_t trieOffset() const {
	  return this->trieOffsetOnly() | NODE_TYPE;
	}

	Position begin() const {
	  if (numValues()) {
	    if (!numChildren() || (valueKeyPtr_()[0] <= firstChildKey_())) {
	      return Position(0, Position:AT_VALUE);
	    } else {
	      return Position(0, Position::AT_CHILD);
	    }
	  } else if (numChildren()) {
	    return Position(0, Position::AT_CHILD);
	  } else {
	    return end();
	  }
	}

	Position next(Position p) const {
	  const uint8_t* const valueKeys = valueKeyPtr_();
	  const uint32_t* const childOffsets = childDataPtr_();
	  uint16_t valueIndex, childIndex;
	  
	  if (p.type() == Position::AT_VALUE) {
	    if (childOffsets[p.index()]) {
	      return Position(p.index(), Position::AT_CHILD);
	    } else {
	      valueIndex = p.index() + 1;
	      childIndex = firstChildKey_(p.index());
	    }
	  } else {
	    childIndex = firstChildKey_(p.index() + 1);
	    valueIndex = searchForKeyAfter_(valueKeys, numValues(), p.index());
	  }

	  return choosePosition_(childIndex, valueIndex);
	}

	Position after(Position p) const {
	  const uint8_t* const valueKeys = valueKeyPtr_();
	  const uint32_t* const childOffsets = childDataPtr_();
	  uint16_t valueIndex, childIndex;
	  
	  if (p.type() == Position::AT_VALUE) {
	    valueIndex = p.index() + 1;
	    childIndex = firstChildKey_(p.index() + 1);
	  } else {
	    childIndex = firstChildKey_(p.index() + 1);
	    valueIndex = searchForKeyAfter_(valueKeys, numValues(), p.index());
	  }

	  return choosePosition_(childIndex, valueIndex);
	}
	      
	Position findKey(uint8_t key) const {
	  int16_t p = searchForKey_(valueKeyPtr_(), numValues(), key);
	  if (p >= 0) {
	    return Position((uint8_t)p, Position::AT_VALUE);
	  } else if (childDataPtr_()[key]) {
	    return Position(key, Position::AT_CHILD);
	  } else {
	    return end();
	  }
	}

	const Value& valueAt(uint8_t index) const {
	  return valueDataPtr_()[index];
	}

	Position findValue(uint8_t key) const {
	  int16_t p = searchForKey_(valueKeyPtr_(), numValues(), key);
	  return (p >= 0) ? Position((uint8_t)p, Position::AT_VALUE)
	                  : end();
	}

	uint32_t childAt(uint8_t index) const {
	  return childDataPtr_()[index];
	}

	Position findChild(uint8_t key) const {
	  return childDataPtr_()[key] ? Position(key, Position::AT_CHILD)
	                              : end();
	}

	template <typename Function>
	void forEachChild(Function f) const {
	  const uint32_t* const children = childDataPtr_();
	  const uint32_t* const end = children + 256;
	  for (auto p = children; p != end; ++p) {
	    if (*p) {
	      f(this->base_, *p);
	    }
	  }
	}

	template <typename Allocator>
	void writeKey(ByteTrieBuilderKeyBuilder<Allocator>& keyBuilder,
		      Position p) {
	  keyBuilder.write(
	      p.type() == Position::AT_VALUE ? valueAt(p.index())
			                     : childAt(p.index())
	  );
	}

	template <typename BaseAllocator>
	static size_t computeTreeDataSize(
	     const ByteTrieBuilderNode<Value, BaseAllocator>** children,
	     const ByteTrieBuilderSparseArray<Value>& values) {
	  typedef ByteTrieBuilderNode<Value, BaseAllocator> Node;
	  const uint16_t numChildren =
	      std::count_if(children, children + 256,
			    [](const Node* n) { return bool(n); });
	  const uint16_t numValues = values.size();
	  size_t s = alignSize_(
	                 alignSize_(2 + numValues, CHILD_OFFSET_ALIGNMENT) +
			     CHILD_ARRAY_SIZE + numValues * sizeof(Value),
	                 NODE_ALIGNMENT
                     );
	  for (const Node** p = children; p < children + 256; ++p) {
	    if (*p) {
	      s += (*p)->computeTreeDataSize();
	    }
	  }
	  return s;
	}

	static size_t writeTreeData(
	     uint32_t baseOffset,
	     const ByteTrieBuilderNode<Value, BaseAllocator>** children,
	     const ByteTrieBuilderSparseArray<Value>& values,
	     const std::function<void (const void*, size_t)>& writeData
	) {
	  typedef ByteTrieBuilderNode<Value, BaseAllocator> Node;
	  const uint16_t numChildren =
	    std::count_if(children, children + 256,
			  [](const Node* n) { return bool(n); });
	  const uint16_t numValues = values.size();
	  const uint16_t HEADER_SIZE =
	      this->alignSize_(2 + numValues, CHILD_OFFSET_ALIGNMENT) +
	      CHILD_ARRAY_SIZE;
	  std::unique_ptr<uint8_t[]> headerBuffer(new uint8_t[HEADER_SIZE]);
	  uint8_t* header = headerBuffer.get_();
	  uint32_t* const childOffsets =
	    (uint32_t*)this->alignTo_(header + 2 + numValues,
				      CHILD_OFFSET_ALIGNMENT);
	  size_t numWritten = 0;

	  assert((baseOffset & (NODE_ALIGNMENT - 1)) == 0);

	  // Write the value and child counts
	  header[0] = numValues - 1;
	  header[1] = numChildren - 1;

	  // Write the value keys
	  for (uint16_t i = 0; i < numValues; ++i) {
	    header[i + 2] = values[i].value;
	  }

	  // Write the children
	  size_t childOffset =
	    alignSize_(baseOffset + HEADER_SIZE + sizeof(Value) * numValues,
		       NODE_ALIGNMENT);
	  for (uint16_t i = 0; i < 256; ++i) {
	    if (!children[i]) {
	      childOffsets[i] = 0;
	    } else {
	      assert(childOffset <= MAX_TRIE_SIZE);
	      assert((childOffset & TYPE_MASK) == 0);
	      childOffsets[i] = childOffset | children[i]->serializedTypeCode();
	      childOffset += children[i]->computeTreeDataSize();
	    }
	  }
	  assert(childOffset <= MAX_TRIE_SIZE);
	  assert((childOffset & TYPE_MASK) == 0);

	  writeData(header, HEADER_SIZE);
	  numWritten = HEADER_SIZE;

	  // Write the values
	  for (auto p = values.begin(); p != values.end(); ++p) {
	    writeData(&(p->value), sizeof(Value));
	  }
	  numWritten += numValues * sizeof(Value);

	  // Write padding to align to node boundary
	  numWritten += writePad_(writeData, numWritten, NODE_ALIGNMENT);
	  assert(numWritten <= MAX_TRIE_SIZE);

	  // Now write the children, verifying offset consistency as we go
	  for (uint16_t i = 0; i < 256; ++i) {
	    if (children[i]) {
	      const size_t base = baseOffset + numWritten;
	      assert(base <= MAX_TRIE_SIZE);
	      assert(base == (childOffsets[i] & ~TYPE_MASK));
	      numWritten += children[i]->write(base, writeData);
	    }
	  }
	  
	  assert((baseOffset + numWritten) == childOffset);
	  return numWritten;
	}
	
      private:
	const uint8_t* valueKeyPtr_() const { return this->start_ + 2; }
	
	uint32_t childDataOffset_() {
	  return alignSize_(2 + numValues(), CHILD_OFFSET_ALIGNMENT);
	}

	const uint32_t* childDataPtr_() const {
	  return (const uint32_t*)(this->start_ + childDataOffset_());
	}

	uint32_t valueDataOffset_() const {
	  return childDataOffset_() + CHILD_ARRAY_SIZE;
	}

	const Value* valueDataPtr_() const {
	  return (const Value*)(this->start_ + valueDataOffset_());
	}

	uint16_t firstChildKey_(uint8_t start = 0) const {
	  // Because children are dense, this loop should stop quickly
	  uint16_t i;
	  for (i = start; (i < 256) && !childDataPtr_()[i]; ++i) {
	  }
	  return i;
	}

	uint16_t choosePosition_(uint16_t childIndex,
				 uint16_t valueIndex) const {
	  // Remember that in a dense node, the child index also doubles as
	  // the key at that index
	  if (valueIndex < numValues()) {
	    const uint8_t* const valueKeys = valueKeyPtr_();
	    if ((childIndex >= 256) || (valueKeys[valueIndex] <= childIndex)) {
	      return Position(valueIndex, Position::AT_VALUE);
	    } else {
	      return Position(childIndex, Position::AT_CHILD);
	    }
	  } else if (childIndex < 256) {
	    return Position(childIndex, Position::AT_CHILD);
	  } else {
	    return end();
	  }
	}
      };
      
    }
  }
}
#endif

