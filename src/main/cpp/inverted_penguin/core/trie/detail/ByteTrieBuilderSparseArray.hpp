#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERSPARSEARRAY_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERSPARSEARRAY_HPP__

#include <pistis/exceptions/IllegalValueError.hpp>
#include <vector>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value>
      class ByteTrieBuilderSparseArray {
      public:
	struct KeyValue {
	  uint8_t key;
	  Value value;

	  KeyValue(uint8_t k, const Value& v): key(k), value(v) { }
	  KeyValue(uint8_t k, Value&& v): key(k), value(std::move(v)) {}
	};

	typedef std::vector<KeyValue>::const_iterator ConstIterator;
	typedef ConstIterator Iterator;
	
      public:
	ByteTrieBuilderSparseArray(): values_() { }
	ByteTrieBuilderSparseArray(uint8_t key, const Value& v):
	    values_{ KeyValue(k, v) } {
	}
	ByteTrieBuilderSparseArray(uint8_t key, Value&& v):
	    values_{ KeyValue(k, std::move(v)) } {
	}
	ByteTrieBuilderSparseArray(const ByteTrieBuilderSparseArray&) =
            default;
	ByteTrieBuilderSparseArray(ByteTrieBuilderSparseArray&&) = default;

	uint16_t size() const { return values_.size(); }
	ConstIterator begin() const { return values_.begin(); }
	ConstIterator end() const { return values_.end(); }
	uint8_t keyAt(uint8_t i) const { return values_[i].key; }
	const Value& valueAt(uint8_t i) const { return values_[i].value; }
	void setValueAt(uint8_t i, const Value& v) {
	  values_[i].value = v;
	}
	void setValueAt(uint8_t i, Value&& v) {
	  values_[i].value = std::move(v);
	}

	int16_t find(uint8_t key) const {
	  int16_t i = 0;
	  int16_t j = size();

	  while (i < j) {
	    int16_t k = (i + j) >> 1; // Safe because i, j in [0, 256]
	    if (values_[k].key < key) {
	      i = k + 1;
	    } else if (key < values_[k].key) {
	      j = k;
	    } else {
	      return k;
	    }
	  }
	  return -i - 1;
	}

	uint16_t after(uint8_t key) const {
	  uint16_t i = 0;
	  uint16_t j = size();

	  while (i < j) {
	    uint16_t k = (i + j) >> 1; // Safe because i, j in [0, 256]
	    if (values_[k].key <= key) {
	      i = k + 1;
	    } else {
	      j = k;
	    }
	  }
	  return i;
	}

	uint8_t add(uint8_t key, const Value& v) {
	  int16_t i = find(key);
	  if (i >= 0) {
	    setValueAt((uint8_t)i, v);
	  } else {
	    i = -i - 1;
	    value_.insert(values_.begin() + i, v);
	  }
	  return (uint8_t)i;
	}

	uint8_t add(uint8_t key, Value&& v) {
	  int16_t i = find(key);
	  if (i >= 0) {
	    setValueAt((uint8_t)i, std::move(v));
	  } else {
	    i = -i - 1;
	    value_.insert(values_.begin() + i, std::move(v));
	  }
	  return (uint8_t)i;
	}

	uint8_t addExclusive(uint8_t key, const Value& v) {
	  int16_t i = find(key);
	  if (i >= 0) {
	    std::ostringstream msg;
	    msg << "Already have a value for key 0x" << std::hex << std::setw(2)
		<< std::setfill('0') << i;
	    throw pistis::exceptions::IllegalValueError(msg.str(),
							PISTIS_EX_HERE);
	  } else {
	    i = -i - 1;
	    value_.insert(values_.begin() + i, v);
	    return (uint8_t)i;
	  }
	}
	
	uint8_t addExclusive(uint8_t key, Value&& v) {
	  int16_t i = find(key);
	  if (i >= 0) {
	    std::ostringstream msg;
	    msg << "Already have a value for key 0x" << std::hex << std::setw(2)
		<< std::setfill('0') << i;
	    throw pistis::exceptions::IllegalValueError(msg.str(),
							PISTIS_EX_HERE);
	  } else {
	    i = -i - 1;
	    value_.insert(values_.begin() + i, std::move(v));
	    return (uint8_t)i;
	  }
	}

	void clear() { values_.clear();	}
	
	ByteTrieBuilderSparseArray& operator=(
	    const ByteTrieBuilderSparseArray&
        ) = default;

	ByteTrieBuilderSparseArray& operator=(ByteTrieBuilderSparseArray&&) =
            default;

	const KeyValue& operator[](uint8_t i) const { return values_[i]; }
      private:
	std::vector<KeyValue> values_;
      };
      
    }
  }
}
#endif
