#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIESPARSENODE_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIESPARSENODE_HPP__

#include <inverted_penguin/core/detail/ByteTrieNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderKeyBuilder.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderSparseArray.hpp>
#include <algorithm>
#include <functional>
#include <memory>
#include <tuple>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value>
      class ByteTrieSparseNode : public ByteTrieNode<Value> {
      public:
	typedef ByteTrieNode<Value> ParentType;
	using typename ParentType::Position;
	using typename ParentType::Location;
	using ParentType::NODE_ALIGNMENT;
	using ParentType::VALUE_ALIGNMENT;
	using ParentType::MAX_TRIE_SIZE;
	using ParentType::TYPE_MASK;

	using ParentType::end;
	using ParentType::pad_;
	using ParentType::searchForKey_;
	using ParentType::searchForKeyAfter_;
	using ParentType::writePad_;

	static const uint32_t NODE_TYPE = 1;

      private:
	static constexpr const uint32_t CHILD_OFFSET_SIZE = sizeof(uint32_t);
	static constexpr const uint32_t CHILD_OFFSET_ALIGNMENT =
	    alignof(uint32_t);
	
      public:
	ByteTrieSparseNode(const uint8_t* base, uint32_t nodeOffset):
	    ByteTrieNode(base, nodeOffset) {
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
	    if (!numChildren() || (valueKeyPtr_()[0] <= childKeyPtr_()[0])) {
	      return Position(0, Position::AT_VALUE);
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
	  const uint8_t* const childKeys = childKeyPtr_();
	  const uint8_t* const valueKeys = valueKeyPtr_();
	  int16_t valueIndex, childIndex;
	  if (p.type() == Position::AT_VALUE) {
	    valueIndex = (int16_t)p.index() + 1;
	    childIndex = searchForKey_(childKeys, numChildren(),
				       valueKeys[p.index()]);
	    if (childIndex >= 0) {
	      return Position(childIndex, Position::AT_CHILD);
	    } else {
	      childIndex = -childIndex - 1;
	    }
	  } else {
	    childIndex = (int16_t)p.index() + 1;
	    valueIndex = searchForKeyAfter_(valueKeys, numValues(),
					    childKeys[p.index()]);
	  }

	  return choosePosition_(childIndex, valueIndex);
	}
	
	Position after(Position p) const {
	  const uint8_t* const childKeys = childKeyPtr_();
	  const uint8_t* const valueKeys = valueKeyPtr_();
	  int16_t valueIndex, childIndex;
	  if (p.type() == Position::AT_VALUE) {
	    valueIndex = (int16_t)p.index() + 1;
	    childIndex = searchForKeyAfter_(childKeys, numChildren(),
					    valueKeys[p.index()]);
	  } else {
	    childIndex = (int16_t)p.index() + 1;
	    valueIndex = searchForKeyAfter_(valueKeys, numValues(),
					    childKeys[p.index()]);
	  }

	  return choosePosition_(childIndex, valueIndex);
	}

	Position findKey(uint8_t key) const {
	  int16_t p = searchForKey_(valueKeyPtr_(), numValues(), key);
	  if (p >= 0) {
	    return Position((uint8_t)p, Position::AT_VALUE);
	  } else {
	    p = searchForKey_(childKeyPtr_(), numChildren(), key);
	    return (p >= 0) ? Position((uint8_t)p, Position::AT_CHILD)
	                    : end();
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
	  int16_t p = searchForKey_(childKeyPtr_(), numChildren(), key);
	  return (p >= 0) ? Position((uint8_t)p, Position::AT_CHILD)
	                  : end();
	}
	
	template <typename Function>
	void forEachChild(Function f) const {
	  const uint32_t* const children = childDataPtr_();
	  const uint32_t* const end = children + numChildren();
	  for (auto p = children; p != end; ++p) {
	    f(this->base_, *p);
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
	static size_t computeTrieDataSize(
	    const ByteTrieBuilderSparseArray<
	        ByteTrieBuilderNode<Value, BaseAllocator>*
	    > children,
	    const ByteTrieBuilderSparseArray<Value, BaseAllocator>& values
	) {
	  const uint16_t numChildren = children.size();
	  const uint16_t numValues = values.size();
	  const size_t padding =
	      pad_(4 + numValues + numChildren, CHILD_OFFSET_ALIGNMENT) +
	      pad_(CHILD_OFFSET_SIZE * numChildren, VALUE_ALIGNMENT);	  
	  // Violated only if Value has absurd alignment requirements
	  assert(padding < 256);

	  size_t s = alignSize_(4 + numValues + numChildren + padding +
				  CHILD_OFFSET_SIZE * numChildren +
				  sizeof(Value) * numValues,
				NODE_ALIGNMENT);
	  for (const auto& kv : children) {
	    s += kv.value->computeTrieDataSize();
	  }
	  return s;
	}

	template <typename BaseAllocator>
	static size_t writeTrieData(
	    uint32_t baseOffset,
	    const ByteTrieBuilderSparseArray<
	        ByteTrieBuilderNode<Value, BaseAllocator>*
	    > children,
	    const ByteTrieBuilderSparseArray<Value, BaseAllocator>& values,
	    const std::function<void (const void*, size_t)>& writeData
	) {
	  const uint16_t numChildren = children.size();
	  const uint16_t numValues = values.size();
	  // Padding between end of keys and start of values to align
	  // offsets on 4 byte boundaries and values on alignof(Value)
	  // boundaries
	  const size_t padding =
	      pad_(4 + numValues + numChildren, CHILD_OFFSET_ALIGNMENT) +
	      pad_(CHILD_OFFSET_SIZE * numChildren, VALUE_ALIGNMENT);
	  // Violated only if Value has absurd alignment requirements
	  assert(padding < 256);

	  const size_t HEADER_SIZE =
	      4 + values.size() + children.size() + padding +
	      CHILD_OFFSET_SIZE * numChildren;
	  std::unique_ptr<uint8_t[]> headerData(new uint8_t[HEADER_SIZE]);
	  uint8_t* const header = headerData.get();
	  uint32_t* const children =
	      (uint32_t*)(header + 4 + numValues + numChildren + padding)
	  size_t numWritten = HEADER_SIZE;

	  header[0] = numValues - 1;
	  header[1] = numChildren - 1;
	  header[2] = padding;

	  // Write keys
	  uint32_t i = 4;
	  for (const auto& kv : values) {
	    header[i] = kv.key;
	    ++i;
	  }
	  for (const auto& kv : children) {
	    header[i] = kv.key;
	    ++i;
	  }
	  assert(header + i + padding == (uint8_t*)children);
	  std::fill(header + i, header + i + padding, (uint8_t)0);

	  // Fill in child offsets.  Children will appear immediately
	  // after this node.
	  size_t childOffset =
	      alignSize_(baseOffset + HEADER_SIZE + sizeof(Value) * numValues,
			 NODE_ALIGNMENT);
	  i = 0;
	  for (const auto& kv : children) {
	    assert((childOffset & TYPE_MASK) == 0);
	    assert(childOffset <= MAX_TRIE_SIZE);
	    children[i] = childOffset | kv.value->serializedTypeCode();
	    childOffset += kv.value->computeTrieDataSize();
	    ++i;
	  }
	  assert(childOffset <= MAX_TRIE_SIZE);
	  writeData(header, HEADER_SIZE);

	  // Write data
	  for (const auto& kv : values) {
	    writeData(&(kv.value), sizeof(Value));
	  }
	  numWritten += sizeof(Value) * numValues;

	  // Pad this node so that the next node will begin on a 4-byte
	  // boundary
	  numWritten += this->writePad_(writeData, numWritten,
					NODE_ALIGNMENT);

	  // Now the children themselves
	  i = 0;
	  for (const auto& kv : children) {
	    // Verify integrity of child offsets
	    size_t base = baseOffset + numWritten;
	    assert(base < MAX_TRIE_SIZE)
	    assert(base == (children[i] & ~TYPE_MASK));
	    numWritten += kv.value->write(base, writeData);
	    ++i;
	  }

	  assert((baseOffset + numWritten) == childOffset);
	  return numWritten;
	}

      private:
	const uint8_t* valueKeyPtr_() const { return this->start_ + 4; }

	const uint8_t* childKeyPtr_() const {
	  return this->start_ + 4 + numValues();
	}
	uint32_t childDataOffset_() const {
	  return 4 + numValues() + numChildren() + this->start_[2];
	}

	const uint32_t* childDataPtr_() const {
	  return (const uint32_t*)(this->start_ + chilDataOffset_());
	}
	
	uint32_t valueDataOffset_() const {
	  return childDataOffset_() + CHILD_OFFSET_SIZE * numChildren();
	}

	const Value* valueDataPtr_() const {
	  return (const Value*)(this->start_ + valueDataOffset_());
	}

	Position choosePosition_(int16_t childIndex, int16_t valueIndex) const {
	  if (valueIndex < numValues()) {
	    const uint8_t* const childKeys = childKeyPtr_();
	    const uint8_t* const valueKeys = valueKeyPtr_();
	    if ((childIndex >= numChildren()) ||
		(valueKeys[valueIndex] < childKeys[childIndex])) {
	      return Position(valueIndex, Position::AT_VALUE);
	    } else {
	      return Position(childIndex, Position::AT_CHILD);
	    }
	  } else if (childIndex < numChildren()) {
	    return Position(childIndex, Position::AT_CHILD_);
	  } else {
	    return end();
	  }
	}
      };
      
    }
  }
}

#endif

