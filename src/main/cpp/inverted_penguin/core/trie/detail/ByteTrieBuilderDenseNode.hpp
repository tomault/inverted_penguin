#ifndef __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERDENSENODE_HPP__
#define __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERDENSENODE_HPP__

#include <inverted_penguin/core/detail/ByteTrieBuilderInnerNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderSparseArray.hpp>
#include <inverted_penguin/core/detail/ByteTrieDenseNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieSparseNode.hpp>
#include <pistis/exceptions/IllegalValueError.hpp>

#include <algorithm>
#include <functional>
#include <iomanip>
#include <memory>
#include <ostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderDenseNode :
	  public ByteTrieBuilderInnerNode<Value, Allocators> {
      public:
	typedef ByteTrieBuilderInnerNode<Value, BaseAllocator> ParentType;
	using typename ParentType::Allocators;
	using typename ParentType::Node;
	using typename ParentType::Position;
	using typename ParentType::ValueArray;

	typedef ByteTrieBuilderSparseArray<Node*> NodeArray;

	static constexpr const uint32_t NODE_TYPE =
	    ByteTrieDenseNode<Value>::NODE_TYPE;

      public:
	ByteTrieBuilderDenseNode(): ParentType(), children_(new Node*[256]) {
	  std::fill(children_.get(), children_.get() + 256, nullptr);
	}
	
	ByteTrieBuilderDenseNode(NodeArray&& children, ValueArray&& values):
	    ParentType(std::move(values)), children_(new Node*[256]) {
	  std::fill(children_.get(), children_.get() + 256, nullptr);
	  for (uint16_t i = 0; i < children.size(); ++i) {
	    children_[children.keyAt(i)] = children.valueAt(i);
	  }
	  children.clear();
	}
	
	ByteTrieBuilderDenseNode(const ByteTrieBuilderDenseNode&) = delete;
	virtual ~ByteTrieBuilderDenseNode() { }

	virtual uint32_t nodeType() const override { return NODE_TYPE; }

	virtual uint16_t numChildren() const override {
	  return std::count_if(children_.get(), children_.get() + 256,
			       [](Node* p) { return (bool)p; });
	}

	virtual Position begin() const override {
	  uint16_t i = 0;
	  for (; (i < 256) && !children_[i]; ++i) {
	    // Left blank
	  }

	  if (this->values_.size() && (values_.keyAt(0) <= i)) {
	    return Position(0, Position::AT_VALUE);
	  } else if (i < 256) {
	    return Position(i, Position::AT_CHILD);
	  } else {
	    return Position(); // no children and no values
	  }
	}

	virtual Position next(Position p) const override {
	  uint16_t childIndex, valueIndex;
	  if (p.type() == Position::AT_CHILD) {
	    valueIndex = this->values_.after(children_.keyAt(p.index()));
	    childIndex = p.index() + 1;
	  } else {
	    valueIndex = p.index() + 1;
	    childIndex = this->values_.keyAt(p.index()) + 1;
	  }

	  for(; (childIndex < 256) && !children_[childIndex]; ++childIndex) {
	  }

	  return choosePosition_(childIndex, valueIndex);
	}

	virtual Position after(Position p) const override {
	  uint16_t childIndex, valueIndex;
	  if (p.type() == Position::AT_CHILD) {
	    valueIndex = this->values_.after(children_.keyAt(p.index()));
	    childIndex = p.index() + 1;
	  } else {
	    valueIndex = p.index() + 1;
	    childIndex = this->values_.keyAt(p.index()) + 1;
	  }

	  for(; (childIndex < 256) && !children_[childIndex]; ++childIndex) {
	  }

	  return choosePosition_(childIndex, valueIndex);
	}

	virtual void writeKey(std::ostream& out, Position p) const override {
	  out << ((p.type() == Position::AT_CHILD) ?(char)p.index()
		                                   : this->keyAt(p.index());
	}

	virtual Node* childAt(uint8_t index) const { return children_[index]; }

	virtual Position findChild(uint8_t key) const {
	  return children_[key] ? Position(key, Position::AT_CHILD)
	                        : Position();
	}

	virtual Node* addChild(uint8_t key, Node* child,
			       Allocators& allocators) {
	  if (children_[key]) {
	    ostringstream msg;
	    msg << "Already have a child at key 0x" << std::hex << std::setw(2)
		<< std::setfill('0') << key;
	    throw pistis::exceptions::IllegalValueError(msg.str(),
							PISTIS_EX_HERE);
	  }
	  children_[key] = child;
	  return this;
	}

	virtual void replaceChildAt(uint8_t index, Node* newChild,
				    Allocators& allocators) {
	  if (!children_[index]) {
	    std::ostringstream msg;
	    msg << "No child at key 0x" << std::hex << std::setw(2)
		<< std::setfill('0') << key;
	    throw pistis::exceptions::IllegalValueError(msg.str(),
							PISTIS_EX_HERE);
	  }
	  allocators.destroyNode(children_[index]);
	  children_[index] = newChild;
	}
	
	virtual size_t computeTreeDataSize() const override {
	  const uint16_t n = this->numChildren();
	  if (n >= ByteTrieDenseNode<Value, Allocators>::MIN_CHILDREN) {
	    return ByteTrieDenseNode<Value, Allocators>::computeTreeDataSize(
	        this->children_.get(), this->values_
	    );
	  } else {
	    NodeArray children = makeChildArray_();
	    return ByteTrieSparseNode<Value, Allocators>::computeTreeDataSize(
	        children, this->values_
	    );
	  }
	}
	
	virtual size_t write(
	    uint32_t baseOffset,
	    const std::function<void (const void*, size_t)>& writeData
	) const override {
	  const uint16_t n = this->numChildren();
	  if (n >= ByteTrieDenseNode<Value, Allocators>::MIN_CHILDREN) {
	    return ByteTrieDenseNode<Value, Allocators>::writeTreeData(
		baseOffset, children_, this->values_, writeData
	    );
	  } else {
	    NodeArray children = makeChildArray_();
	    return ByteTrieSparseNode<Value, Allocators>::writeTreeData(
		baseOffset, children, this->values_, writeData
	    );
	  }
	}

	virtual uint32_t serializedTypeCode() const {
	  const bool isDense =
	    this->numChildren() >= ByteTrieDenseNode<Value>::MIN_CHILDREN;
	  return isDense ? ByteTrieDenseNode<Value>::NODE_TYPE
	                 : ByteTrieSparseNode::NODE_TYPE;
	}

	virtual void destroy(Allocators& allocators) override {
	  for (auto p = children_.get(); p < children_.get() + 256; ++p) {
	    allocators.destroyNode(*p);
	  }
	  children_.reset();
	}

	ByteTrieBuilderDenseNode& operator=(const ByteTrieBuilderDenseNode&)
	    = delete;

      private:
	std::unique_ptr<Node*[]> children_;

	NodeArray makeChildArray_() const {
	  NodeArray children;
	  for (Node* p = children_.get(); p < children_.get() + 256; ++p) {
	    if (*p) {
	      children.add(p - children_.get(), *p);
	    }
	  }
	  return std::move(children);
	}
      };
      
    }
  }
}
#endif

