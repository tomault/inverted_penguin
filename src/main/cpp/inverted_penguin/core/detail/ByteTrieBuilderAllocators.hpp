#ifndef __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERALLOCATORS_HPP__
#define __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERALLOCATORS_HPP__

#include <pistis/exceptions/IllegalStateError.hpp>
#include <memory>

namespace inverted_penguin {
  namespace core {

    template <typename Value, typename BaseAllocator>
    class ByteTrieBuilderDenseNode;

    template <typename Value, typename BaseAllocator>
    class ByteTrieBuilderSparseNode;

    template <typename Value, typename BaseAllocator>
    class ByteTrieBuilderBucketNode;

    template <typename, typename BaseAllocator>
    class ByteTrieBuilderAllocators {
    public:
      typedef typename std::allocator_traits<BaseAllocator>::rebind_alloc<
          ByteTrieBuilderDenseNode<Value, BaseAllocator>
      > DenseNodeAllocator;
      typedef typename std::allocator_traits<BaseAllocator>::rebind_alloc<
	  ByteTrieBuilderSparseNode<Value, BaseAllocator>
      > SparseNodeAllocator;
      typedef typename std::allocator_traits<BaseAllocator>::rebind_alloc<
	  ByteTrieBuilderBucketNode<Value, BaseAllocator>
      > BucketNodeAllocator;

      typedef ByteTrieBuilderDenseNode<Value, BaseAllocator> DenseNode;
      typedef ByteTrieBuilderSparseNode<Value, BaseAllocator> SparseNode;
      typedef ByteTrieBuilderBucketNode<Value, BaseAllocator> BucketNode;

    public:
      ByteTrieBuilderAllocators(const BaseAllocator& base):
	  baseAllocator_(base), denseNodeAllocator(base),
	  sparseNodeAllocator(base), bucketNodeAllocator(base) {
      }
      
      BaseAllocator& baseAllocator() { return byteAllocator_; }
      DenseNodeAllocator& denseNodeAllocator() { return denseNodeAllocator_; }
      SparseNodeAllocator& sparseNodeAllocator() {
	return sparseNodeAllocator_;
      }
      BucketNodeAllocator& bucketNodeAllocator() {
	return bucketNodeAllocator_;
      }
      
      uint8_t* allocateBytes(size_t n) { return byteAllocator_.allocate(n); }
      void freeBytes(uint8_t* p, size_t n) { byteAllocator_.deallocate(p, n); }

      void destroyNode(Node* node) {
	switch (node->nodeType()) {
	  case BucketNode::NODE_TYPE:
	    destroyBucketNode((BucketNode*)node);
	    break;
	    
	  case SparseNode::NODE_TYPE:
	    destroySparseNode((SparseNode*)node);
	    break;
	    
	  case DenseNode::NODE_TYPE:
	    destroyDenseNode((DenseNode*)node);
	    break;

	  default:
	    throw pistis::exceptions::IllegalStateError("Unknown node type",
							PISTIS_EX_HERE);
	}
      }
      
      template <typename... Args>
      BucketNode* newBucketNode(Args&&... args) {
	return createNewNode_<BucketNode>(bucketNodeAllocator_,
					  std::forward<Args>(args)...);
      }

      void destroyBucketNode(BucketNode* node) {
	node->~BucketNode();
	bucketNodeAllocator_.deallocate(node, 1);
      }

      template <typename... Args>
      SparseNode* newSparseNode(Args&&... args) {
	return createNewNode_<SparseNode>(sparseNodeAllocator_,
					  std::forward<Args>(args)...);
      }

      void destroySparseNode(SparseNode* node) {
	node->~SparseNode();
	sparseNodeAllocator_.deallocate(node, 1);
      }

      template <typename... Args>
      DenseNode* newDenseNode(Args&&... args) {
	return createNewNode_<DenseNode>(denseNodeAllocator_,
					 std::forward<Args>(args)...);
      }
      
      void destroyDenseNode(DenseNode* node) {
	node->~DenseNode();
	denseNodeAllocator_.deallocate(node, 1);
      }

      std::function<void (Node*)> nodeDeleter() {
	auto deleteNode = [this](Node* node) { this->destroyNode(node); };
	return std::function<void (Node*)>(deleteNode);
      }

    private:
      BaseAllocator byteAllocator_;
      DenseNodeAllocator denseNodeAllocator_;
      SparseNodeAllocator sparseNodeAllocator_;

      template <typename T, typename A, typename... Args>
      static T* createNewNode_(A& allocator, Args&&... args) {
	T* newNode = allocator.allocate(1);
	try {
	  new(newNode) T(std::forward<Args>(args)...);
	  return newNode;
	} catch(...) {
	  a.deallocate(newNode, 1);
	}
      }
    };
  }
}
#endif

