#include <inverted_penguin/segments/PostingsListBuilder.hpp>
#include <assert.h>

using namespace inverted_penguin::core;
using namespace inverted_penguin::segments;

PostingsListBuilder::PostingsListBuilder():
  postings_(), numDocs_(0), lastDocId_(NO_DOCS) {
}

void PostingsListBuilder::add(uint32_t docId, uint32_t tf) {
  if (lastDocId_ == NO_DOCS) {
    postings_.write(docId);
  } else {
    ::assert(docId > lastDocId_);
    postings_.write(docId - lastDocId_);
  }
  postings_.write(tf);
  lastDocId_ = docId;
  ++numDocs_;
}

size_t PostingsListBuilder::write(core::DataWriter& writer) const {
  return writer.write(postings_.begin(), postings_.size());
}
