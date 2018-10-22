#ifndef __INVERTED_PENGUIN__CORE__BYTETRIEBUILDER_HPP__
#define __INVERTED_PENGUIN__CORE__BYTETRIEBUILDER_HPP__

#include <inverted_penguin/core/InMemoryByteTrie.hpp>
#include <inverted_penguin/core/Optional.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderConstIterator.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderIterator.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderAllocators.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderBucketNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderDenseNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderSpaseNode.hpp>
#include <functional>
#include <memory>
#include <size_t>

namespace inverted_penguin {
  namespace core {

    template <typename Value, typename Allocator = std::allocator<uint8_t> >
    class ByteTrieBuilder {
    public:
      typedef detail::ByteTrieBuilderConstIterator<Value, Allocator>
              ConstIterator;
      typedef detail::ByteTrieBuilderIterator<Value, Allocator> Iterator;
      
    public:
      ByteTrieBuilder(const Allocator& allocator = Allocator()):
	  allocators_(allocator),
	  root_(allocators.newBucketNode(), allocators_.nodeDeleter()),
	  numKeys_(0) {
      }
      ByteTrieBuilder(const ByteTrieBuilder&) = delete;
      ByteTrieBuilder(ByteTrieBuilder&&) = default;
      ~ByteTrieBuilder() { }

      uint32_t size() const { return numKeys_; }
      ConstIterator begin() const { return ConstIterator(root_->begin()); }
      Iterator begin() { return Iterator(root_->begin()); }
      ConstIterator end() const { return ConstIterator(root_->end()); }
      Iterator end() { return Iterator(root_->end()); }

      ConstIterator find(const uint8_t* key, const uint8_t* keyEnd) const {
	return ConstIterator(root_->find(key, keyEnd));
      }
      
      Iterator find(const uint8_t* key, const uint8_t* keyEnd) {
	return Iterator(root_->find(key, keyEnd));
      }
      ConstIterator find(const char* key) const {
	return find(key, key + strlen(key));
      }
      Iterator find(const char* key) {
	return find(key, key + strlen(key));
      }

      std::tuple<ConstIterator, ConstIterator> prefix(
	  const uint8_t* key, const uint8_t* keyEnd
      ) const {
	typename Node::Position rangeStart;
	typename Node::Position rangeEnd;

	std::tie(rangeStart, rangeEnd) = root_->prefix(key, keyEnd);
	return std::make_tuple(ConstIterator(rangeStart),
			       ConstIterator(rangeEnd));
      }
      
      std::tuple<Iterator, Iterator> prefix(
	  const uint8_t* key, const uint8_t* keyEnd
      ) {
	typename Node::Position rangeStart;
	typename Node::Position rangeEnd;

	std::tie(rangeStart, rangeEnd) = root_->prefix(key, keyEnd);
	return std::make_tuple(Iterator(rangeStart), Iterator(rangeEnd));
      }
      
      std::tuple<ConstIterator, ConstIterator> prefix(const char* key) const {
	return prefix(key, key + strlen(key));
      }
      std::tuple<Iterator, Iterator> prefix(const char* key) {
	return prefix(key, key + strlen(key));
      }
      
      bool has(const uint8_t* key, const uint8_t* keyEnd) const {
	return root_->has(key, keyEnd);
      }
      bool has(const char* key) const {
	return has(key, key + strlen(key));
      }
      
      Optional<Value> get(const uint8_t* key, const uint8_t* keyEnd) const {
	return root_->get(key, keyEnd);
      }
      
      Optional<Value> get(const char* key) const {
	return get(key, key + strlen(key));
      }

      void add(const uint8_t* key, const uint8_t* keyEnd,
	       const Value& value) {
	std::unique_ptr< Node, std::function<void (Node*)> > =
	    root_->add(key, keyEnd, value, allocators_);
	if (newRoot != root_) {
	  root_ = std::move(newRoot);
	}
      }
      
      void add(const char* key, const Value& value) {
	add(key, key + strlen(key), value);
      }

      template <typename ValueFn>
      Value getOrAdd(const char* key, const char* keyEnd, ValueFn getValue) {
	Position p = root_->find(key, keyEnd);

	if (!p.atEnd()) {
	  return p.value();
	} else {
	  Value v = getValue();
	  this->add(key, keyEnd, v);
	  return v;
	}
      }

      void clear() {
	root_.reset(allocators_.newBucketNode());
	numKeys_ = 0;
      }

      InMemoryByteTrie<Value, Allocator> build() const {
	uint32_t treeDataSize = root_->computeTreeDataSize();
	uint8_t* data = allocators_.allocateBytes(treeDataSize);
	  
	try {
	  uint8_t* p = data;
	  std::function<void (const void*, size_t)> dataWriter =
	      [p](const void* d, size_t n) {
	          ::memcpy(p, d, n);
		  p += n;
	      };

	  this->write(dataWriter);
	  
	  return InMemoryByteTrie(data, allocators_.baseAllocator());
	} catch(...) {
	  allocators_.freeBytes(data, treeDataSize);
	  throw;
	}
      }

      uint32_t write(
	  const std::function<void (const void*, size_t)>& dataWriter
      ) const {
	return root_->write(dataWriter);
      }
      
      ByteTrieBuilder& operator=(const ByteTrieBuilder&) = delete;
      ByteTrieBuilder& operator=(ByteTrieBuilder&&) = default;

    protected:
      typedef detail::ByteTrieBuilderAllocators<Value, Allocator> Allocators;
      typedef detail::ByteTrieBuilderNode<Value, Allocator> Node;
      typedef detail::ByteTrieBuilderDenseNode<Value, Allocator> DenseNode;
      typedef detail::ByteTrieBuilderSparseNode<Value, Allocator> SparseNode;
      typedef detail::ByteTrieBuilderBucketNode<Value, Allocator> BucketNode;

    private:
      Allocators allocators_;
      std::unique_ptr<Node, std::function<void (Node*)> > root_;
      uint32_t numKeys_;
    };
    
  }
}

#endif
