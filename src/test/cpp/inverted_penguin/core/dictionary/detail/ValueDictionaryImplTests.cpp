#include <inverted_penguin/core/dictionary/detail/ValueDictionaryImpl.hpp>
#include <inverted_penguin/core/dictionary/detail/ValueDictionaryKey.hpp>
#include <pistis/testing/Iterators.hpp>
#include <gtest/gtest.h>

#include <vector>

#include <stdint.h>
#include <string.h>

using namespace inverted_penguin::core::dictionary::detail;
namespace pt = pistis::testing;
namespace pti = pistis::testing::iterators;

namespace {

  // Keys and their corresponding values:
  //   apple     10
  //   big       20
  //   bigger    30
  //   biggest   40
  //   cow       50
  //   crisp     60
  //   dog       70
  //   penguin   80
  //   storm     90
  //   thorn    100
  alignas(4) static const uint8_t DICTIONARY_DATA[] = {
      0x0a, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
      0x0d, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00,
      0x1c, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00,
      0x2b, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
      0x14, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
      0x32, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00,
      0x50, 0x00, 0x00, 0x00, 0x5a, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00,
      0x61, 0x70, 0x70, 0x6c, 0x65, 0x62, 0x69, 0x67, 0x62, 0x69, 0x67, 0x67,
      0x65, 0x72, 0x62, 0x69, 0x67, 0x67, 0x65, 0x73, 0x74, 0x63, 0x6f, 0x77,
      0x63, 0x72, 0x69, 0x73, 0x70, 0x64, 0x6f, 0x67, 0x70, 0x65, 0x6e, 0x67,
      0x75, 0x69, 0x6e, 0x73, 0x74, 0x6f, 0x72, 0x6d, 0x74, 0x68, 0x6f, 0x72,
      0x6e, 0x00, 0x00, 0x00,
  };

  alignas(4) static const uint8_t EMPTY_DICTIONARY[] = {
      0x00, 0x00, 0x00, 0x00
  };

  static const char* TRUE_KEYS[] = {
      "apple", "big", "bigger", "biggest", "cow", "crisp", "dog",
      "penguin", "storm", "thorn"
  };
  static const uint32_t NUM_TRUE_KEYS = sizeof(TRUE_KEYS)/sizeof(const char*);
  static const char** const END_OF_TRUE_KEYS = TRUE_KEYS + NUM_TRUE_KEYS;

  static const uint32_t TRUE_VALUES[] =
      { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
  static const uint32_t NUM_TRUE_VALUES = sizeof(TRUE_VALUES)/sizeof(uint32_t);
  static const uint32_t* const END_OF_TRUE_VALUES =
      TRUE_VALUES + NUM_TRUE_VALUES;

  static std::vector<ValueDictionaryImpl<uint32_t>::Key> makeTrueKeys() {
    std::vector<ValueDictionaryKey> keys;
    
    for (const char** p = TRUE_KEYS; p < END_OF_TRUE_KEYS; ++p) {
      keys.emplace_back((const uint8_t*)*p,
			(const uint8_t*)*p + ::strlen(*p));
    }

    return std::move(keys);
  }

}

TEST(ValueDictionaryImplTests, Construct) {
  ValueDictionaryImpl<uint32_t> dictionary(DICTIONARY_DATA);

  EXPECT_EQ(DICTIONARY_DATA, dictionary.base());
  EXPECT_EQ(10, dictionary.size());
}

TEST(ValueDictionaryImplTests, ConstructByMove) {
  ValueDictionaryImpl<uint32_t> dictionary(DICTIONARY_DATA);
  std::vector<ValueDictionaryKey> trueKeys = makeTrueKeys();

  ASSERT_EQ(DICTIONARY_DATA, dictionary.base());
  ASSERT_EQ(10, dictionary.size());

  {
    ValueDictionaryImpl<uint32_t> copy(std::move(dictionary));
    ASSERT_EQ(DICTIONARY_DATA, copy.base());
    ASSERT_EQ(10, copy.size());

    for (uint32_t i = 0; i < copy.size(); ++i) {
      EXPECT_EQ(trueKeys[i], copy.keyAt(i));
      EXPECT_EQ(TRUE_VALUES[i], copy.valueAt(i));
    }

    EXPECT_EQ((const uint8_t*)0, dictionary.base());
  }
}

TEST(ValueDictionaryImplTests, AssignByMove) {
  ValueDictionaryImpl<uint32_t> dictionary(DICTIONARY_DATA);
  ValueDictionaryImpl<uint32_t> copy(EMPTY_DICTIONARY);
  std::vector<ValueDictionaryKey> trueKeys = makeTrueKeys();

  ASSERT_EQ(DICTIONARY_DATA, dictionary.base());
  ASSERT_EQ(10, dictionary.size());
  ASSERT_EQ(EMPTY_DICTIONARY, copy.base());
  ASSERT_EQ(0, copy.size());

  {
    copy = std::move(dictionary);

    ASSERT_EQ(DICTIONARY_DATA, copy.base());
    ASSERT_EQ(10, copy.size());

    for (uint32_t i = 0; i < copy.size(); ++i) {
      EXPECT_EQ(trueKeys[i], copy.keyAt(i));
      EXPECT_EQ(TRUE_VALUES[i], copy.valueAt(i));
    }

    EXPECT_EQ((const uint8_t*)0, dictionary.base());    
  }
}

TEST(ValueDictionaryImplTests, KeyAt) {
  ValueDictionaryImpl<uint32_t> dictionary(DICTIONARY_DATA);
  auto trueKeys = makeTrueKeys();

  ASSERT_EQ(trueKeys.size(), dictionary.size());
  for (auto i = 0; i < dictionary.size(); ++i) {
    EXPECT_EQ(trueKeys[i], dictionary.keyAt(i));
  }
}

TEST(ValueDictionaryImplTests, ValueAt) {
  ValueDictionaryImpl<uint32_t> dictionary(DICTIONARY_DATA);

  ASSERT_EQ(NUM_TRUE_VALUES, dictionary.size());
  for (auto i = 0; i < dictionary.size(); ++i) {
    EXPECT_EQ(TRUE_VALUES[i], dictionary.valueAt(i));
  }
}

TEST(ValueDictionaryImplTests, VerifyIterator) {
  ValueDictionaryImpl<uint32_t> dictionary(DICTIONARY_DATA);
  std::vector<ValueDictionaryKey> trueKeys = makeTrueKeys();

  ASSERT_EQ(trueKeys.size(), NUM_TRUE_VALUES);

  pti::testEqualityOp(dictionary.end(), dictionary.end(), dictionary.begin());
  pti::testInequalityOp(dictionary.end(), dictionary.end(),
			dictionary.begin());
  {
    ValueDictionaryImpl<uint32_t>::Iterator it(dictionary.begin());
    ValueDictionaryImpl<uint32_t>::Iterator copy(it);

    EXPECT_TRUE(it == copy);
  }

  {
    ValueDictionaryImpl<uint32_t>::Iterator it(dictionary.begin());
    ValueDictionaryImpl<uint32_t>::Iterator copy(dictionary.end());

    EXPECT_FALSE(it == copy);

    copy = it;
    EXPECT_TRUE(it == copy);
  }

  
  uint32_t i = 0;
  ValueDictionaryImpl<uint32_t>::Iterator p;
  for (p = dictionary.begin(); p != dictionary.end(); ++p, ++i) {
    EXPECT_EQ(trueKeys[i], (*p).key);
    EXPECT_EQ(TRUE_VALUES[i], (*p).value);
  }

  i = 0;
  p = dictionary.begin();
  while (p != dictionary.end()) {
    auto kv = *p++;
    EXPECT_EQ(trueKeys[i], kv.key);
    EXPECT_EQ(TRUE_VALUES[i], kv.value);
    ++i;
  }  
}

TEST(ValueDictionaryImplTests, VerifyKeyIterator) {
  ValueDictionaryImpl<uint32_t> dictionary(DICTIONARY_DATA);
  std::vector<ValueDictionaryKey> trueKeys = makeTrueKeys();
  auto createIteratorAtStart = [&dictionary]() {
    return ValueDictionaryImpl<uint32_t>::KeyIterator(dictionary.begin());
  };
  auto createIteratorAtEnd = [&dictionary]() {
    return ValueDictionaryImpl<uint32_t>::KeyIterator(dictionary.end());
  };

  pti::testForwardIterator(createIteratorAtStart, createIteratorAtEnd,
			   trueKeys);
}

TEST(ValueDictionaryImplTests, VerifyValueIterator) {
  ValueDictionaryImpl<uint32_t> dictionary(DICTIONARY_DATA);
  std::vector<uint32_t> trueValues(TRUE_VALUES, END_OF_TRUE_VALUES);  
  auto createIteratorAtStart = [&dictionary]() {
    return ValueDictionaryImpl<uint32_t>::ValueIterator(dictionary.begin());
  };
  auto createIteratorAtEnd = [&dictionary]() {
    return ValueDictionaryImpl<uint32_t>::ValueIterator(dictionary.end());
  };

  pti::testForwardIterator(createIteratorAtStart, createIteratorAtEnd,
			   trueValues);
}

TEST(ValueDictionaryImplTests, PrefixMatchingOneValue) {
  ValueDictionaryImpl<uint32_t> dictionary(DICTIONARY_DATA);
  ValueDictionaryImpl<uint32_t>::Iterator i, j;
  auto trueKeys = makeTrueKeys();
  const char prefix[] = "cris";

  std::tie(i, j) =
      dictionary.prefix((const uint8_t*)prefix,
			(const uint8_t*)prefix + sizeof(prefix) - 1);
  ASSERT_NE(i, j);
  EXPECT_EQ(trueKeys[5], (*i).key);
  EXPECT_EQ(TRUE_VALUES[5], (*i).value);

  ++i;
  EXPECT_EQ(i, j);
}

TEST(ValueDictionaryImplTests, PrefixMatchingManyValues) {
  ValueDictionaryImpl<uint32_t> dictionary(DICTIONARY_DATA);
  ValueDictionaryImpl<uint32_t>::Iterator i, j;
  auto trueKeys = makeTrueKeys();
  const char prefix[] = "big";

  std::tie(i, j) =
      dictionary.prefix((const uint8_t*)prefix,
			(const uint8_t*)prefix + sizeof(prefix) - 1);
  ASSERT_NE(i, j);
  EXPECT_EQ(trueKeys[1], (*i).key);
  EXPECT_EQ(TRUE_VALUES[1], (*i).value);

  ++i;
  ASSERT_NE(i, j);
  EXPECT_EQ(trueKeys[2], (*i).key);
  EXPECT_EQ(TRUE_VALUES[2], (*i).value);

  ++i;
  ASSERT_NE(i, j);
  EXPECT_EQ(trueKeys[3], (*i).key);
  EXPECT_EQ(TRUE_VALUES[3], (*i).value);

  ++i;
  EXPECT_EQ(i, j);
}

TEST(ValueDictionaryImplTests, PrefixMatchingNoValues) {
  ValueDictionaryImpl<uint32_t> dictionary(DICTIONARY_DATA);
  ValueDictionaryImpl<uint32_t>::Iterator i, j;
  auto trueKeys = makeTrueKeys();
  const char prefix[] = "cu";

  std::tie(i, j) =
      dictionary.prefix((const uint8_t*)prefix,
			(const uint8_t*)prefix + sizeof(prefix) - 1);
  EXPECT_EQ(i, j);
}

TEST(ValueDictionaryImplTests, PrefixMatchingNoValuesKeyTooLong) {
  ValueDictionaryImpl<uint32_t> dictionary(DICTIONARY_DATA);
  ValueDictionaryImpl<uint32_t>::Iterator i, j;
  auto trueKeys = makeTrueKeys();
  const char prefix[] = "doggi";

  std::tie(i, j) =
      dictionary.prefix((const uint8_t*)prefix,
			(const uint8_t*)prefix + sizeof(prefix) - 1);
  EXPECT_EQ(i, j);
}

TEST(ValueDictionaryImplTests, PrefixMatchingNoValuesIncorrectSuffix) {
  ValueDictionaryImpl<uint32_t> dictionary(DICTIONARY_DATA);
  ValueDictionaryImpl<uint32_t>::Iterator i, j;
  auto trueKeys = makeTrueKeys();
  const char prefix[] = "stou";

  std::tie(i, j) =
      dictionary.prefix((const uint8_t*)prefix,
			(const uint8_t*)prefix + sizeof(prefix) - 1);
  EXPECT_EQ(i, j);
}
