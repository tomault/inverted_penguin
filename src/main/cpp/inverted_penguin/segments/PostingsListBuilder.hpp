#ifndef __INVERTED_PENGUIN__SEGMENTS__POSTINGSLISTSBUILDER_HPP__
#define __INVERTED_PENGUIN__SEGMENTS__POSTINGSLISTSBUILDER_HPP__

#include <inverted_penguin/core/VByteStreamWriter.hpp>
#include <inverted_penguin/core/DataWriter.hpp>
#include <stdint.h>

namespace inverted_penguin {
  namespace segments {

    class PostingsListBuilder {
    public:
      static const uint32_t NO_DOCS = (uint32_t)-1;
      
    public:
      PostingsListBuilder();
      PostingsListBuilder(const PostingsListBuilder&) = delete;
      PostingsListBuilder(PostingsListBuilder&&) noexcept = default;

      uint32_t lastDocId() const { return lastDocId_; }
      uint32_t numDocs() const { return numDocs_; }

      void add(uint32_t docId, uint32_t tf);
      
      size_t write(core::DataWriter& writer) const;

      PostingsListBuilder& operator=(const PostingsListBuilder&) = delete;
      PostingsListBuilder& operator=(PostingsListBuilder&&) noexcept = default;

    private:
      core::VByteStreamWriter postings_;
      uint32_t numDocs_;
      uint32_t lastDocId_;
    };
    
  }
}
#endif
