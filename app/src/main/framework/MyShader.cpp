//
//
//

#include "MyShader.h"
#include <iostream>

#include <android/log.h>



GLshort MYSHADER::SHADER_compile(const char *code, bool debug )
{
    char type[512] = {""};

    if( this->sid )
        return 0;
    GLuint t = this->type;
    GLuint ret = glCreateShader(t);
    this->sid =  ret;
    glShaderSource( this->sid, 1, &code, NULL );
    glCompileShader( this->sid );
/////////////////////////////////
    GLint vertex_compiled;
    glGetShaderiv( this->sid, GL_COMPILE_STATUS, &vertex_compiled );
    if( debug )
    {
        if( this->type == GL_VERTEX_SHADER )
            strcpy( type, "GL_VERTEX_SHADER" );
        else
            strcpy( type, "GL_FRAGMENT_SHADER" );

        if (vertex_compiled != GL_TRUE)
        {
            GLsizei log_length = 0;
            GLchar message[1024];
            glGetShaderInfoLog(this->sid, 1024, &log_length, message);

            __android_log_print( ANDROID_LOG_ERROR, "", "[ %s:%s ]\n%s", name, type, message );
        }

    }

    if(vertex_compiled != GL_TRUE)
    {
        glDeleteShader(sid );
        sid = 0;
        return 0;
    }

    return 1;
}



