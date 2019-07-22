#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYBUILDERITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYBUILDERITERATOR_HPP__

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {

	template <typename Value, typename Builder>
	class ValueDictionaryBuilderValueIterator;

	template <typename Value, typename Builder>
	class ValueDictionaryBuilderIterator :
	    public ValueDictionaryBuilderBaseIterator<
	        Builder, ValueDictionaryBuilderIterator<Value, Builder>
	    > {
	private:
	  typedef ValueDictionaryBuilderBaseIterator<
	      Builder, ValueDictionaryBuilderIterator<Value, Builder>
	  > ParentType;

	public:
	  struct KeyValue {
	    ValueDictionaryKey key;
	    Value& value;

	    KeyValue(const ValueDictionaryKey& k, Value& v):
	        key(k), value(v) {
	    }
	  };

	  typedef KeyValue value_type;
	  typedef value_type reference;
	  
	public:
	  ValueDictionaryBuilderIterator():
	      ValueDictionaryBuilderIterator(nullptr, 0) {
	  }
	  
	  ValueDictionaryBuilderIterator(Builder* builder, uint32_t index):
	      ParentType(builder, index) {
	  }
	  
	  ValueDictionaryBuilderIterator(
	      const ValueDictionaryBuilderIterator&
	  ) = default;

	  ValueDictionaryBuilderIterator(
	      const ValueDictionaryBuilderValueIterator<Value, Builder>& other
	  ): ParentType(other) {
	  }

	  reference operator*() const {
	    return KeyValue(this->builder_->keyAt(this->index_),
			    this->builder_->valueAt(this->index_));
	  }

	  ValueDictionaryBuilderIterator& operator=(
	      const ValueDictionaryBuilderIterator&
	  ) = default;

	  ValueDictionaryBuilderIterator& operator=(
	      const ValueDictionaryBuilderValueIterator<Value, Builder>&
	          other
	  ) {
	    ParentType::operator=(other);
	    return *this;
	  }
	};
	
      }
    }
  }
}
#endif

