//
//  MemoryAlignment.h
//  metal-cpp-projects
//
//  Created by Dmitrii Belousov on 8/11/22.
//

#ifndef MemoryAlignment_h
#define MemoryAlignment_h

#include <stdio.h>

/**
 Utilities for memory alignment
 */
class MemoryAlignment {
public:
  // From here: https://stackoverflow.com/questions/54930382/is-the-glm-math-library-compatible-with-apples-metal-shading-language
  static inline size_t roundUpToNextMultipleOf16(size_t val) {
	const size_t remainder = val % 16;
	if (remainder == 0) {
	  return val;
	}
	return val + 16 - remainder;
  }
};

#endif /* MemoryAlignment_h */
