#ifndef __INVERTED_PENGUIN__EXCEPTIONS__INVERTEDPENGUINEXCEPTION_HPP__
#define __INVERTED_PENGUIN__EXCEPTIONS__INVERTEDPENGUINEXCEPTION_HPP__

#include <pistis/exceptions/PistisException.hpp>
#include <string>

namespace inverted_penguin {
  namespace exceptions {

    class InvertedPenguinException :
        public pistis::exceptions::PistisException {
    public:
      InvertedPenguinException(const std::string& details);
      InvertedPenguinException(
	  const std::string& details,
	  const pistis::exceptions::ExceptionOrigin& origin
      );
      InvertedPenguinException(const InvertedPenguinException&) = default;
      InvertedPenguinException(InvertedPenguinException&&) = default;
      virtual ~InvertedPenguinException();

      InvertedPenguinException& operator=(const InvertedPenguinException&) =
          default;
      InvertedPenguinException& operator=(InvertedPenguinException&) =
          default;
      
    private:
      std::string details_;
    };

  }
}

#endif

