//
//  ResourceBundle.h
//  metal-cpp-projects
//
//  Created by Dmitrii Belousov on 7/31/22.
//

#ifndef ResourceBundle_h
#define ResourceBundle_h

#include <CoreFoundation/CFBundle.h>
#include <string>

class ResourceBundle {
public:
  /**
   Get path to a resource inside the app's bundle by using C APIs from CoreFoundation.
   @param resourceName - file name
   @param resourceType - file extension
   */
  static inline const std::string absolutePath(const char * resourceName, const char * resourceType) {
	// Based on: https://stackoverflow.com/questions/2220098/using-iphone-resources-in-a-c-file
	CFBundleRef bundleRef = CFBundleGetMainBundle();
	CFAllocatorRef allocatorRef = CFAllocatorGetDefault();
	CFStringRef resourceNameRef = CFStringCreateWithCString(allocatorRef, resourceName, CFStringBuiltInEncodings(kCFStringEncodingUTF8));
	CFStringRef resourceTypeRef = CFStringCreateWithCString(allocatorRef, resourceType, CFStringBuiltInEncodings(kCFStringEncodingUTF8));
	CFURLRef resourceUrlRef = CFBundleCopyResourceURL(bundleRef,
												   resourceNameRef,
												   resourceTypeRef,
												   NULL);
	char resourcePath[PATH_MAX];
	CFURLGetFileSystemRepresentation(resourceUrlRef, true,
									 (uint8_t *)resourcePath, PATH_MAX);
	// Clean up memory
	CFRetain(resourceUrlRef);
	CFRelease(resourceUrlRef);
	
	CFRetain(resourceTypeRef);
	CFRelease(resourceTypeRef);
	
	CFRetain(resourceNameRef);
	CFRelease(resourceNameRef);
	
	CFRetain(allocatorRef);
	CFRelease(allocatorRef);
	
	CFRetain(bundleRef);
	CFRelease(bundleRef);
	
	return std::string(resourcePath);
  }
};

#endif /* ResourceBundle_h */
