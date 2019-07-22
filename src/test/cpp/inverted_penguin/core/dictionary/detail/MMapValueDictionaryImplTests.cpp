#include <inverted_penguin/core/dictionary/detail/MMapValueDictionaryImpl.hpp>
#include <pistis/filesystem/Path.hpp>
#include <pistis/memory/MMapPtr.hpp>
#include <pistis/testing/Resources.hpp>
#include <gtest/gtest.h>

#include <vector>
#include <stdint.h>

using namespace inverted_penguin::core::dictionary::detail;
namespace pm = pistis::memory;
namespace pp = pistis::filesystem::path;
namespace pt = pistis::testing;

namespace {
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

  static std::vector<MMapValueDictionaryImpl<uint32_t>::Key> makeTrueKeys() {
    std::vector<MMapValueDictionaryImpl<uint32_t>::Key> keys;
    
    for (const char** p = TRUE_KEYS; p < END_OF_TRUE_KEYS; ++p) {
      keys.emplace_back((const uint8_t*)*p,
			(const uint8_t*)*p + ::strlen(*p));
    }

    return std::move(keys);
  }  
}

TEST(MMapValueDictionaryImplTests, Construct) {
  std::string dataFile = pt::getResourcePath("ten_keys_data");
  uint64_t dataFileSize = pistis::filesystem::path::fileSize(dataFile);
  ASSERT_GT(dataFileSize, 0);

  MMapValueDictionaryImpl<uint32_t> dictionary(
      pm::MMapPtr<uint8_t>::ofFile(dataFile, 0, dataFileSize)
  );
  auto trueKeys = makeTrueKeys();
  std::vector<uint32_t> trueValues(TRUE_VALUES, TRUE_VALUES + NUM_TRUE_VALUES);
  std::vector<MMapValueDictionaryImpl<uint32_t>::Key> keys;
  std::vector<uint32_t> values;

  EXPECT_EQ(10, dictionary.size());
  for (auto kv : dictionary) {
    keys.push_back(kv.key);
    values.push_back(kv.value);
  }

  EXPECT_EQ(trueKeys, keys);
  EXPECT_EQ(trueValues, values);
}
