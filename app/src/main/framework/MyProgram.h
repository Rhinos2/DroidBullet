//
//
//

#ifndef CLEAN23_MYPROGRAM_H
#define CLEAN23_MYPROGRAM_H

#include <vector>
#include "GLES3/gl3.h"
#include "MyShader.h"


struct MYUNIFORM
{
    GLchar	name[256 ];
    GLuint	type;
    GLint	location;

};

struct MYVERTEX_ATTRIB
{
    GLchar	name[64];
    GLenum	type;
    GLint	location;

};

class MYPROGRAM
{
    GLchar						name[64];
    MYSHADER					vertex_shader;
    MYSHADER					fragment_shader;
    GLuint				 		pid;
    GLshort						uniform_count;
    std::vector<MYUNIFORM>		uniform_array;
    GLshort				 		vertex_attrib_count;
    std::vector<MYVERTEX_ATTRIB>		 vertex_attrib_array;
public:
    std::vector<MYVERTEX_ATTRIB>& GetVertAttribArray() { return vertex_attrib_array; }
    //Constructor

    MYPROGRAM(GLchar *nm) : vertex_shader(( GLchar * )"noshader"), fragment_shader(( GLchar * )"noshader")
    {
        strcpy(name, nm);
        pid = 0;
        uniform_count = 0;
        vertex_attrib_count = 0;
    }

    ~MYPROGRAM(){
        if(pid){
            glDeleteProgram(pid );
            pid = 0;
        }
        uniform_array.clear();
        vertex_attrib_array.clear();
    }
//memeber functions
    GLuint GetPID(){
        return pid;
    }

    GLshort PROGRAM_create(GLchar *vertex_shader_filename, GLchar *fragment_shader_filename, GLchar*  apkFilePath, bool debug_shader);

    GLshort PROGRAM_link(bool debug );
    GLint PROGRAM_get_vertex_attrib_location( GLchar *name );
    GLint PROGRAM_get_uniform_location( GLchar *name );


};
#endif
