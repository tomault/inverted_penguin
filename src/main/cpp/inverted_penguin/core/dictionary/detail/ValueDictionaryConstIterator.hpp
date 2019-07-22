#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYCONSTITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYCONSTITERATOR_HPP__

#include <inverted_penguin/core/dictionary/detail/ValueDictionaryBaseIterator.hpp>
#include <inverted_penguin/core/dictionary/detail/ValueDictionaryKey.hpp>
#include <utility>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {

	template <typename Value>
	class ValueDictionaryConstIterator :
	    public ValueDictionaryBaseIterator<
	        Value, ValueDictionaryConstIterator<Value>
	    > {
	protected:
	  typedef ValueDictionaryBaseIterator<
	     Value, ValueDictionaryConstIterator<Value>
	  > ParentType;
	  using typename ParentType::DictionaryType;

	public:
	  struct KeyValue {
	    ValueDictionaryKey key;
	    const Value& value;

	    KeyValue(const ValueDictionaryKey& k, const Value& v):
	        key(k), value(v) {
	    }
	  };

	  typedef KeyValue value_type;
	  typedef value_type reference;
	  
	public:
	  ValueDictionaryConstIterator(): ParentType(nullptr, 0) { }
	  
	  ValueDictionaryConstIterator(const DictionaryType* dict,
				       uint32_t index):
	      ParentType(dict, index) {
	  }
	  
	  ValueDictionaryConstIterator(const ValueDictionaryConstIterator&) =
	      default;
	  
	  ValueDictionaryConstIterator(const ParentType& other):
	      ParentType(other) {
	  }

	  reference operator*() const {
	    return KeyValue(this->dict_->keyAt(this->index_),
			    this->dict_->valueAt(this->index_));
	  }

	  ValueDictionaryConstIterator& operator=(
	      const ValueDictionaryConstIterator&
	  ) = default;
	  
	  ValueDictionaryConstIterator& operator=(const ParentType& other) {
	    ParentType::operator=(other);
	    return *this;
	  }
	};
	
      }
    }
  }
}
#endif

