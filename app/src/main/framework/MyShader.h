//
//
//

#ifndef CLEAN23_MYSHADER_H_H
#define CLEAN23_MYSHADER_H_H

#include "GLES3/gl3.h"
#include "GLES3/gl3ext.h"
#include <string.h>

struct MYSHADER
{
    GLchar	name[256];
    GLuint 	type;
    GLuint	sid;
    MYSHADER(){
        type = 0;
        sid = 0;
    }
    MYSHADER(GLchar* nm ){
        strcpy(name, nm);
        type = 0;
        sid = 0;
    }
    ~MYSHADER(){
        //A value of 0 for sid  will be silently ignored
        glDeleteShader(sid );
    }
    void SHADER_init(GLchar* nom, GLuint ty){
        strcpy(name, nom);
        type = ty;
    }
    GLshort SHADER_compile( const GLchar *code, bool debug );
};

#endif
