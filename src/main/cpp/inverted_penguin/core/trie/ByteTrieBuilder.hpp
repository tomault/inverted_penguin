#ifndef __INVERTED_PENGUIN__CORE__BYTETRIEBUILDER_HPP__
#define __INVERTED_PENGUIN__CORE__BYTETRIEBUILDER_HPP__

#include <inverted_penguin/core/InMemoryByteTrie.hpp>
#include <inverted_penguin/core/Optional.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderAllocators.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderIterators.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderInnerNode.hpp>
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
      typedef detail::ByteTrieBuilderKeyIterator<Value, Allocator> KeyIterator;
      typedef detail::ByteTrieBuilderConstValueIterator<Value, Allocator>
              ConstValueIterator;
      typedef detail::ByteTrieBuilderValueIterator<Value, Allocator>
              ValueIterator;

    protected:
      typedef detail::ByteTrieBuilderAllocators<Value, Allocator> Allocators;
      typedef detail::ByteTrieBuilderNode<Value, Allocator> Node;
      typedef detail::ByteTrieBuilderInnerNode<Value, Allocator> InnerNode;
      typedef detail::ByteTrieBuilderDenseNode<Value, Allocator> DenseNode;
      typedef detail::ByteTrieBuilderSparseNode<Value, Allocator> SparseNode;
      typedef detail::ByteTrieBuilderBucketNode<Value, Allocator> BucketNode;

      typedef typename Node::Position Position;
      typedef typename ConstIterator::Location Location;

    public:
      ByteTrieBuilder(const Allocator& allocator = Allocator()):
	  allocators_(allocator),
	  root_(allocators.newBucketNode(), allocators_.nodeDeleter()) {
      }
      ByteTrieBuilder(const ByteTrieBuilder&) = delete;
      ByteTrieBuilder(ByteTrieBuilder&&) = default;
      ~ByteTrieBuilder() { }

      uint32_t numValues() const {
	uint32_t total = 0;
	walk_([&total](const Node* node) { total += node->numValues(); });
	return total;
      }

      uint32_t numNodes() const {
	uint32_t total = 0;
	walk_([&total](const Node* node) { ++total; });
	return total;
      }
      
      uint32_t numBuckets() const {
	uint32_t total = 0;
	walk_([&total](const Node* node) {
	    if (node->nodeType() == BucketNode::NODE_TYPE) {
	      ++total;
	    }
	});
	return total;
      }
      
      ConstIterator begin() const {
	return ConstIterator(pathToFirstValue_(root_.get()));
      }
      Iterator begin() { return Iterator(pathToFirstValue_(root_.get())); }
      ConstIterator end() const { return ConstIterator(); }
      Iterator end() { return Iterator(); }
      ConstIterator cbegin() const {
	return ConstIterator(pathToFirstValue_(root_.get()));
      }
      ConstIterator cend() const { return ConstIterator(); }

      ConstIterator find(const uint8_t* key, const uint8_t* keyEnd) const {
	return ConstIterator(pathToKey_(key, keyEnd));
      }
      
      Iterator find(const uint8_t* key, const uint8_t* keyEnd) {	
	return Iterator(pathToKey_(key, keyEnd));
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
	std::vector<Location> start;
	std::vector<Location> end;

	std::tie(start, end) = pathToPrefix_(key, keyEnd);
	return std::make_tuple(ConstIterator(std::move(start)),
			       ConstIterator(std::move(end)));
      }
      
      std::tuple<Iterator, Iterator> prefix(
	  const uint8_t* key, const uint8_t* keyEnd
      ) {
	std::vector<Location> start;
	std::vector<Location> end;

	std::tie(start, end) = pathToPrefix_(key, keyEnd);
	return std::make_tuple(Iterator(std::move(start)),
			       Iterator(std::move(end)));
      }
      
      std::tuple<ConstIterator, ConstIterator> prefix(const char* key) const {
	return prefix(key, key + strlen(key));
      }
      
      std::tuple<Iterator, Iterator> prefix(const char* key) {
	return prefix(key, key + strlen(key));
      }
      
      bool has(const uint8_t* key, const uint8_t* keyEnd) const {
	const auto searchBucket =
	    [](const Location& parent, BucketNode* node,
	       const uint8_t* k, const uint8_t* keyEnd) {
	      return node->find(k, keyEnd) != node->end();
	    };
	const auto searchInnerNode =
	    [](const Location& parent, InnerNode* node, uint8_t k) {
	      return node->findValue(k) != node->end();
	    };
	const auto failAtInnerNode =
	    [](const Location& parent, InnerNode* node, const uint8_t* k,
	       const uint8_t* keyEnd) {
	      return false;
	    };
	const auto doNothing =
	    [](const Location& parent, Node* child, uint8_t k) { };
	const auto failOnEmptyKey = []() { return false; }

	return descend_(key, keyEnd, searchBucket, searchInnerNode,
			failAtInnerNode, doNothing, failOnEmptyKey);
      }
      
      bool has(const char* key) const {
	return has(key, key + strlen(key));
      }
      
      Optional<Value> get(const uint8_t* key, const uint8_t* keyEnd) const {
	const auto searchBucket =
	    [](const Location& parent, BucketNode* node, const uint8_t* k,
	       const uint8_t* keyEnd) {
	      Position p = node->find(k, keyEnd);
	      if (p != node->end()) {
		return Optional<Value>(node->valueAt(p.index()));
	      } else {
		return Optional<Value>();
	      }
	    };
	const auto searchInnerNode =
	    [](const Location& parent, InnerNode* node, uint8_t k) {
	      Position p = node->findValue(k);
	      if (p != node->end()) {
		return Optional<Value>(node->valueAt(p.index()));
	      } else {
		return Optional<Value>();
	      }
	    };
	const auto failAtInnerNode =
	    [](const Location& parent, InnerNode* node, const uint8_t* k,
	       const uint8_t* keyEnd) {
	      return Optional<Value>();
	    };
	const auto doNothing =
	    [](const Location& parent, Node* child, uint8_t k) { };
	const auto failOnEmptyKey = []() { return Optional<Value>(); }

	return descend_(key, keyEnd, searchBucket, searchInnerNode,
			failAtInnerNode, doNothing, failOnEmptyKey);
      }
      
      Optional<Value> get(const char* key) const {
	return get(key, key + strlen(key));
      }

      void add(const uint8_t* key, const uint8_t* keyEnd,
	       const Value& value) {
	const auto addToBucket = 
            [this, &value](const Location& parent, BucketNode* node,
			   const uint8_t* k, const uint8_t* keyEnd) {
	      Node* newNode = node->add(k, keyEnd, value, allocators_);
	      changeChildIfNeeded_(parent, node, newNode);
	      return 0;  // Return valued ignored
	    };
	const auto addValueToInnerNode =
	    [&value](const Location& parent, InnerNode* node, uint8_t k) {
	       node->addValue(k, value);
	       return 0; // Return value ignored
	    };
	const auto addChildToInnerNode =
	    [this, &value](const Location& parent, InnerNode* node,
			   const uint8_t* k, const uint8_t* keyEnd) {
	       Node* newBucket =
	           allocators_.newBucketNode(k + 1, keyEnd, value);
	       Node* newChild = parent.node->addChild(*k, newBucket,
						      allocators_);
	       changeChildIfNeeded_(parent, node, newNode);
	    };
	const auto doNothing =
	    [](const Location& parent, Node* child, uint8_t k) { };
	const auto failOnEmptyKey =
	    []() {
	      throw pistis::exception::IllegalValueError(
		  "Cannot add empty key", PISTIS_EX_HERE
	      );
	    };
	descend_(key, keyEnd, addToBucket, addValueToInnerNode,
		 addChildToInnerNode, doNothing, failOnEmptyKey);
      }
      
      void add(const char* key, const Value& value) {
	add(key, key + strlen(key), value);
      }

      template <typename ValueFn>
      Value getOrAdd(const char* key, const char* keyEnd, ValueFn getValue) {
	const auto checkBucket =
	    [this, &getValue](const Location& parent, BucketNode* node,
			      const uint8_t* k, const uint8_t* keyEnd) {
	      Position p = node->find(k, keyEnd);
	      if (p != node->end()) {
		return node->valueAt(p.index());
	      } else {
		Value v = getValue();
		Node* newNode = node->add(k, keyEnd, v);
		changeChildIfNeeded_(parent, node, newNode);
		return std::move(v);
	      }
	    };

	const auto checkInnerNode =
	    [&getValue](const Location& parent, InnerNode* node, uint8_t k) {
	      Position p = node->findValue(k);
	      if (p != node->end()) {
		return node->valueAt(p.index());
	      } else {
		p = node->addValue(k, getValue());
		return node->valueAt(p.index());
	      }
	    };

	const auto extendInnerNode =
	    [this, getValue](const Location& parent, InnerNode* node,
			     const uint8_t* k, const uint8_t* keyEnd) {
	      Value v = getValue();
	      Node* newBucket = allocators_.newBucketNode(k + 1, keyEnd, v);
	      Node* newChild = node->addChild(*k, newBucket, allocators_);
	      changeChildIfNeeded_(parent, node, newNode);
	      return std::move(v);
	    };

	const auto doNothing =
	    [](const Location& parent, Node* child, uint8_t k) { };

	const auto failOnEmptyKey =
	    []() {
	      throw pistis::exception::IllegalValueError(
		  "Cannot add empty key", PISTIS_EX_HERE
	      );
	    };

	return descend_(key, keyEnd, checkBucket, checkInnerNode,
			extendInnerNode, doNothing failOnEmptyKey);
      }

      void clear() {
	allocators_.destroyNode(root_.release());
	root_.reset(allocators_.newBucketNode());
      }

      InMemoryByteTrie<Value, Allocator> build() const {
	size_t treeDataSize = root_->computeTreeDataSize();
	uint8_t* data = allocators_.allocateBytes(treeDataSize);
	  
	try {
	  uint8_t* p = data;
	  std::function<void (const void*, size_t)> dataWriter =
	      [&p](const void* d, size_t n) {
	          ::memcpy(p, d, n);
		  p += n;
	      };

	  this->write(dataWriter);
	  assert((p - data) == treeDataSize);

	  // TODO: Allow for a specialized key allocator
	  return InMemoryByteTrie(data, treeDataSize,
				  allocators_.baseAllocator(),
				  allocators_.baseAllocator());
	} catch(...) {
	  allocators_.freeBytes(data, treeDataSize);
	  throw;
	}
      }

      uint32_t write(
	  const std::function<void (const void*, size_t)>& dataWriter
      ) const {
	// First four bytes is offset to root node OR'd with the type
	// code for the root
	const uint32_t rootOffset = 4 | root_->serializedTypeCode();
	dataWriter(&rootOffset, 4);
	return root_->write(dataWriter) + 4;
      }
      
      ByteTrieBuilder& operator=(const ByteTrieBuilder&) = delete;
      ByteTrieBuilder& operator=(ByteTrieBuilder&&) = default;

    private:
      Allocators allocators_;
      std::unique_ptr<Node, std::function<void (Node*)> > root_;

      template <typename Function>
      void walk_(Function f) {
	std::vector<Node*> stack;

	// Depth-first tree traversal
	stack.push_back(root_.get());
	while (stack.size()) {
	  Node* node = stack.back();
	  const uint16_t numChildren = node->numChildren();
	  
	  stack.pop_back();
	  f(node);
	  
	  if (numChildren) {
	    for (uint16_t i = numChildren; i > 0; --i) {
	      stack.push_back(((InnerNode*)node)->childAt(i - 1));
	    }
	  }
	}
      }

      template <typename EndAtBucketFn, typename EndAtInnerNodeFn,
		typename ContinueInnerNodeFn,
		typename DescendToChildFn,
		typename EmptyKeyFunction>
      auto descend_(const uint8_t* const key, const uint8_t* const keyEnd,
		    EndAtBucketFn endAtBucket, EndAtInnerNodeFn endAtInnerNode,
		    ContinueInnerNodeFn continueInnerNode,
		    DescendToChildFn descendToChild,
		    EmptyKeyFunction handleEmptyKey) const {
	if (k >= keyEnd) {
	  return handleEmptyKey();
	} else {
	  const uint8_t* k = key;
	  Location parent;
	  Node* node = root_.get();
	
	  while (k < keyEnd) {
	    if (node->nodeType() == BucketNode::NODE_TYPE) {
	      return endAtBucket(parent, (BucketNode*)node, k, keyEnd);
	    } else if ((keyEnd - k) == 1) {
	      return endAtInnerNode(parent, (InnerNode*)node, *k);
	    } else {
	      Position p = ((InnerNode*)node)->findChild(*k);
	      if (p == node->end()) {
		return continueInnerNode(parent, (InnerNode*)node, k, keyEnd);
	      } else {
		parent.node = node;
		parent.position = p;
		node = ((InnerNode*)node)->childAt(p.index());
		descendToChild(parent, node, *k);
		++k;
	      }
	    }
	  }

	  // Should never get here if the above loop is coded correctly
	  assert(false);
	}
      }

      void changeChildIfNeeded_(const Location& parent, Node* child,
				Node* newChild) {
	if (child == newChild) {
	  // Child didn't change, so do nothing
	} else if (parent.node) {
	  // Not at the root.  The parent owns the child, so it will destroy it
	  assert(parent.node->nodeType() != BucketNode::NODE_TYPE);
	  ((InnerNode*)parent.node)->replaceChildAt(parent.position.index(),
						    newChild, allocators_);
	} else {
	  // Need to replace the root
	  root_.reset(newChild);
	}
      }
      
      std::vector<Location> pathToFirstValue_(Node* start) const {
	Node* node = start;
	Position p = node->begin();
	std::vector<Location> path;

	while (p.type() == Position::AT_CHILD) {
	  path.push_back(Location(node, p));
	  node = ((InnerNode*)node)->childAt(p.index());
	  p = node->begin();
	}

	path.push_back(Location(node, p));
	return std::move(path);
      }
      
      std::vector<Location> pathToKey_(const uint8_t* key,
				       const uint8_t* keyEnd) const {
	std::vector<Location> path;
	const auto searchBucket =
	    [&path](const Location& parent, BucketNode* node,
		    const uint8_t* k, const uint8_t* keyEnd) {
	      Position p = node->find(k, keyEnd);
	      if (p != node->end()) {
		path.emplace_back(node, p);
		return std::move(path);
	      } else {
		return std::vector<Location>();
	      }
	    };
	const auto searchInnerNode =
	    [&path](const Location& parent, InnerNode* node, uint8_t k) {
	      Position p = node->findValue(k);
	      if (p == node->end()) {
		return std::vector<Location>();
	      } else {
		path.emplace_back(node, p);
		return std::move(path);
	      }
	    };
	const auto failAtInnerNode =
	    [](const Location& parent, InnerNode* node, const uint8_t* k,
	       const uint8_t* keyEnd) {
	      return std::vector<Location>();
	    };
	const auto descendToChild =
	    [&path](const Location& parent, Node* child, uint8_t k) {
	      path.emplace_back(parent);
	    };
	const auto handleEmptyKey = []() { return std::vector<Location>(); }

	return descend_(key, keyEnd, searchBucket, searchInnerNode,
			failAtInnerNode, descendToChild, handleEmptyKey);

      }
      
      std::tuple< std::vector<Location>, std::vector<Location> > pathToPrefix_(
	  const uint8_t* key, const uint8_t* keyEnd
      ) const {
	std::vector<Location> beginPath;
	std::vector<Location> endPath;
	const auto searchBucket =
	    [this, &beginPath, &endPath](const Location& parent,
					 BucketNode* node,
					 const uint8_t* k,
					 const uint8_t* keyEnd) {
	      Position s, e;
	      std::tie(s, e) = ((BucketNode*)node)->prefix(k, keyEnd);
	      if (s == e) {
		return std::make_tuple(std::vector<Location>(),
				       std::vector<Location>());
	      } else {
		beginPath.emplace_back(node, s);
		if (e != node->end()) {
		  endPath.emplace_back(node, e);
		} else {
		  setToNextValue_(endPath);
		}
		return std::make_tuple(beginPath, endPath);
	      }
	    };
	const auto searchInnerNode =
	    [this, &beginPath, &endPath](const Location& parent,
					 InnerNode* node,
					 const uint8_t k) {
	      Position p = node->findKey(k);
	      if (p == node->end()) {
		return std::make_tuple(std::vector<Location>(),
				       std::vector<Location>());
	      } else {
		beginPath.emplace_back(node, p);

		Position next = node->after(p);
		if (next != node->end()) {
		  endPath.emplace_back(node, end);
		} else {
		  setToNextValue_(endPath);
		}
		return std::make_tuple(beginPath, endPath);
	      }
	    };
	const auto failAtInnerNode =
	    [](const Location& parent, InnerNode* child, const uint8_t* k,
	       const uint8_t* keyEnd) {
	      return std::make_tuple(std::vector<Location>(),
				     std::vector<Location>());
	    };
	const auto descendToChild =
	    [&beginPath, &endPath](const Location& parent, InnerNode* child,
				   uint8_t k) {
	      beginPath.emplace_back(parent);
	      endPath.emplace_back(parent);
	    };
	const auto handleEmptyKey =
	    []() {
	      return std::make_tuple(std::vector<Location>(),
				     std::vector<Location>());
	    };
	
	return descend_(key, keyEnd, searchBucket, searchInnerNode,
			failAtInnerNode, descendToChild, handleEmptyKey);
      }

      void setToNextValue_(std::vector<Location>& path) {
	while (path.size()) {
	  assert(path.back().position.type() == Position::AT_CHILD);
	  Location loc = path.back();
	  Position next = loc.node->after(loc.position);

	  if (next == loc.node->end()) {
	    path.pop_back();
	  } else {
	    path.back().position = next;
	    break;
	  }
	}
      }
      
    };
    
  }
}

#endif
