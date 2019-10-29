#ifndef __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERINNERNODE_HPP__
#define __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERINNERNODE_HPP__

#include <inverted_penguin/core/detail/ByteTrieBuilderNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderSparseArray.hpp>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename Allocator>
      class ByteTrieBuilderInnerNode :
	  public ByteTrieBuilderNode<Value, Allocator> {
      public:
	typedef ByteTrieBuilderNode<Value, Allocator> ParentType;
	using typename ParentType::Allocators;
	using typename ParentType::Node;
	using typename ParentType::Position;

	typedef ByteTrieBuilderSparseArray<Value> ValueArray;
	
      public:
	ByteTrieBuilderInnerNode(const ByteTrieBuilderInnerNode&) = delete;
	virtual ~ByteTrieBuilderInnerNode() { }

	virtual uint16_t numValues() const override { return values_.size(); }

	virtual Position after(Position p) const = 0;
	
	Position findKey(uint8_t key) const {
	  const int16_t i = values_.find(key);
	  if (i >= 0) {
	    return Position(i, Position::AT_VALUE);
	  } else {
	    return findChild(key);
	  }
	}
	
	virtual Node* childAt(uint8_t index) const = 0;
	virtual Position findChild(uint8_t key) const = 0;
	virtual Node* addChild(uint8_t key, Node* child,
			       Allocators& allocators) = 0;
	virtual void replaceChildAt(uint8_t index, Node* newChild,
				    Allocators& allocators) = 0;
	
	virtual const Value& valueAt(uint8_t index) const override {
	  return values_.valueAt(index);
	}

	virtual void setValueAt(uint8_t index, const Value& v) const override {
	  values_.setValueAt(index, v);
	}

	virtual void setValueAt(uint8_t index, Value&& v) const override {
	  values_.setValueAt(index, std::move(v));
	}

	Position findValue(uint8_t key) const {
	  int16_t i = values_.find(key);
	  if (i >= 0) {
	    return Position((uint8_t)i, Position::AT_VALUE);
	  } else {
	    return this->end();
	  }
	}

	Position addValue(uint8_t key, const Value& v) {
	  return Position(values_.add(key, v), Position::AT_VALUE);
	}


	Position addValue(uint8_t key, Value&& v) {
	  return Position(values_.add(key, std::move(v)), Position::AT_VALUE);
	}

      protected:
	ValueArray values_;
	
	ByteTrieBuilderInnerNode(): values_() { }
	ByteTrieBuilderInnerNode(ValueArray&& values):
	    values_(std::move(values)) {
	}
      };
      
    }
  }
}

#endif
