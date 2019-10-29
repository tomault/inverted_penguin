#include "DynamicTermStreamModifier.hpp"
#include "DynamicTermStreamModifierSequence.hpp"

using namespace inverted_penguin::terms;

std::shared_ptr<DynamicTermStreamModifier> inverted_penguin::terms::operator+(
    const std::shared_ptr<DynamicTermStreamModifier>& first,
    const std::shared_ptr<DynamicTermStreamModifier>& second
) {
  return std::unique_ptr<DynamicTermStreamModifier>(
      new DynamicTermStreamModifierSequence(first, second)
  );
}
