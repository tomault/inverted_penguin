#ifndef __INVERTED_PENGUIN__CORE__INMEMORYBYTETRIE_HPP__
#define __INVERTED_PENGUIN__CORE__INMEMORYBYTETRIE_HPP__

#include <memory>

namespace inverted_penguin {
  namespace core {

    template <typename Value, typename DataAllocator = std::allocator<uint8_t>,
	      typename KeyAllocator = DataAllocator>
    class InMemoryByteTrie :
        public ByteTrie<Value, KeyAllocator>,
        protected DataAllocator
    {
    public:
      InMemoryByteTrie(const uint8_t* data, size_t trieSize,
		       const DataAllocator& dataAllocator = DataAllocator(),
		       const KeyAllocator& allocator = KeyAllocator()):
	  DataByteTrie(data, keyAllocator), trieSize_(trieSize) {
      }
      ~InMemoryByteTrie() { this->deallocate(this->base_, trieSize_); }

    private:
      size_t trieSize_;
    };
    
  }
}
#endif

