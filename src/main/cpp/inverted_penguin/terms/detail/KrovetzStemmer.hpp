/*==========================================================================
 * Copyright (c) 2005 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 *
 * Modified 2018/03/31 by Tom Ault as follows:
 * - Changed namespace to inverted_penguin::tokens::detail
 * - Changed the include guard
 * - Removed old TR1/Windows stuff.  Code is now C++11 compatible
 * - Removed cache, since it's not thread-safe and should be done outside
 *   the stemmer itself.
 * - Words that are too short, too long or contain non-alphanumeric characters
 *   now just return 0 and leave the return buffer alone.
 * - Removed char* kstem_stemmer(char*), since it is not thread-safe.
 * - The original implementation relied on instance variables j, k to
 *   maintain state across calls to private methods that implemented stemming
 *   rules.  This was not thread-safe, so these variables and all of the
 *   rules, etc were moved into a private implementation class.  One instance
 *   of this class is created in each call to kstem_stem_tobuffer and holds
 *   a reference to the dictionary, from which the implementation class just
 *   reads.  The KrovetzStemmer class itself just initializes and
 *   maintains this dictionary and performs some initial checks in
 *   kstem_stem_tobuffer.  All the actual work is done in the implementation
 *   class.
 */

// dmf
// C++ thread safe implementation of the Krovetz stemmer.
// requires no external data files.
// 07/29/2005
#ifndef __INVERTED_PENGUIN__TERMS__DETAIL__KROVETZSTEMMER_HPP__
#define __INVERTED_PENGUIN__TERMS__DETAIL__KROVETZSTEMMER_HPP__

#include <iostream>
#include <cstring>
#include <unordered_map>

namespace inverted_penguin {
  namespace terms {
    namespace detail {
      
      class KrovetzStemmer {
      public:
	/// maximum number of characters in a word to be stemmed.
	static const int MAX_WORD_LENGTH=25;

      public:
	KrovetzStemmer();
	~KrovetzStemmer();

	/*!
	  \brief stem a term using the Krovetz algorithm into the specified
	  buffer.
	  The stem returned may be longer than the input term.
	  Performs case normalization on its input argument. 
	  @param term the term to stem
	  @param buffer the buffer to hold the stemmed term. The buffer should
	  be at MAX_WORD_LENGTH or larger.
          @param alreadyLowerCase  If true, "term" is assumed to already
	      be lower case, and no lower-case transformation is done.
	  @return the number of characters written to the buffer, including
	  the terminating '\\0'. If 0, the caller should use the value in term.
	*/
	int kstem_stem_tobuffer(const char *term, char *buffer,
				bool alreadyLowerCase = false) const;
	
	/*!
	  \brief Add an entry to the stemmer's dictionary table.
	  @param variant the spelling for the entry.
	  @param word the stem to use for the variant. If "", the variant
	  stems to itself.
	  @param exc Is the word an exception to the spelling rules.
	*/
	void kstem_add_table_entry(const char* variant, const char* word, 
				   bool exc=false);
	
      private:
	/// Dictionary table entry
	struct DictEntry {
	  /// is the word an exception to stemming rules?
	  bool exception;      
	  /// stem to use for this entry.
	  const char *root;
	};

	// maint.
	void loadTables();
	
	struct eqstr {
	  bool operator()(const char* s1, const char* s2) const {
	    return strcmp(s1, s2) == 0;
	  }
	};

	typedef std::unordered_map<const char *, DictEntry,
				   std::hash<std::string>,
				   eqstr>
	        DictTable;

	DictTable dictEntries;

	// Where all the work gets done.  Every call to kstem_stem_tobuffer
	// makes a call
	class Impl;
      };

    }
  }
}
#endif
