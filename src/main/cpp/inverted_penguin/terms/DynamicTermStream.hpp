#ifndef __INVERTED_PENGUIN__TERMS__DYNAMICTERMSTREAM_HPP__
#define __INVERTED_PENGUIN__TERMS__DYNAMICTERMSTREAM_HPP__

#include <pistis/memory/PtrUtils.hpp>
#include <inverted_penguin/terms/TermStream_.hpp>
#include <inverted_penguin/terms/Term.hpp>
#include <memory>

namespace inverted_penguin {
  namespace terms {
    class DynamicTermStreamModifier;

    class DynamicTermStream : public TermStream<DynamicTermStream> {
    public:
      /** @brief Return the next term in the stream, or the the empty term
       *         if the stream has reached its end.
       *
       *  A DynamicTermStream produces the empty token if and only if the
       *  stream has reached its end.  Reading past the stream end produces
       *  more empty tokens.
       *
       *  @returns  The next term in the stream, or the empty term if the
       *                stream has reached its end.
       */
      virtual Term next() = 0;

      /** @brief Reset the stream to its beginning.
       *
       *  @returns True if the stream was reset, false if it could not
       *               be reset.
       */
      virtual bool reset() = 0;

      /** @brief Create a copy of this term stream
       *
       *  @returns A copy of @c this
       */
      virtual DynamicTermStream* duplicate() const = 0;
    };


    std::unique_ptr<DynamicTermStream> operator+(
	std::unique_ptr<DynamicTermStream>&& stream,
	const std::shared_ptr<DynamicTermStreamModifier>& modifier
    );

    inline std::unique_ptr<DynamicTermStream> operator+(
	const std::unique_ptr<DynamicTermStream>& stream,
	const std::shared_ptr<DynamicTermStreamModifier>& modifier
    ) {
      return pistis::memory::wrapUnique(stream->duplicate()) + modifier;
    }


    inline std::unique_ptr<DynamicTermStream> operator+(
	const DynamicTermStream& stream,
	const DynamicTermStreamModifier& modifier
    );
    
  }
}
#endif
