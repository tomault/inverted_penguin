#ifndef __INVERTED_PENGUIN__TERMS__DYNAMICTERMSTREAMMODIFIERSEQUENCE_HPP__
#define __INVERTED_PENGUIN__TERMS__DYNAMICTERMSTREAMMODIFIERSEQUENCE_HPP__

#include <inverted_penguin/terms/DynamicTermStream.hpp>
#include <inverted_penguin/terms/DynamicTermStreamModifier.hpp>
#include <inverted_penguin/terms/Term.hpp>
#include <iostream>
#include <memory>

namespace inverted_penguin {
  namespace terms {

    class DynamicTermStreamModifierSequence : public DynamicTermStreamModifier {
    public:
      DynamicTermStreamModifierSequence(
	  const std::shared_ptr<DynamicTermStreamModifier>& first,
	  const std::shared_ptr<DynamicTermStreamModifier>& second
      ):
	  first_(DynamicTermStreamModifier::reuse(first)),
	  second_(DynamicTermStreamModifier::reuse(second)) {
      }

      const std::shared_ptr<DynamicTermStreamModifier>& firstModifier() const {
	return first_;
      }
      
      const std::shared_ptr<DynamicTermStreamModifier>& secondModifier() const {
	return second_;
      }
      
      virtual bool isStateful() const override {
	return first_->isStateful() || second_->isStateful();
      }

      virtual Term next(DynamicTermStream& stream) override {
	Term t = stream.next();
	std::cout << "Read " << t << std::endl;
	while (t.notEmpty() && ((t = apply(t)).isEmpty())) {
	  t = stream.next();
	  std::cout << "Read " << t << std::endl;
	}
	std::cout << "Return " << t << std::endl;
	return t;
      }

      virtual Term apply(const Term& t) override {
	return second_->apply(first_->apply(t));
      }
      
      virtual bool reset() override {
	// Short-ciruit boolean logic will keep
	// "first_->reset() && second_->reset()" from calling the second
	// reset if the first fails, so execute each reset separately
	// and combine the result
	const bool firstReset = first_->reset();
	const bool secondReset = second_->reset();
	return firstReset && secondReset;
      }

      virtual DynamicTermStreamModifierSequence* duplicate() const override {
	return new DynamicTermStreamModifierSequence(
	    DynamicTermStreamModifier::reuse(first_),
	    DynamicTermStreamModifier::reuse(second_)
	);
      }

    private:
      std::shared_ptr<DynamicTermStreamModifier> first_;
      std::shared_ptr<DynamicTermStreamModifier> second_;
    };

    
  }
}
#endif

