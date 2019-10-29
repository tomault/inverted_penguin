#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERCONSTVALUEITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERCONSTVALUEITERATOR_HPP__
#include <inverted_penguin/core/detail/ByteTrieBuilderBaseIterator.hpp>
#include <vector>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderConstValueIterator :
	public ByteTrieBuilderBaseIterator<
	    Value, BaseAllocator,
	    ByteTrieBuilderConstValueIterator<Value, BaseAllocator>
	> {
      private:
	typedef ByteTrieBuilderBaseIterator<
	    Value, BaseAllocator,
	    ByteTrieBuilderConstValueIterator<Value, BaseAllocator>
	> Base;
					    
      public:
	using typename Base::Location;

	using typename Base::iterator_category;
	using typename Base::difference_type;
	typedef Value value_type;
	typedef const Value& reference;
	typedef void pointer;

      public:
	ByteTrieBuilderConstValueIterator(): Base() { }
	ByteTrieBuilderConstValueIterator(std::vector<Location>&& path):
	    Base(std::move(path_)) {
	}
	ByteTrieBuilderConstValueIterator(const Base& other): Base(other) { }
	ByteTrieBuilderConstValueIterator(Base&& other):
	    Base(std::move(other)) {
	}

	reference operator*() const {
	  const Location loc = this->path_.back();
	  return loc.node->valueAt(loc.position);
	}

	ByteTrieBuilderConstValueIterator& operator=(const Base& other) {
	  Base::operator=(other);
	  return *this;
	}
	
	ByteTrieBuilderConstValueIterator& operator=(Base&& other) {
	  Base::operator=(std::move(other));
	  return *this;
	}
      };
      
    }
  }
}
#endif

