#ifndef __INVERTED_PENGUIN__TERMS__DYNAMICTERMSTREAMMODIFIER__HPP__
#define __INVERTED_PENGUIN__TERMS__DYNAMICTERMSTREAMMODIFIER__HPP__

#include <inverted_penguin/terms/TermStreamModifier_.hpp>
#include <inverted_penguin/terms/Term.hpp>
#include <memory>

namespace inverted_penguin {
  namespace terms {

    class DynamicTermStream;
    
    class DynamicTermStreamModifier :
        public TermStreamModifier<DynamicTermStreamModifier> {
    public:
      virtual ~DynamicTermStreamModifier() { }

      virtual bool isStateful() const = 0;
      virtual Term next(DynamicTermStream& stream) = 0;
      virtual Term apply(const Term& t) = 0;
      virtual bool reset() = 0;
      virtual DynamicTermStreamModifier* duplicate() const = 0;

      static std::shared_ptr<DynamicTermStreamModifier> reuse(
	  const std::shared_ptr<DynamicTermStreamModifier>& modifier
      ) {
	return modifier->isStateful() ?
	    std::shared_ptr<DynamicTermStreamModifier>(modifier->duplicate()) :
	    modifier;
      }
    };

    std::shared_ptr<DynamicTermStreamModifier> operator+(
	const std::shared_ptr<DynamicTermStreamModifier>& first,
	const std::shared_ptr<DynamicTermStreamModifier>& second
    );
    
  }
}
#endif

