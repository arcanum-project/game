//

#pragma once

// Set this to 1 to always use OpenGL/OpenGLES
#define DEBUG_FORCE_OPENGL 0

#if DEBUG_FORCE_OPENGL || !defined(__APPLE__)
#	include "GLRenderer.h"
	typedef GLRenderer PlatformRenderer;
#else
#	include "Renderer.hpp"
	typedef Renderer PlatformRenderer;
#endif
