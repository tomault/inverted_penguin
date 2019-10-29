#ifndef __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERNODE_HPP__
#define __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERNODE_HPP__

#include <inverted_penguin/core/Optional.hpp>
#include <inverted_penguin/core/detail/ByteTrieNodeType.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderTriePosition.hpp>
#include <function>
#include <ostream>
#include <tuple>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderAllocators;

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderNode {
      public:
	typedef ByteTrieBuilderAllocators<Value, BaseAllocator> Allocators;
	typedef ByteTrieBuilderNode<Value, BaseAllocator> Node;
	typedef ByteTrieBuilderTriePosition<Value, BaseAllocator> Position;
	
      public:
	virtual ~ByteTrieBuilderNode() { }

	/** @brief Type of this node */
	virtual uint32_t nodeType() const = 0;

	/** @brief Number of children */
	virtual uint16_t numChildren() const = 0;

	/** @brief Number of values */
	virtual uint16_t numValues() const = 0;

	/** @brief First position in this node */
	virtual Position begin() const = 0;

	/** @brief End of all positions in this node */
	virtual Position end() const { return Position(); }

	/** @brief Next position after @c p */
	virtual Position next(Position p) const = 0;

	/** @brief Write the key at @c p to @c out */
	virtual void writeKey(std::ostream& out, Position p) const = 0;

	virtual const Value& valueAt(uint8_t index) const = 0;
	virtual void setValueAt(uint8_t index, const Value& v) = 0;
	virtual void setValueAt(uint8_t index, Value&& v) = 0;
	
	/** @brief Compute the number of bytes needed to store the subtree
	 *         rooted at this node in the constructed ByteTrie
	 */
	virtual size_t computeTreeDataSize() const = 0;

	/** @brief Write the ByteTrie representation of the subtree rooted
	 *         at this node
	 */
	virtual size_t write(
	    uint32_t baseOffset,
	    const std::function<void (const void*, size_t)>& writeData
	) const = 0;

	/** @brief Return the type code for the ByteTrieNode this node
	 *         will be serialized to.
	 */
	virtual uint32_t serializedTypeCode() const = 0;

	/** @brief Free all resources used by the subtree rooted at this
	 *         node.
	 *
	 *  To avoid storing the allocators at each node, allocators are
	 *  stored in one place and passed in when needed.  However, there
	 *  is no way to pass the allocators to the destructor, so the
	 *  release of memory (and other resources) must occur outside
	 *  the destructor.  It is the responsibility of the node's owner
	 *  to call destroy() on each node it owns to free up resources.
	 *
	 *  After destroy() is called, the node will have no children or
	 *  values.
	 */
	virtual void destroy(Allocators& allocators) = 0;
      };
      
    }
  }
}


#endif
