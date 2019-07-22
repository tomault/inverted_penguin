#include <inverted_penguin/terms/IteratorRangeTermStream.hpp>
#include <gtest/gtest.h>

#include <ostream>
#include <string>
#include <vector>

using namespace inverted_penguin::terms;

namespace inverted_penguin {
  namespace terms {

    inline std::ostream& operator<<(std::ostream& out, const Term& t) {
      return out << "(\"" << t.text << ", " << t.position << ")";
    }

  }
}

TEST(IteratorRangeTermStreamTests, IterateOverRange) {
  const std::vector<std::string> WORDS{ "Penguins", "are", "cute" };
  const std::vector<Term> TRUTH{
    { "Penguins", 0 }, { "are", 1 }, { "cute", 2 }
  };
  std::vector<Term> terms;
  IteratorRangeTermStream< std::vector<std::string>::const_iterator > stream(
      WORDS.begin(), WORDS.end()
  );
  Term t;

  while ((t = stream.next()).notEmpty()) {
    terms.push_back(t);
  }
  EXPECT_EQ(TRUTH, terms);
}

TEST(IteratorRangeTermStreamTests, IterateOverEmptyRange) {
  const std::vector<std::string> WORDS{ };
  const std::vector<Term> TRUTH{ };
  std::vector<Term> terms;
  IteratorRangeTermStream< std::vector<std::string>::const_iterator > stream(
      WORDS.begin(), WORDS.end()
  );
  Term t;

  while ((t = stream.next()).notEmpty()) {
    terms.push_back(t);
  }
  EXPECT_EQ(TRUTH, terms);
}

TEST(IteratorRangeTermStreamTests, Reset) {
  const std::vector<std::string> WORDS{ "Penguins", "are", "cute" };
  const std::vector<Term> TRUTH{
    { "Penguins", 0 }, { "are", 1 }, { "cute", 2 }
  };
  std::vector<Term> terms;
  IteratorRangeTermStream< std::vector<std::string>::const_iterator > stream(
      WORDS.begin(), WORDS.end()
  );
  Term t;

  while ((t = stream.next()).notEmpty()) {
    terms.push_back(t);
  }
  EXPECT_EQ(TRUTH, terms);

  EXPECT_TRUE(stream.next().isEmpty());

  terms.clear();
  stream.reset();
  while ((t = stream.next()).notEmpty()) {
    terms.push_back(t);
  }
  EXPECT_EQ(TRUTH, terms);  
}
