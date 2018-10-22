#ifndef __INVERTED_PENGUIN__INVERTEDPENGUINEXCEPTION_HPP__
#define __INVERTED_PENGUIN__INVERTEDPENGUINEXCEPTION_HPP__

#include <exception>
#include <string>

namespace inverted_penguin {

  class InvertedPenguinException : public std::exception {
  public:
    InvertedPenguinException(const std::string& details);
    virtual ~InvertedPenguinException();

    virtual const char* what() const noexcept { return details_.c_str(); }

  private:
    std::string details_;
  };
  
}

#endif

