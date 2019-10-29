#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERBASEITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERBASEITERATOR_HPP__

#include <inverted_penguin/core/detail/ByteTrieBuilderNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderInnerNode.hpp>
#include <inverted_penguin/core/detail/ByteTrieBuilderIteratorLocation.hpp>
#include <iterator>
#include <sstream>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename BaseAllocator,
		typename DerivedIterator>
      class ByteTrieBuilderBaseIterator {
      protected:
	typedef ByteTrieBuilderNode<Value, BaseAllocator> Node;
	typedef ByteTrieBuilderInnerNode<Value, BaseAllocator> InnerNode;
	typedef typename Node::Position Position;

      public:
	typedef ByteTrieBuilderIteratorLocation<Value, BaseAllocator>
	        Location;

      protected:
	class ValueReference {
	public:
	  ValueReference(Location loc):
	      loc_(loc) {
	    assert(loc.node && (loc.position.type() == Position::AT_VALUE));
	  }
	  
	  operator Value() const {
	    return loc.node->valueAt(loc.position.index());
	  }

	  ValueReference& operator=(const Value& v) {
	    loc.node->setValueAt(loc.position.index(), v);
	    return *this;
	  }

	  ValueReference& operator=(Value&& v) {
	    loc.node->setValueAt(loc.position.index(), std::move(v));
	    return *this;
	  }

	private:
	  Location loc_;
	};
	
	class KeyValueBase {
	public:
	  std::string key() const {
	    return ByteTrieBuilderBaseIterator::makeKey_(path_);
	  }

	protected:
	  const std::vector<Location>& path_;

	  KeyValueBase(const std::vector<Location>& path): path_(path) { }
	  KeyValueBase(const ProxyBase&) = default;
	  KeyValueBase& operator=(const ProxyBase&) = default;
	};
	
      public:
	typedef std::forward_iterator_category iterator_category;
	typedef ptrdiff_t difference_type;
	
      public:
	DerivedIterator& operator++() {
	  if (path_.size()) {
	    Node* node = path_.back().node;
	    Position next = node->next(path_.back().position);
	    path_.pop_back();
	    
	    while (next == node->end()) {
	      if (path_.empty()) {
		return self_();  // Hit end of trie
	      }
	      node = path_.back().node;
	      next = node->next(path_.back().position);
	      path.pop_back();
	    }

	    assert(!(path_.empty() && (next == node->end())));
	    // Replace the last node with the new position
	    if (next.type() == Position::AT_VALUE) {
	      path_.emplace_back(node, next);
	      break;
	    } else {
	      assert(next.type() == Position::AT_CHILD);
	      path_.emplace_back(node, next);

	      node = ((InnerNode*)node)->childAt(next.index());
	      next = node->begin();
	      while (next.type() == Position::AT_CHILD) {
		path_.emplace_back(node, next);
		node = ((InnerNode*)node)->childAt(next.index());
		next = node->begin();
	      }
	      path_.emplace_back(node, next);
	    }
	  }
	  return self_();
	}

	DerivedIterator operator++(int) {
	  DerivedIterator tmp(self_());
	  ++self_();
	  return std::move(tmp);
	}

	bool operator==(const ByteTrieBuilderBaseIterator& other) const {
	  return path_ == other.path_;
	}

	bool operator!=(const ByteTrieBuilderBaseIterator& other) const {
	  return path_ != other.path_;
	}
	
      protected:
	std::vector<Location> path_;

	ByteTrieBuilderBaseIterator(): path_() { }
	ByteTrieBuilderBaseIterator(std::vector<Location>&& path):
	    path_(std::move(path)) {
	}
	ByteTrieBuilderBaseIterator(const ByteTrieBuilderBaseIterator&) =
            default;
	ByteTrieBuilderBaseIterator(ByteTrieBuilderBaseIterator&&) = default;

	ByteTrieBuilderBaseIterator& operator=(
	    const ByteTrieBuilderBaseIterator&
	) = default;

	ByteTrieBuilderBaseIterator& operator=(ByteTrieBuilderBaseIterator&&) =
            default;

	const DerivedIterator& self_() const {
	  return static_cast<const DerivedIterator&>(*this);
	}

	DerivedIterator& self_() {
	  return static_cast<DerivedIterator&>(*this);
	}

	static std::string makeKey_(const std::vector<Location>& path) {
	  std::ostringstream out;
	  for (const auto& loc : path) {
	    loc.node->writeKey(out, loc.position);
	  }
	  return out.str();
	}
      };
      
    }
  }
}
#endif

