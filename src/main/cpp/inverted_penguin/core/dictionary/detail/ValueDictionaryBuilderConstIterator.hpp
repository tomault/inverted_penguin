#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYBUILDERCONSTITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYBUILDERCONSTITERATOR_HPP__

#include <inverted_penguin/core/dictionary/detail/ValueDictionaryBuilderBaseIterator.hpp>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {

	template <typename Value, typename Builder>
	class ValueDictionaryBuilderConstIterator :
	    public ValueDictionaryBuilderBaseIterator<
	        Builder, ValueDictionaryBuilderConstIterator<Value, Builder>
	    > {
	private:
	  typedef ValueDictionaryBuilderBaseIterator<
	      Builder, ValueDictionaryBuilderConstIterator<Value, Builder>
	  > ParentType;
	  
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
	  ValueDictionaryBuilderConstIterator():
	      ValueDictionaryBuilderConstIterator(nullptr, 0) {
	  }
	  
	  ValueDictionaryBuilderConstIterator(Builder* builder, uint32_t index):
	      ParentType(builder, index) {
	  }
	  
	  ValueDictionaryBuilderConstIterator(
	      const ValueDictionaryBuilderConstIterator&
	  ) = default;

	  template <typename OtherIterator>
	  ValueDictionaryBuilderConstIterator(
	       const ValueDictionaryBuilderBaseIterator<
	           Builder, OtherIterator
	       >& other):
	      ParentType(other) {
	  }

	  reference operator*() const {
	    return KeyValue(this->builder_->keyAt(this->index_),
			    this->builder_->valueAt(this->index_));
	  }

	  ValueDictionaryBuilderConstIterator& operator=(
	      const ValueDictionaryBuilderConstIterator&
	  ) = default;

	  template <typename OtherIterator>
	  ValueDictionaryBuilderConstIterator& operator=(
	      const ValueDictionaryBuilderBaseIterator<
	          Builder, OtherIterator
	      >& other) {
	    ParentType::operator=(other);
	    return *this;
	  }
	  
	};
	
      }
    }
  }
}

#endif

