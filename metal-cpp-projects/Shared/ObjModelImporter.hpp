//
//  ObjImporter.cpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#include "ModelImporter.hpp"
#include <CoreFoundation/CFBundle.h>
#include <string>

class ObjModelImporter : public ModelImporter
{
  public:
	/**
	 Import .obj file as a model.
	 @param resourceName - .obj file name
	 @param resourceType - file extension (default is obj)
	 */
	Model * const import(const char * resourceName, const char * resourceType = "obj") const override;
	~ObjModelImporter(){};
  
  private:
	/**
	 Get path to a resource inside the app's bundle by using C APIs from CoreFoundation.
	 @param resourceName - file name
	 @param resourceType - file extension
	 */
	inline const std::string absolutePath(const char * resourceName, const char * resourceType) const
	{
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
	  CFRelease(resourceUrlRef);
	  CFRelease(resourceTypeRef);
	  CFRelease(resourceNameRef);
	  CFRelease(allocatorRef);
	  CFRelease(bundleRef);
	  
	  return std::string(resourcePath);
	}
};
