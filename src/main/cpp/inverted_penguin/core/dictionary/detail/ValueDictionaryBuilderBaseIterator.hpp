#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYBUILDERBASEITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYBUILDERBASEITERATOR_HPP__

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      template <typename Value, typename KeyAllocator,
		typename DictionaryAllocator>
      class ValueDictionaryBuilder;
      
      namespace detail {
	
	template <typename Builder, typename DerivedIterator>
	class ValueDictionaryBuilderBaseIterator {
	public:
	  typedef std::forward_iterator_tag iterator_category;
	  typedef ptrdiff_t difference_type;

	public:
	  DerivedIterator& operator++() {
	    ++index_;
	    return self_();
	  }

	  DerivedIterator operator++(int) {
	    DerivedIterator tmp(self_());
	    ++index_;
	    return tmp;
	  }

	  template <typename OtherIterator>
	  bool operator==(
	      const ValueDictionaryBuilderBaseIterator<Builder,
	                                               OtherIterator>& other
	  ) const {
	    return (builder_ == other.builder_) && (index_ == other.index_);
	  }

	  template <typename OtherIterator>
	  bool operator!=(
	      const ValueDictionaryBuilderBaseIterator<Builder,
	                                               OtherIterator>& other
	  ) const {
	    return (builder_ != other.builder_) || (index_ != other.index_);
	  }

	protected:
	  Builder* builder_;
	  uint32_t index_;

	  ValueDictionaryBuilderBaseIterator(Builder* builder, uint32_t index):
	      builder_(builder), index_(index) {
	  }

	  ValueDictionaryBuilderBaseIterator(
	      const ValueDictionaryBuilderBaseIterator&
	  ) = default;

	  template <typename OtherIterator>
	  ValueDictionaryBuilderBaseIterator(
	      const ValueDictionaryBuilderBaseIterator<
	          Builder, OtherIterator
	      >& other):
	      builder_(other.builder_), index_(other.index_) {
	  }

	  ValueDictionaryBuilderBaseIterator& operator=(
	      const ValueDictionaryBuilderBaseIterator&
	  ) = default;

	  template <typename OtherIterator>
	  ValueDictionaryBuilderBaseIterator& operator=(
	      const ValueDictionaryBuilderBaseIterator<
	          Builder, OtherIterator
	      >& other
	  ) {
	    builder_ = other.builder_;
	    index_ = other.index_;
	  }

	  const DerivedIterator& self_() const {
	    return static_cast<const DerivedIterator&>(*this);
	  }

	  DerivedIterator& self_() {
	    return static_cast<DerivedIterator&>(*this);
	  }

	  template <typename B, typename O>
	  friend class ValueDictionaryBuilderBaseIterator;
	};
	
      }
    }
  }
}
#endif

