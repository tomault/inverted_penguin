#ifndef __INVERTED_PENGUIN__TERMS__DYNAMICTERMSTREAMMODIFIERWRAPPER_HPP__
#define __INVERTED_PENGUIN__TERMS__DYNAMICTERMSTREAMMODIFIERWRAPPER_HPP__

#include <inverted_penguin/terms/DynamicTermStreamModifier.hpp>
#include <inverted_penguin/terms/Term.hpp>
#include <utility>

namespace inverted_penguin {
  namespace terms {

    template <typename WrappedModifier>
    class DynamicTermStreamModifierWrapper :
      public DynamicTermStreamModifier,
      private WrappedModifier {

    public:
      DynamicTermStreamModifierWrapper(): WrappedModifier() { }

      template <typename... Args>
      DynamicTermStreamModifierWrapper(Args&&... args):
	  WrappedModifier(std::forward<Args>(args)...) {
      }
      
      DynamicTermStreamModifierWrapper(const WrappedModifier& modifier):
	  WrappedModifier(modifier) {
      }
      
      DynamicTermStreamModifierWrapper(WrappedModifier&& modifier):
	  WrappedModifier(std::move(modifier)) {
      }

      virtual bool isStateful() const override {
	return WrappedModifier::isStateful();
      }

      virtual Term next(DynamicTermStream& stream) override {
	return WrappedModifier::next(stream);
      }

      virtual Term apply(const Term& t) override {
	return WrappedModifier::apply(t);
      }

      virtual bool reset() override { return WrappedModifier::reset(); }

      virtual DynamicTermStreamModifierWrapper* duplicate() const override {
	return new DynamicTermStreamModifierWrapper(
	    static_cast<const WrappedModifier&>(*this)
	);
      }

    };
  }
}
#endif

