#ifndef __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERNODE_HPP__
#define __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERNODE_HPP__

#include <inverted_penguin/core/Optional.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderTriePosition.hpp>
#include <function>
#include <tuple>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderAllocators;

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderConstIterator;

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderIterator;

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderNode {
      public:
	typedef ByteTrieBuilderAllocators<Value, BaseAllocator> Allocators;
	typedef ByteTrieBuilderNode<Value, BaseAllocator> Node;
	typedef ByteTrieBuilderTriePosition<Value, BaseAllocator> Position;
	
      public:
	virtual ~ByteTrieBuilderNode();

	virtual uint32_t nodeType() const = 0;

	virtual Position begin() const = 0;
	virtual Position end() const = 0;
	virtual Position next(uint8_t index) const = 0;
	virtual Position find(const uint8_t* key,
			      const uint8_t* keyEnd) const = 0;
	virtual std::tuple<Position, Position> prefix(
	    const uint8_t* key, const uint8_t* keyEnd
	) const = 0;

	virtual Value valueAt(uint8_t index) const = 0;
	virtual void setValueAt(uint8_t index, const Value& v) = 0;
	virtual void setValueAt(uint8_t index, Value&& v) = 0;

	virtual bool has(const uint8_t* key, const uint8_t* keyEnd) const = 0;
	virtual Optional<Value> get(const uint8_t* key,
				    const uint8_t* keyEnd) const = 0;

	virtual std::unique_ptr< Node, std::function<void (Node*)> > add(
	    const uint8_t* key, const uint8_t* keyEnd, const Value& value,
	    Allocators& allocators
	) = 0;

	virtual uint32_t computeTreeDataSize() const = 0;
	
	virtual uint32_t write(
	    const std::function<void (const void*, size_t)>& writeData
	) const = 0;

      };
      
    }
  }
}


#endif
