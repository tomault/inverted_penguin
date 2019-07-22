#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__INMEMORYVALUEDICTIONARYIMPL_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__INMEMORYVALUEDICTIONARYIMPL_HPP__

#include <inverted_penguin/core/dictionary/detail/ValueDictionaryImpl.hpp>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {

	template <typename Value, typename Allocator>
	class InMemoryValueDictionaryImpl :
	    public ValueDictionaryImpl<Value>,
	    protected Allocator {
	public:
	  typedef Allocator AllocatorType;

	public:
	  InMemoryValueDictionaryImpl(const uint8_t* base, size_t sizeInBytes,
				      const Allocator& allocator):
	      ValueDictionaryImpl<Value>(base), Allocator(allocator),
	      sizeInBytes_(sizeInBytes) {
	  }

	  InMemoryValueDictionaryImpl(InMemoryValueDictionaryImpl&& other):
	      ValueDictionaryImpl<Value>(std::move(other)),
	      Allocator(std::move(other.allocator())),
	      sizeInBytes_(other.sizeInBytes_) {
	    other.sizeInBytes_ = 0;
	  }
	  
	  ~InMemoryValueDictionaryImpl() { release_(); }

	  const Allocator& allocator() const {
	    return static_cast<const Allocator&>(*this);
	  }

	  Allocator& allocator() {
	    return static_cast<Allocator&>(*this);
	  }

	  size_t sizeInBytes() const { return sizeInBytes_; }

	  InMemoryValueDictionaryImpl& operator=(
	      InMemoryValueDictionaryImpl&& other
	  ) {
	    if (this != &other) {
	      release_();

	      Allocator::operator=(std::move(other.allocator()));
	      ValueDictionaryImpl<Value>::operator=(std::move(other));
	      sizeInBytes_ = other.sizeInBytes_;
	      other.sizeInBytes_ = 0;
	    }
	    return *this;
	  }

	private:
	  size_t sizeInBytes_;

	  void release_() {
	    if (this->base_) {
	      this->deallocate(const_cast<uint8_t*>(this->base_), sizeInBytes_);
	    }
	  }
	};
	
      }
    }
  }
}
#endif

