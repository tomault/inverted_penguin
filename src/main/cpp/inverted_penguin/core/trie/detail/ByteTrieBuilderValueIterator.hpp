#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERVALUEITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERVALUEITERATOR_HPP__

#include <inverted_penguin/core/detail/ByteTrieBuilderBaseIterator.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderIterator.hpp>
#include <vector>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderValueIterator :
	public ByteTrieBuilderBaseIterator<
	    Value, BaseAllocator,
	    ByteTrieBuilderValueIterator<Value, BaseAllocator>
	> {
      private:
	typedef ByteTrieBuilderBaseIterator<
	    Value, BaseAllocator,
	    ByteTrieBuilderConstValueIterator<Value, BaseAllocator>
	> Base;

	typedef ByteTrieBuilderIterator<Value, BaseAllocator> MutableIterator;
	
      public:
	using typename Base::Location;
	using typename Base::ValueReference;

	using typename Base::iterator_category;
	using typename Base::difference_type;
	typedef Value value_type;
	typedef ValueReference reference;
	typedef void pointer;

      public:
	ByteTrieBuilderValueIterator(): Base() { }
	ByteTrieBuilderValueIterator(std::vector<Location>&& path):
	    Base(std::move(path_)) {
	}

	ByteTrieBuilderValueIterator(
	    const ByteTrieBuilderValueIterator&
	) = default;

	ByteTrieBuilderValueIterator(
	    ByteTrieBuilderValueIterator&&
	) = default;
	
	ByteTrieBuilderValueIterator(const MutableIterator&):
	    Base(other) {
	}

	ByteTrieBuilderValueIterator(MutableIterator&&):
	    Base(std::move(other)) {
	}

	reference operator*() const { return reference(this->path_.back()); }

	ByteTrieBuilderValueIterator& operator=(
	    const ByteTrieBuilderValueIterator& other
	) = default;
	
	ByteTrieBuilderValueIterator& operator=(
	    ByteTrieBuilderValueIterator&& other
	) = default;

	ByteTrieBuilderValueIterator& operator=(const MutableIterator& other) {
	  Base::operator=(other);
	  return *this;
	}
	
	ByteTrieBuilderValueIterator& operator=(MutableIterator&& other) {
	  Base::operator=(std::move(other));
	  return *this;
	}
      };
      
    }
  }
}
#endif

