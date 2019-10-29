#ifndef __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERSPARSENODE_HPP__
#define __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERSPARSENODE_HPP__

#include <inverted_penguin/core/detail/ByteTrieBuilderInnerNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderSparseArray.hpp>
#include <inverted_penguin/core/detail/ByteTrieSparseNode.hpp>
#include <pistis/exceptions/IllegalValueError.hpp>

#include <algorithm>
#include <iomanip>
#include <functional>
#include <ostream>
#include <sstream>
#include <vector>

#include <assert.h>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderSparseNode :
	  public ByteTrieBuilderInnerNode<Value, BaseAllocator> {
      public:
	typedef ByteTrieBuilderInnerNode<Value, BaseAllocator> ParentType;
	using typename ParentType::Allocators;
	using typename ParentType::Node;
	using typename ParentType::Position;
	using typename ParentType::ValueArray;

	typedef ByteTrieBuilderSparseArray<Node*> NodeArray;
	
	static constexpr const uint32_t NODE_TYPE =
	    ByteTrieSparseNode<Value>::NODE_TYPE;
	static constexpr const uint8_t MAX_CHILDREN = 128;
	
      public:
	ByteTrieBuilderSparseNode(): ParentType(), children_() {
	}
	
	ByteTrieBuilderSparseNode(NodeArray&& children, ValueArray&& values):
	    ParentType(std::move(values)), children_(std::move(children)) {
	}
	ByteTrieBuilderSparseNode(const ByteTrieBuilderSparseNode&) = delete;
	virtual ~ByteTrieBuilderSparseNode() { }

	virtual uint32_t nodeType() const override { return NODE_TYPE; }

	virtual uint16_t numChildren() const override {
	  return children_.size();
	}
	
	virtual Position begin() const override {
	  if (this->values_.size() && children_.size()) {
	    return (children_.keyAt(0) < this->values_.keyAt(0)) ?
	        Position(0, Position::AT_CHILD);
	        Position(0, Position::AT_VALUE);
	  } else if (this->values_.size()) {
	    return Position(0, Position::AT_VALUE);
	  } else if (children_.size()) {
	    return Position(0, Position::AT_CHILD);
	  } else {
	    return Position();
	  }
	}

	virtual Position next(Position p) const override {
	  int16_t childIndex, valueIndex;
	  if (p.type() == Position::AT_CHILD) {
	    childIndex = (int16_t)p.index() + 1;
	    valueIndex = this->values_.after(children_.keyAt(p.index()));
	  } else {
	    valueIndex = (int16_t)p.index() + 1;
	    childIndex = children_.find(this->values_.keyAt(p.index()));
	    if (childIndex >= 0) {
	      return Position((uint8_t)childIndex, Position::AT_CHILD);
	    } else {
	      childIndex = -childIndex - 1;
	    }
	  }

	  return choosePosition_(childIndex, valueIndex);
	}

	virtual Position after(Position p) const override {
	  int16_t childIndex, valueIndex;
	  if (p.type() == Position::AT_CHILD) {
	    childIndex = (int16_t)p.index() + 1;
	    valueIndex = this->values_.after(children_.keyAt(p.index()));
	  } else {
	    valueIndex = (int16_t)p.index() + 1;
	    childIndex = children_.after(this->values_.keyAt(p.index()));
	  }
	  
	  return choosePosition_(childIndex, valueIndex);
	}

	virtual Node* childAt(uint8_t index) const override {
	  return children_.valueAt(index);
	}

	virtual Position findChild(uint8_t key) const override {
	  const int16_t i = children_.find(key);
	  if (i >= 0) {
	    return Position((uint8_t)i, Position::AT_CHILD);
	  } else {
	    return Position();
	  }
	}

	virtual Node* addChild(uint8_t key, Node* child,
			       Allocators& allocators) const override {
	  int16_t i;
	  try {
	    i = children_.addExclusive(key, child);
	  } catch(const pistis::exception::IllegalValueError& e) {
	    std::ostringstream msg;
	    msg << "Already have a child at key 0x"
		<< std::hex << std::setw(2) << std::setfill('0')
		<< (int16_t)i;
	    throw pistis::exception::IllegalValueError(msg.str(),
						       PISTIS_EX_HERE);
	  }

	  if (children_.size() > MAX_CHILDREN) {
	    return allocators.newDenseNode(std::move(children),
					   std::move(values));
	  } else {
	    return this;
	  }
	}

	virtual void replaceChildAt(uint8_t index, Node* newChild,
				    Allocators& allocators) override {
	  Node* const oldChild = chidren_.valueAt(index);
	  allocators.destroyNode(oldChild);
	  children_.setValueAt(index, newChild);
	}
	
	virtual void writeKey(std::ostream& out, Position p) const override {
	  out << (p.type() == Position::AT_VALUE) ?
	             children_.keyAt(p.index()) :
	             values_.keyAt(p.index());
	}

	
	virtual size_t computeTreeDataSize() const override {
	  return ByteTrieSparseNode<Value>::computeTreeDataSize(
	      children_, values_
	  );
	}
	
	virtual size_t write(
	    uint32_t baseOffset,
	    const std::function<void (const void*, size_t)>& writeData
	) const override {
	  return ByteTrieSparseNode<Value>::writeTreeData(
	      baseOffset, children_, values_, writeData
	  );
	}

	virtual uint32_t serializedTypeCode() const {
	  return ByteTrieSparseNode::NODE_TYPE;
	}

	virtual void destroy(Allocators& allocators) override {
	  for (uint8_t i = 0; i < children_.size(); ++i) {
	    allocators.destroyNode(children_.valueAt(i));
	  }
	}

      private:
	NodeArray children_;

	Position choosePosition_(int16_t childIndex, int16_t valueIndex) const {
	  if (valueIndex < this->values_.size()) {
	    if (childIndex >= children_.size()) {
	      return Position(valueIndex, Position::AT_VALUE);
	    } else {
	      const uint8_t valueKey = this->values_.keyAt(valueIndex);
	      const uint8_t childKey = children_.keyAt(childIndex);
	      return (childKey < valueKey) ?
		  Position(childIndex, Position::AT_CHILD) :
		  Position(valueIndex, Position::AT_VALUE);
	    }
	  } else if (childIndex < children_.size()) {
	    return Position(childIndex, Position::AT_CHILD);
	  } else {
	    return Position();
	  }
	}
	
      };
      
    }
  }
}

#endif
