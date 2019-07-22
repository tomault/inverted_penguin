/** @file Unit tests for unicode utilities */
#include <inverted_penguin/terms/UnicodeUtils.hpp>
#include <gtest/gtest.h>
#include <iomanip>
#include <sstream>
#include <vector>

using namespace inverted_penguin::exceptions;
using namespace inverted_penguin::terms;

TEST(UnicodeUtilsTests, DecodeUtf8) {
  const unsigned char TEXT[] = { 0x41,
				 0xD5, 0xB9,
				 0xEF, 0xA5, 0x90,
				 0xF1, 0x80, 0xBF, 0xAA,
				 0xFA, 0x8C, 0x93, 0xA7, 0xB8,
				 0xFD, 0x88, 0x99, 0xAA, 0xBB, 0x90 };
  const uint32_t TRUTH[] = { 1, 0x41, 2, 0x579, 3, 0xF950, 4, 0x40FEA,
			     5, 0x23139F8, 6, 0x4866AED0 };
  const uint32_t NUM_TRUTH = sizeof(TRUTH)/(2 * sizeof(uint32_t));
  const unsigned char *p = TEXT;
  const unsigned char *next = nullptr;
  uint32_t c = 0;

  for (uint32_t i = 0; i < (NUM_TRUTH * 2); i += 2) {
    std::tie(c, next) = decodeUtf8(p, p + TRUTH[i]);
    ASSERT_EQ(TRUTH[i + 1], c);
    ASSERT_EQ(TRUTH[i], next - p);
    p = next;
  }
}

TEST(UnicodeUtilsTests, DecodeIllegalUtf8) {
  const unsigned char TEXT[] = { 0x95,  ///< Illegal start byte
				 0xFE,  ///< Also illegal start byte
				 0xEF, 0xA5, 0x41, ///< Illegal trailing byte
				 0xEF, 0xA5, 0xC0, ///< Also illegal trailing byte
				 0xEF, 0xA5, 0x90 ///< Legal, but will end prematurely
                               };

  EXPECT_THROW(decodeUtf8((const unsigned char*)TEXT, TEXT + 1),
	       Utf8DecodeError);
  EXPECT_THROW(decodeUtf8(TEXT + 1, TEXT + 2), Utf8DecodeError);
  EXPECT_THROW(decodeUtf8(TEXT + 2, TEXT + 5), Utf8DecodeError);
  EXPECT_THROW(decodeUtf8(TEXT + 5, TEXT + 8), Utf8DecodeError);
  EXPECT_THROW(decodeUtf8(TEXT + 8, TEXT + 10), Utf8DecodeError);

  // Empty sequence is a decode error
  EXPECT_THROW(decodeUtf8(TEXT + 8, TEXT + 8), Utf8DecodeError);
}

TEST(UnicodeUtilsTests, EncodeUtf8) {
  const uint32_t VALUES[] = { 0x41, 0x579, 0xF950, 0x40FEA, 0x23139F8,
			      0x4866AED0 };
  const unsigned char TRUTH[] = { 1, 0x41, 2, 0xD5, 0xB9, 3, 0xEF, 0xA5, 0x90,
				  4, 0xF1, 0x80, 0xBF, 0xAA,
				  5, 0xFA, 0x8C, 0x93, 0xA7, 0xB8,
				  6, 0xFD, 0x88, 0x99, 0xAA, 0xBB, 0x90 };
  const uint32_t NUM_VALUES = sizeof(VALUES)/sizeof(uint32_t);
  const uint32_t* p = VALUES;
  const unsigned char* truth = TRUTH;
  std::vector<uint8_t> buffer;

  // Verify encoding now.  Check return value later.
  for (const uint32_t* p = VALUES; p != VALUES + NUM_VALUES; ++p) {
    uint32_t true_length = *truth;
    const unsigned char *true_data = truth + 1;

    buffer.clear();
    auto end = encodeUtf8(std::back_inserter(buffer), *p);
    
    ASSERT_EQ(true_length, buffer.size());
    for (auto i = buffer.begin(); i != buffer.end(); ++i, ++true_data) {
      if (*i != *true_data) {
	std::ostringstream msg;
	msg << "Encoding for 0x" << std::hex << *p
	    << " is incorrect.  It was encoded as [ ";
	for (auto j = buffer.begin(); j != buffer.end(); ++j) {
	  if (j != buffer.begin()) {
	    msg <<  " ";
	  }
	  msg << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)*j;
	}
	msg << " ], but it should be [ ";
	for (auto j = truth + 1; j != (truth + true_length + 1); ++j) {
	  if (j != (truth + 1)) {
	    msg << " ";
	  }
	  msg << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)*j;
	}
	msg << " ].  Byte at index " << std::dec << (i - buffer.begin())
	    << " (0x" << std::hex << std::setw(2) << std::setfill('0')
	    << *i << ") is incorrect.";
	ASSERT_TRUE(false) << msg.str();
      }
    }
    truth = true_data;    
  }

  // Since all lengths encoded correctly, this is enough
  unsigned char bytes[6];
  truth = TRUTH;
  for (const uint32_t* p = VALUES; p != VALUES + NUM_VALUES; ++p) {
    unsigned char* end = encodeUtf8(bytes, *p);

    EXPECT_EQ(*truth, end - bytes);
    truth += (*truth) + 1;
  }

  // UTF-8 can encode a maximum of 31 bits
  buffer.clear();
  EXPECT_THROW(encodeUtf8(std::back_inserter(buffer), 0x80000000),
	       Utf8EncodeError);
}

TEST(UnicodeUtilsTests, TransformUtf8Sequence) {
  const uint8_t DATA[] = { 0x41, 0x79, 0xD5, 0xB9, 0xEF, 0xA5, 0x90 };
  const uint32_t DATA_SIZE = sizeof(DATA)/sizeof(uint8_t);
  const uint8_t TRUTH[] = { 0x61, 0x79, 0xD5, 0xB9, 0x35 };
  const uint32_t TRUE_SIZE = sizeof(TRUTH)/sizeof(uint8_t);
  std::vector<uint8_t> buffer;
  auto transformer = [](uint32_t c) {
    if (c == 0x41) {
      return (uint32_t)0x61;
    } else if (c == 0xF950) {
      return (uint32_t)0x35;
    } else {
      return c;
    }
  };

  auto end = transformUtf8Sequence((const uint8_t*)DATA, DATA + DATA_SIZE,
				   std::back_inserter(buffer),
				   transformer);
  ASSERT_EQ(TRUE_SIZE, buffer.size());
  for (uint32_t i = 0; i < buffer.size(); ++i) {
    if (TRUTH[i] != buffer[i]) {
      std::ostringstream msg;
      msg << "Encoding sequence is [ ";
      for (auto j = buffer.begin(); j != buffer.end(); ++j) {
	if (j != buffer.begin()) {
	  msg <<  " ";
	}
	msg << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)*j;
      }
      msg << " ], but it should be [ ";
      for (auto j = TRUTH; j != (TRUTH + TRUE_SIZE); ++j) {
	if (j != TRUTH) {
	  msg << " ";
	}
	msg << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)*j;
      }
      msg << " ].  Byte at index " << std::dec << i << "(0x"
	  << std::hex << std::setw(2) << std::setfill('0') << buffer[i]
	  << ") is incorrect.";
      ASSERT_TRUE(false) << msg.str();
    }
  }

  // Know sequence is encoded correctly, so this is enough
  uint8_t bytes[TRUE_SIZE];
  uint8_t* seqEnd = transformUtf8Sequence((const uint8_t*)DATA,
					  DATA + DATA_SIZE, (uint8_t*)bytes,
					  transformer);
  ASSERT_EQ(TRUE_SIZE, seqEnd - bytes);
}

TEST(UnicodeUtilsTests, ToLowerCase) {
  // Scan all of LATIN-1 then select ranges from higher planes
  for (uint32_t i = 0; i < 256; ++i) {
    if (((i >= 'A') && (i <= 'Z') ||
	 ((i >= 0xC0) && (i <= 0xDE) && (i != 0xD7)))) {
      EXPECT_EQ(i + 0x20, toLowerCase(i));
    } else {
      EXPECT_EQ(i, toLowerCase(i));
    }
  }

  // TODO: Add additional Unicode ranges
}





