#ifndef __INVERTED_PENGUIN__TERMS__DYNAMICMODIFIEDTERMSTREAM_HPP__
#define __INVERTED_PENGUIN__TERMS__DYNAMICMODIFIEDTERMSTREAM_HPP__

#include <pistis/memory/PtrUtils.hpp>
#include <inverted_penguin/terms/DynamicTermStream.hpp>
#include <inverted_penguin/terms/DynamicTermStreamModifier.hpp>
#include <inverted_penguin/terms/Term.hpp>
#include <memory>

namespace inverted_penguin {
  namespace terms {

    class DynamicModifiedTermStream : public DynamicTermStream {
    public:
      DynamicModifiedTermStream(
	  std::unique_ptr<DynamicTermStream>&& stream,
	  const std::shared_ptr<DynamicTermStreamModifier>& modifier
      ):
	  stream_(std::move(stream)),
	  modifier_(DynamicTermStreamModifier::reuse(modifier)) {
      }
      virtual ~DynamicModifiedTermStream() { }

      DynamicTermStream* source() const { return stream_.get(); }
      const std::shared_ptr<DynamicTermStreamModifier> modifier() const {
	return modifier_;
      }
      
      virtual Term next() override { return modifier_->next(*stream_); }

      virtual bool reset() override {
	const bool streamReset = stream_->reset();
	const bool modifierReset = modifier_->reset();
	return streamReset && modifierReset;
      }

      virtual DynamicTermStream* duplicate() const {
	return new DynamicModifiedTermStream(
	    pistis::memory::wrapUnique(stream_->duplicate()),
	    DynamicTermStreamModifier::reuse(modifier_)
	);
      }
      
    private:
      std::unique_ptr<DynamicTermStream> stream_;
      std::shared_ptr<DynamicTermStreamModifier> modifier_;
    };

    
  }
}
#endif

