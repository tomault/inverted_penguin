#include "PositionListBuilder.hpp"
#include <assert.h>

using namespace inverted_penguin::core;
using namespace inverted_penguin::segments;

PositionListBuilder::PositionListBuilder():
    positions_(), lastPosition_(NO_POSITION), numTermsInDoc(0) {
}

void PositionListBuilder::add(uint32_t position) {
  if (lastPosition_ == NO_POSITION) {
    positions_.write(position);
  } else {
    assert(position > lastPosition_);
    positions_.write(position - lastPosition_);
  }
  lastPosition_ = position;
  ++numTermsInDoc_;
}

void PositionListBuilder::endDoc() {
  lastPosition_ = NO_POSITION;
  numTermsInDoc_ = 0;
}

uint32_t PositionListBuilder::write(DataWriter& writer) const {
  return writer.write(positions_.begin(), positions_.size());
}
