//
//  GLPreq.h
//  Project Unknown
//
//  Created by Robert Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

// use gl3.2 profile means there's no fixed pipeline functions unless use compability context
// #define UKN_REQUEST_OPENGL_3_2_PROFILE

#ifdef UKN_OS_WINDOWS
// glew for higher opengl profile extensions

// static link
#define GLEW_STATIC

#include <glew/glew.h>

#include <GL/GL.h>
#include <GL/GLU.h>

#pragma comment(lib, "glfw.lib")

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

	// check opengl profile availability
	#if !defined(GLEW_VERSION_2_1)
		#error ukn need at least opengl 2.1 profile support to run
	#endif

#if defined(GLEW_VERSION_3_2)
	#define UKN_OPENGL_3_2
	#define UKN_OPENGL_VERSION 32
#else
	#define UKN_OPENGL_VERSION 21
#endif

#elif defined(UKN_OS_OSX)
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/glext.h>

	#include <Availability.h>
	#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_7
        #if defined(UKN_REQUEST_OPENGL_3_2_PROFILE)
            #include <OpenGL/gl3.h>
        #endif

		#define UKN_APPLE_OSX_LION
		#define UKN_OPENGL_3_2
		#define UKN_OPENGL_VERSION 32
	#else
		#define UKN_OPENGL_VERSION 21
	#endif

#elif defined(UKN_OS_LINUX) // linux still need workaround
	#include <glew/glew.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glext.h>
#endif

#include "glfw/glfw3.h"

