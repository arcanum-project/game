//
//  ObjImporter.cpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#include "ModelImporter.hpp"
#include <CoreFoundation/CFBundle.h>
#include <string>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include <boost/lexical_cast.hpp>
#include "VertexData.hpp"
#include <vector>
#include <unordered_map>

class ObjModelImporter : public ModelImporter
{
  public:
	/**
	 Import .obj file as a model.
	 @param pDevice - raw pointer to Metal device. Using void * to avoid depending on Metal here
	 @param resourceName - .obj file name
	 @param resourceType - file extension (default is obj)
	 */
	Model * const import(void * const pDevice, const char * resourceName, const char * resourceType = "obj") const override;
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
  
	// Based on: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
	inline void processNonFace(std::string & line, std::vector<glm::vec3> & vertices, std::vector<glm::vec2> & textures, std::vector<glm::vec3> & normals, bool isTexture, bool isVertex) const {
	  const std::string delimiter = " ";
	  // We need this to read last piece of the data in the string.
	  // We parse the string by finding delimiter and reading the data before it.
	  line.append(delimiter);
	  size_t pos = 0;
	  std::string token {};

	  glm::vec3 vector {};
	  uint8_t iterationInd = 1;
	  while ((pos = line.find(delimiter)) != std::string::npos) {
		token = line.substr(0, pos);
		vector.x = iterationInd == 1 ? boost::lexical_cast<float_t>(token) : vector.x;
		vector.y = iterationInd == 2 ? boost::lexical_cast<float_t>(token) : vector.y;
		vector.z = iterationInd == 3 && !isTexture ? boost::lexical_cast<float_t>(token) : 0;
		// Move to the next data item inside the string
		line.erase(0, pos + delimiter.length());
		++iterationInd;
	  }
	  
	  if (isVertex)
		vertices.push_back(vector);
	  else if (isTexture)
		textures.push_back(glm::vec2(vector.x, vector.y));
	  else
		normals.push_back(vector);
	}
  
	inline void processFace(std::string & line, std::vector<VertexData> & vertexData, std::vector<uint16_t> & indices, std::unordered_map<std::string, uint16_t> & vertexToIndex, std::vector<glm::vec3> & vertices, std::vector<glm::vec2> & textures, std::vector<glm::vec3> & normals) const {
	  // Delimiter between vertices inside the face
	  const std::string faceDelimiter = " ";
	  line.append(faceDelimiter);
	  size_t facePos = 0;
	  std::string faceToken {};
	  
	  uint8_t iterationIndex = 1;
	  while ((facePos = line.find(faceDelimiter)) != std::string::npos) {
		if (iterationIndex > 3)
		  // Means that .obj file has rectangular faces
		  throw std::runtime_error("Non-triangular faces are not currently supported.");
		faceToken = line.substr(0, facePos);
		processFaceVertex(faceToken, vertexData, indices, vertexToIndex, vertices, textures, normals);
		line.erase(0, facePos + faceDelimiter.length());
		++iterationIndex;
	  }
	}
  
  inline void processFaceVertex(std::string & line, std::vector<VertexData> & vertexData, std::vector<uint16_t> & indices, std::unordered_map<std::string, uint16_t> & vertexToIndex, std::vector<glm::vec3> & vertices, std::vector<glm::vec2> & textures, std::vector<glm::vec3> & normals) const {
	VertexData vd = VertexData();
	uint16_t vertexIndex = 0;
	std::string vertexToIndexKey = line;
	
	const std::string vertexDelimiter = "/";
	line.append(vertexDelimiter);
	size_t vertexPos = 0;
	std::string vertexToken {};
	
	uint8_t iterationIndex = 1;
	while ((vertexPos = line.find(vertexDelimiter)) != std::string::npos) {
	  // vertexToken is an index in either vertices or textures or normals:
	  // iterationIndex = 1 => vertex
	  // iterationIndex = 2 => texture
	  // iterationIndex = 3 => normal
	  vertexToken = line.substr(0, vertexPos);
	  if (iterationIndex == 1) {
		vertexIndex = boost::lexical_cast<uint16_t>(vertexToken) - 1;
		const glm::vec3 vertex = vertices.at(vertexIndex);
		vd.vertex = vertex;
	  } else if (iterationIndex == 2) {
		const glm::vec2 texture = textures.at(boost::lexical_cast<float_t>(vertexToken) - 1);
		vd.texture = texture;
	  } else if (iterationIndex == 3) {
		const glm::vec3 normal = normals.at(boost::lexical_cast<float_t>(vertexToken) - 1);
		vd.normal = normal;
	  } else {
		throw std::runtime_error("Expected a vertex to only have 3 attributes");
	  }
	  
	  line.erase(0, vertexPos + vertexDelimiter.length());
	  ++iterationIndex;
	}
	
	std::unordered_map<std::string,uint16_t>::const_iterator indexedVertex = vertexToIndex.find(vertexToIndexKey);
	if (indexedVertex != vertexToIndex.end()) {
	  indices.push_back(indexedVertex->second);
	} else {
	  vertexData.push_back(vd);
	  indices.push_back(vertexData.size() - 1);
	  vertexToIndex.insert(std::make_pair(vertexToIndexKey, vertexData.size() - 1));
	}
  }
  
  inline void indexVertex(std::unordered_map<char *, uint16_t> &vertexDescriptorToIndex, const glm::vec3 &vertex) const {
	
  }
};
