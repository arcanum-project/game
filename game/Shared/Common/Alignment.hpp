//
//  Alignment.h
//  metal-cpp-projects
//
//  Created by Dmitrii Belousov on 8/11/22.
//

#ifndef Alignment_h
#define Alignment_h

#include <stdio.h>

/**
 Utilities for alignment, mostly for memory.
 */
class Alignment {
public:
  // From here: https://stackoverflow.com/questions/54930382/is-the-glm-math-library-compatible-with-apples-metal-shading-language
  static inline size_t roundUpToNextMultipleOf16(size_t val) {
	const size_t remainder = val % 16;
	if (remainder == 0) {
	  return val;
	}
	return val + 16 - remainder;
  }
  
  static inline size_t roundUpToNextMultipleOf4(size_t val) {
	const size_t remainder = val % 4;
	if (remainder == 0) {
	  return val;
	}
	return val + 4 - remainder;
  }
};

#endif /* Alignment_h */
