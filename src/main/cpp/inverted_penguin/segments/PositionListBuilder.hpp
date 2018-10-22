#ifndef __INVERTED_PENGUIN__SEGMENTS__POSITIONLISTSBUILDER_HPP__
#define __INVERTED_PENGUIN__SEGMENTS__POSITIONLISTSBUILDER_HPP__

#include <inverted_penguin/core/VByteStreamWriter.hpp>
#include <inverted_penguin/core/DataWriter.hpp>
#include <stdint.h>

namespace inverted_penguin {
  namespace segments {

    class PositionListBuilder {
    public:
      static const uint32_t NO_POSITION = (uint32_t)-1;
      
    public:
      PositionListBuilder();
      PositionListBuilder(const PositionListBuilder&) = delete;
      PositionListBuilder(PositionListBuilder&&) noexcept = default;

      bool docInProgress() const { return lastPosition_ != NO_POSITION; }
      uint32_t lastPosition() const { return positions_; }
      uint32_t numTermsInDoc() const { return numTermsInDoc_; }
      
      void add(uint32_t position);
      void endDoc();
      size_t write(core::DataWriter& writer) const;
      
      PositionListBuilder& operator=(const PositionListBuilder&) = delete;
      PositionListBuilder& operator=(PositionListBuilder&&) noexcept = default;

    private:
      core::VByteStreamWriter positions_;
      uint32_t lastPosition_;
      uint32_t numTermsInDoc_;
    };
    
  }
}
#endif

