#include "InvertedPenguinException.hpp"

using namespace inverted_penguin::exceptions;
using namespace pistis::exceptions;

InvertedPenguinException::InvertedPenguinException(
    const std::string& details
): PistisException(details) {
  // Intentionally left blank
}

InvertedPenguinException::InvertedPenguinException(
    const std::string& details,
    const ExceptionOrigin& origin
):
    PistisException(details, origin) {
}


InvertedPenguinException::~InvertedPenguinException() {
  // Intentionally left blank
}
