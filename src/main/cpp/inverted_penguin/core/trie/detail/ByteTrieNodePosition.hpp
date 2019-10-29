#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIENODEPOSITION_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIENODEPOSITION_HPP__

#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      class ByteTrieNodePosition {
      public:
	enum class Type : uint8_t {
	  NOWHERE,    ///< Not positioned within a node
	  AT_CHILD,   ///< Positioned at a child within an (inner) node
	  AT_VALUE    ///< Positioned at a value within a node
	};

	static constexpr const Type AT_END = Type::AT_END;
	static constexpr const Type AT_CHILD = Type::AT_CHILD;
	static constexpr const Type AT_VALUE = Type::AT_VALUE;

      public:
	ByteTrieNodePosition(): type_(AT_END), index_(0) { }
	ByteTrieNodePosition(uint8_t index, Type type):
	    type_(type), index_(index) {
	}

	Type type() const { return type; }
	uint8_t index() const { return index; }

	bool operator==(const Position& other) const {
	  return (type() == other.type()) && (index() == other.index());
	}

	bool operator!=(const Position& other) const {
	  return (type() != other.type()) || (index() != other.index());
	}

      private:
	Type type_;
	uint8_t index_;
      };
      
    }
  }
}
#endif


