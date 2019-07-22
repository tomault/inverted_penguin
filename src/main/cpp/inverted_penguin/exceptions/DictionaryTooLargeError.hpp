#ifndef __INVERTED_PENGUIN__EXCEPTIONS__DICTIONARYTOOLARGEERROR_HPP__
#define __INVERTED_PENGUIN__EXCEPTIONS__DICTIONARYTOOLARGEERROR_HPP__

#include <inverted_penguin/exceptions/InvertedPenguinException.hpp>

namespace inverted_penguin {
  namespace exceptions {

    class DictionaryTooLargeError : public InvertedPenguinException {
    public:
      DictionaryTooLargeError(
	  const pistis::exceptions::ExceptionOrigin& origin
      ):
	  DictionaryTooLargeError("Dictionary too large", origin) {
      }

      DictionaryTooLargeError(
	  const std::string& details,
	  const pistis::exceptions::ExceptionOrigin& origin
      ):
	  InvertedPenguinException(details, origin) {
      }
    };
    
  }
}
#endif
