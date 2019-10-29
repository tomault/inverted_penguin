#include "DynamicTermStream.hpp"
#include "DynamicTermStreamModifier.hpp"
#include "DynamicModifiedTermStream.hpp"

using namespace inverted_penguin::terms;
using pistis::memory::wrapUnique;
using pistis::memory::wrapShared;

std::unique_ptr<DynamicTermStream> inverted_penguin::terms::operator+(
    std::unique_ptr<DynamicTermStream>&& stream,
    const std::shared_ptr<DynamicTermStreamModifier>& modifier
) {
  return std::unique_ptr<DynamicTermStream>(
      new DynamicModifiedTermStream(std::move(stream), modifier)
  );
}

std::unique_ptr<DynamicTermStream> operator+(
    const DynamicTermStream& stream, const DynamicTermStreamModifier& modifier
) {
  return wrapUnique(stream.duplicate()) + wrapShared(modifier.duplicate());
}
