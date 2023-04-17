//

#if __APPLE__
#	include <TargetConditionals.h>
#	if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#		define GLES_SILENCE_DEPRECATION
#		include <SDL_opengles2.h>
#	else
#		include <SDL_opengl.h>
#	endif
#elif __ANDROID__
#	include <SDL_opengles2.h>
#else
#	include <SDL_opengl.h>
#endif

#include "GLRenderer.h"

GLRenderer::GLRenderer(SDL_Window* window)
: window(window),
  glContext(nullptr)
{
#if __IPHONEOS__ || __ANDROID__
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
  
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  
  glContext = SDL_GL_CreateContext(window);
}

GLRenderer::~GLRenderer()
{
  SDL_GL_DeleteContext(glContext);
}

void GLRenderer::drawFrame()
{
  static float time = 0.f;
  glClearColor(tan(time), sin(time), cos(time), 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(window);
  time += 0.001;
}
