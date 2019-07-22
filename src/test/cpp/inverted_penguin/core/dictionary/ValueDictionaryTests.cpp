#include <inverted_penguin/core/dictionary/ValueDictionary.hpp>
#include <pistis/testing/Iterators.hpp>
#include <gtest/gtest.h>

using namespace inverted_penguin::core::dictionary;
using inverted_penguin::core::Optional;
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

  static std::vector<UnmanagedValueDictionary<uint32_t>::Key> makeTrueKeys() {
    std::vector<UnmanagedValueDictionary<uint32_t>::Key> keys;
    
    for (const char** p = TRUE_KEYS; p < END_OF_TRUE_KEYS; ++p) {
      keys.emplace_back((const uint8_t*)*p,
			(const uint8_t*)*p + ::strlen(*p));
    }

    return std::move(keys);
  }

}

TEST(ValueDictionaryTests, Construct) {
  UnmanagedValueDictionary<uint32_t> dictionary(DICTIONARY_DATA);
  auto trueKeys = makeTrueKeys();
  std::vector<uint32_t> trueValues(TRUE_VALUES, END_OF_TRUE_VALUES);
  std::vector<UnmanagedValueDictionary<uint32_t>::Key> keys;
  std::vector<uint32_t> values;

  EXPECT_EQ(10, dictionary.size());
  for (auto kv : dictionary) {
    keys.push_back(kv.key);
    values.push_back(kv.value);
  }

  EXPECT_EQ(trueKeys, keys);
  EXPECT_EQ(trueValues, values);
}

TEST(ValueDictionaryTests, Find) {
  UnmanagedValueDictionary<uint32_t> dictionary(DICTIONARY_DATA);
  const char key[] = "bigger";
  const char unknown[] = "doggy";

  UnmanagedValueDictionary<uint32_t>::ConstIterator i =
      dictionary.find((const uint8_t*)key,
		      (const uint8_t*)key + sizeof(key) - 1);

  auto target = UnmanagedValueDictionary<uint32_t>::Key(
      (const uint8_t*)key, (const uint8_t*)key + sizeof(key) - 1
  );
  ASSERT_NE(i, dictionary.end());
  EXPECT_EQ(target, (*i).key);
  EXPECT_EQ(30, (*i).value);
  
  i = dictionary.find((const uint8_t*)unknown,
		      (const uint8_t*)unknown + sizeof(unknown) - 1);
  EXPECT_EQ(dictionary.end(), i);
}

TEST(ValueDictionaryTests, Prefix) {
  UnmanagedValueDictionary<uint32_t> dictionary(DICTIONARY_DATA);
  const char key[] = "big";  
  const char unknown[] = "doggy";
  UnmanagedValueDictionary<uint32_t>::ConstIterator i, j;
  auto trueKeys = makeTrueKeys();
  std::vector<uint32_t> trueValues(TRUE_VALUES, END_OF_TRUE_VALUES);

  std::tie(i, j) = dictionary.prefix((const uint8_t*)key,
				     (const uint8_t*)key + sizeof(key) - 1);
  ASSERT_NE(i, j);
  EXPECT_EQ(trueKeys[1], (*i).key);
  EXPECT_EQ(trueValues[1], (*i).value);

  ++i;
  ASSERT_NE(i, j);
  EXPECT_EQ(trueKeys[2], (*i).key);
  EXPECT_EQ(trueValues[2], (*i).value);

  ++i;
  ASSERT_NE(i, j);
  EXPECT_EQ(trueKeys[3], (*i).key);
  EXPECT_EQ(trueValues[3], (*i).value);

  ++i;
  EXPECT_EQ(i, j);

  std::tie(i, j) =
      dictionary.prefix((const uint8_t*)unknown,
			(const uint8_t*)unknown + sizeof(unknown) - 1);
  EXPECT_EQ(i, j);
}

TEST(ValueDictionaryTests, Get) {
  UnmanagedValueDictionary<uint32_t> dictionary(DICTIONARY_DATA);
  const char key[] = "cow";
  const char unknown[] = "moo";
  Optional<uint32_t> v = dictionary.get((const uint8_t*)key,
					(const uint8_t*)key + sizeof(key) - 1);
  ASSERT_TRUE(v.present());
  EXPECT_EQ(50, v.value());

  v = dictionary.get((const uint8_t*)unknown,
		     (const uint8_t*)unknown + sizeof(unknown) - 1);
  EXPECT_TRUE(v.empty());  
}
