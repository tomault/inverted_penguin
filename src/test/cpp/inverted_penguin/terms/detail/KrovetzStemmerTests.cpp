#include <inverted_penguin/tokens/detail/KrovetzStemmer.hpp>
#include <gtest/gtest.h>

#include "OriginalKrovetzStemmer.hpp"

#include <exception>
#include <fstream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>

#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
using namespace inverted_penguin::tokens::detail;

namespace {
  std::string computeResourcesDir() {
    const char* dirName = getenv("INVERTED_PENGUIN_TEST_RESOURCES");
    if (dirName) {
      std::string dir = std::string(dirName);
      auto i = dir.find_last_not_of("/");
      if (i == std::string::npos) {
	dir = std::string(".");
      } else {
	dir = dir.substr(0, i + 1);
      }
      return dir;
    } else {
      return std::string(".");
    }
  }
  
  const std::string& getResourcesDir() {
    static const std::string RESOURCES_DIR = computeResourcesDir();
    return RESOURCES_DIR;
  }

  std::string joinPaths(const std::string& before, const std::string& after) {
    return before + "/" + after;
  }

  std::string computeWordsFile() {
    const char* filename = getenv("INVERTED_PENGUIN_KSTEM_WORDS_FILE");
    return joinPaths(getResourcesDir(),
		     filename ? filename : "words_alpha.txt");
  }
  
  const std::string& getWordsFile() {
    static const std::string WORDS_FILE = computeWordsFile();
    return WORDS_FILE;
  }
  
  class WordsFileReader {
  public:
    WordsFileReader(const std::string& filename) : filename_(filename) { }

    std::string next() {
      if (!input_) {
	input_ = std::unique_ptr<std::ifstream>(
	    new std::ifstream(filename_.c_str())
	);
	if (input_->fail()) {
	  std::ostringstream msg;
	  msg << "Failed to open " << filename_ << " (" << strerror(errno)
	      << ")";
	  throw std::runtime_error(msg.str());
	}
      }

      std::string text;
      while (text.empty() && std::getline(*input_, text)) {
	auto i = text.size();
	while ((i > 0) && isspace(text[i - 1])) {
	  --i;
	}
	text = text.substr(0, i);
      }

      return text;
    }

  private:
    std::string filename_;
    std::unique_ptr<std::ifstream> input_;
  };
}

TEST(KrovetzStemmerTests, VerifyAgainstOriginal) {
  static const uint32_t MAX_ERRORS = 10;
  KrovetzStemmer stemmer;
  stem::KrovetzStemmer original;
  WordsFileReader words(getWordsFile());
  uint32_t numErrors = 0;
  std::string word;
  char buffer[KrovetzStemmer::MAX_WORD_LENGTH * 2];
  char wordText[KrovetzStemmer::MAX_WORD_LENGTH * 2 + 1];
  std::ostringstream msg;

  while ((word = words.next()).size()) {
    // Skip really long words in the file so we can work with bounded buffer
    // space
    if (word.size() <= (KrovetzStemmer::MAX_WORD_LENGTH * 2)) {
      strcpy(wordText, word.c_str());
      size_t n = stemmer.kstem_stem_tobuffer(wordText, buffer);
      std::string stemmedWord = n ? std::string(buffer, n - 1) : word;

      strcpy(wordText, word.c_str());
      n = original.kstem_stem_tobuffer(wordText, buffer);
      std::string originalStemmedWord =
	  n ? std::string(buffer, n - 1) : std::string(wordText);
      if (stemmedWord != originalStemmedWord) {
	if (!numErrors) {
	  msg << "The original and current Krovetz stemmer implementations "
	      << "differ on the\nfollowing words (original, new):\n";
	} else if (numErrors == MAX_ERRORS) {
	  msg << "    ...\n";
	  break;
	} else {
	  msg << "    " << originalStemmedWord << " " << stemmedWord << "\n";
	  ++numErrors;
	}
      }
    }
  }

  EXPECT_EQ(0, numErrors) << msg.str();
}
