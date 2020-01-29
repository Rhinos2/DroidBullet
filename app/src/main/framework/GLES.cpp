//
//
//

#include "GLES.h"
#include <android/log.h>

void GLES::GLES_Init(){

//opengl state

    glHint( GL_GENERATE_MIPMAP_HINT, GL_NICEST );
 //   glHint( GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES, GL_NICEST );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE  );
    glDisable( GL_DITHER );
    glDepthMask( GL_TRUE );
    glDepthFunc( GL_LESS );
    glDepthRangef( 0.0f, 1.0f );
    glCullFace ( GL_BACK );
    glFrontFace( GL_CCW  );
    glClearStencil( 0 );
    glStencilMask( 0xFFFFFFFF );

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    __android_log_print( ANDROID_LOG_INFO, "", "\nGL_VENDOR:      %s\n", ( char * )glGetString( GL_VENDOR     ) );
    __android_log_print( ANDROID_LOG_INFO, "", "GL_RENDERER:    %s\n"  , ( char * )glGetString( GL_RENDERER   ) );
    __android_log_print( ANDROID_LOG_INFO, "", "GL_VERSION:     %s\n"  , ( char * )glGetString( GL_VERSION    ) );
    __android_log_print( ANDROID_LOG_INFO, "", "GL_EXTENSIONS:  %s\n"  , ( char * )glGetString( GL_EXTENSIONS ) );
}
