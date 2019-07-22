#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYBUILDERCONSTVALUEITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYBUILDERCONSTVALUEITERATOR_HPP__

#include <inverted_penguin/core/dictionary/detail/ValueDictionaryBuilderBaseIterator.hpp>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {

	template <typename Value, typename Builder>
	class ValueDictionaryBuilderConstValueIterator :
	    public ValueDictionaryBuilderBaseIterator<
	        Builder,
	        ValueDictionaryBuilderConstValueIterator<Value, Builder>
	    > {
	private:
	  typedef ValueDictionaryBuilderBaseIterator<
	      Builder, ValueDictionaryBuilderConstValueIterator<Value, Builder>
	  > ParentType;
	  
	public:
	  typedef Value value_type;
	  typedef const Value& reference;

	public:
	  ValueDictionaryBuilderConstValueIterator():
	      ValueDictionaryBuilderConstValueIterator(nullptr, 0) {
	  }
	  
	  ValueDictionaryBuilderConstValueIterator(Builder* builder,
						   uint32_t index):
	      ParentType(builder, index) {
	  }
	  
	  ValueDictionaryBuilderConstValueIterator(
	      const ValueDictionaryBuilderConstValueIterator&
	  ) = default;

	  template <typename OtherIterator>
	  ValueDictionaryBuilderConstValueIterator(
	      const ValueDictionaryBuilderBaseIterator<
	          Builder, OtherIterator
	      >& other):
	      ParentType(other) {
	  }

	  reference operator*() const {
	    return this->builder_->valueAt(this->index_);
	  }

	  ValueDictionaryBuilderConstValueIterator& operator=(
	      const ValueDictionaryBuilderConstValueIterator&
	  ) = default;

	  template <typename OtherIterator>
	  ValueDictionaryBuilderConstValueIterator& operator=(
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

