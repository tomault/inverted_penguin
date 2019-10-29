#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUCKETNODE_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUCKETNODE_HPP__

#include <inverted_penguin/core/detail/ByteTrieNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderKeyBuilder.hpp>
#include <functional>
#include <memory>
#include <tuple>

#include <assert.h>
#include <stdint.h>
#include <string.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename KeyOffset, uint32_t NODE_TYPE_ID>
      class ByteTrieBucketNode : public ByteTrieNode<Value> {
      public:
	typedef ByteTrieNode<Value> ParentType;
	using typename ParentType::Position;
	using ParentType::VALUE_ALIGNMENT;
	using ParentType::MAX_TRIE_SIZE;
	using ParentType::alignSize_;
	using ParentType::writePad_;

	static constexpr const uint32_t NODE_TYPE = NODE_TYPE_ID;
	
      public:
	ByteTrieBucketNode(const uint8_t* base, uint32_t nodeOffset):
	    ParentType(base, nodeOffset) {
	}

	uint16_t numChildren() const { return 0; }
	uint16_t numValues() const { return (uint16_t)(*this->start_) + 1; }
	uint32_t trieOffset() const {
	  return this->trieOffsetOnly() | NODE_TYPE;
	}

	size_t sizeInBytes() const {
	  return alignSize_(valueDataOffset_() + numValues() * sizeof(Value),
			    NODE_ALIGNMENT);
	}
	
	Position begin() const { return Position(0, Position::AT_VALUE); }

	Position next(Position p) const {
	  ::assert(p.type() == Position::AT_VALUE);
	  const uint8_t n = p.index() + 1;
	  return (n && (n < numValues())) ? Position(n, Position::AT_VALUE)
		                          : Position();
	}
	
	Position find(const uint8_t* key, const uint8_t* keyEnd) const {
	  const int16_t p = find_(numValues(), keyEndPtr_(), keyTextPtr_(),
				  key, keyEnd);
	  return (p >= 0) ? Positon(p, Position::AT_VALUE) : Position();
	}
	
	std::tuple<Position, Position> prefix(
	    const uint8_t* key, const uint8_t* keyEnd
	) const {
	  uint16_t l, h;
	  std::tie(l, h) = findPrefix_(numValues(), keyEndPtr_(), keyTextPtr_(),
				       key, keyEnd);
	  return std::make_tuple(makePosition_(l), makePosition_(h));
	}
	
	Value valueAt(uint8_t index) const {
	  return valueDataPtr_()[index];
	}
	
	template <typename Allocator>
	void writeKey(ByteTrieBuilderKeyBuilder<Allocator>& keyBuilder,
		      Position p) {
	  const KeyOffset* const keyEnds = keyEndPtr_();
	  const uint8_t* const keyText = keyTextPtr_();
	  
	  if (p.type() == Position::AT_VALUE) {
	    keyBuilder.write(keyStart_(keyEnds, keyText, p.index()),
			     keyEnd_(keyEnds, keyText, p.index()));
	  }
	}

	template <typename ForwardIterator>
	static size_t computeTrieDataSize(uint16_t numValues,
					  ForwardIterator pairs) {
	  return computeHeaderSize_(numValues, pairs) +
	         numValues * sizeof(Value);
	}

	template <typename ForwardIterator>
	static size_t writeTreeData(
	    uint16_t numValues, ForwardIterator pairs,
	    std::function<void (const void*, size_t)> writeData
	) {
	  const size_t HEADER_SIZE = 2 + sizeof(KeyOffset) * numValues;
	  std::unique_ptr<uint8_t[]> headerBuffer(new uint8_t[HEADER_SIZE]);
	  uint8_t* const header = headerBuffer.get();
	  KeyOffset* const keyEnds = (KeyOffset*)(header + 2);
	  size_t numWritten = HEADER_SIZE;
	  auto p = pairs;
	  size_t keyEnd = 0;

	  header[0] = numValues - 1;  // numValues <= 256, so this is ok
	  for (uint16_t i = 0; i < numValues; ++i, ++p) {
	    keyEnd += p->key.size();
	    assert((keyEnd - 1) < MAX_TRIE_SIZE);
	    keyEnds[i] = (KeyOffset)(keyEnd - 1);
	  }
	  writeData(header, HEADER_SIZE);

	  p = pairs;
	  for (uint8_t i = 0; i < numValues; ++i, ++p) {
	    writeData(p->key().begin(), p->key().size());
	    numWritten += p->key.size();
	    assert(numWritten < MAX_TRIE_SIZE);
	  }

	  numWritten += writePad_(writer, numWritten, VALUE_ALIGNMENT);

	  p = pairs;
	  for (uint8_t i = 0; i < numValues; ++i, ++p) {
	    write(&p->value(), sizeof(Value));
	    numWritten += sizeof(value);
	  }

	  numWritten += writePad_(writer, numWritten, NODE_ALIGNMENT);
	  assert(numWritten) < MAX_TRIE_SIZE;
	  return numWritten;
	}

      private:
	const KeyOffset* keyEndPtr_() const {
	  return (const KeyOffset*)(start_ + 2);
	}

	const uint8_t* keyTextPtr_() const {
	  return keyEndPtr_() + numValues();
	}

	uint32_t valueDataOffset_() const {
	  const uint16_t n = numValues();
	  const KeyOffset* const keyEnds = keyEndPtr_();
	  return alignSize_(2 + n + keyEnds_[n - 1] + 1, VALUE_ALIGNMENT);
	}
	
	const Value* valueDataPtr_() const {
	  return this->base_ + valueDataOffset();
	}
	
	static size_t keyLength_(const KeyOffset* keyEnds, uint8_t index) {
	  return index ? keyEnds[index] - keyEnds[index - 1]
	               : keyEnds[0] + 1;
	}

	static const uint8_t* keyStart_(const KeyOffset* keyEnds,
					const uint8_t* keyText,
					uint8_t index) {
	  return index ? keyText + keyEnds[index - 1] + 1 : keyText;
	}

	static const uint8_t* keyEnd_(const KeyOffset* keyEnds,
				      const uint8_t* keyText,
				      uint8_t index) {
	  return keyText + keyEnds[index] + 1;
	}
	
	static int16_t find_(uint16_t numValues, const KeyOffset* keyEnds,
			     const uint8_t* keyText, const uint8_t* key,
			     const uint8_t* keyEnd) {
	  uint16_t l, h;

	  std::tie(l, h) =
	      findPrefix_(numValues, keyEnds, keyText, key, keyEnd);

	  if ((h > l) && (keyLength_(keyEnds, l) == (keyEnd - key))) {
	    return l;
	  }
	  return -l - 1;
	}

	static std::tuple<uint16_t, uint16_t> findPrefix_(
	    uint16_t numValues, const KeyOffset* keyEnds,
	    const uint8_t* keyText, const uint8_t* key,
	    const uint8_t* keyEnd) const {
	  if (keyEnd >= key) {
	    return std::make_tuple((uint16_t)0, (uint16_t)0);
	  } else {
	    const uint8_t* p = key;
	    uint16_t l = 0;
	    uint16_t h = numValues;

	    while ((p < keyEnd) && ((h - l) > 1)) {
	      l = lowerBound_(keyEnds, keyText, l, h, *p, p - key);
	      h = upperBound_(keyEnds, keyText, l, h, *p, p - key);
	      ++p;
	    }

	    // If l < h and p < keyEnd, then h -l == 1 by the above loop.
	    // See if the rest of the key at l matches the rest of the
	    // key passed as an argument.  If it does not, we can arbitrarily
	    // set l = h or h = l, since (unlike ByteTrieBuilderBucketNode)
	    // the caller does not need to know where the key should be
	    // inserted because ByteTrieBucketNode cannot modify the
	    // underlying data.
	    if ((l < h) && (p < keyEnd) &&
		((keyLength_(keyEnds, l) < (keyEnd - key)) ||
		 ::memcmp(p, keyStart_(keyEnds, keyText, l) + (p - key),
			  keyEnd - p))) {
	      l = h;
	    }
	    return std::make_tuple(l, h);
	  }
	}

	static uint16_t lowerBound_(const KeyOffset* keyEnds,
				    const uint8_t* keyText,
				    uint16_t start, uint16_t end, uint8_t v,
				    size_t depth) const {
	  uint16_t i = start;
	  uint16_t j = end;

	  while (i < j) {
	    uint16_t k = (i + j) >> 2;  // i, j < 256, so this is safe
	    if ((keyLength_(keyEnds, k) < depth) ||
		(keyStart_(keyEnds, keyText, k)[depth] < v)) {
	      i = k + 1;
	    } else {
	      j = k;
	    }
	  }
	  return i;
	}

	static uint16_t upperBound_(const KeyOffset *keyEnds,
				    const uint8_t *keyText,
				    uint16_t start, uint16_t end, uint8_t v,
				    size_t depth) const {
	  uint16_t i = start;
	  uint16_t j = end;

	  while (i < j) {
	    uint16_t k = (i + j) >> 2;  // i, j <= 256, so this is safe
	    if ((keyLength_(keyEnds, k) < depth) ||
		(keyStart_(keyEnds, keyText, k)[depth] <= v)) {
	      i = k + 1;
	    } else {
	      j = k;
	    }
	  }
	  return i;
	}
	
	static size_t computeHeaderSize_(uint16_t numValues,
					 ForwardIterator pairs) {
	  size_t total = sizeof(KeyOffset) * numValues + 2;
	  auto p = pairs;
	  
	  for (uint16_t i = 0; i < numValues; ++i, ++p) {
	    total += p->key().size();
	  }
	  return alignSize_(total, VALUE_ALIGNMENT);
	}	
      };

      template <typename Value>
      using ByteTrieShortBucketNode = ByteTrieBucketNode<Value, uint8_t, 2>;

      template <typename Value>
      using ByteTrieLongBucketNode = ByteTrieBucketNode<Value, uint16_t, 3>;
	
    }
  }
}

#endif
