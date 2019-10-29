#ifndef __INVERTED_PENGUIN__TERMS__DYNAMICTERMSTREAMWRAPPER_HPP__
#define __INVERTED_PENGUIN__TERMS__DYNAMICTERMSTREAMWRAPPER_HPP__

#include <inverted_penguin/terms/DynamicTermStream.hpp>
#include <inverted_penguin/terms/Term.hpp>
#include <vector>
#include <utility>

namespace inverted_penguin {
  namespace terms {

    template <typename WrappedStream>
    class DynamicTermStreamWrapper : public DynamicTermStream {
    public:
      template <typename... Args>
      DynamicTermStreamWrapper(Args&&... args):
	  stream_(std::forward<Args>(args)...) {
      }
      DynamicTermStreamWrapper(const DynamicTermStreamWrapper&) = default;
      DynamicTermStreamWrapper(DynamicTermStreamWrapper&&) = default;

      const WrappedStream& unwrap() const { return stream_; }
      WrappedStream& unwrap() { return stream_; }

      virtual Term next() override { return stream_.next(); }
      virtual bool reset() override { return stream_.reset(); }

      virtual DynamicTermStreamWrapper* duplicate() const override {
	return new DynamicTermStreamWrapper(stream_);
      }

      DynamicTermStreamWrapper& operator=(
	  const DynamicTermStreamWrapper&
      ) = default;
      
      DynamicTermStreamWrapper& operator=(
	  DynamicTermStreamWrapper&&
      ) = default;
      
    private:
      WrappedStream stream_;
    };
    
  }
}

#endif
