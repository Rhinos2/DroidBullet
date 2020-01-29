//
//
//

#ifndef DUMMY_0LDFONT_H
#define DUMMY_0LDFONT_H


#include "ttf/stb_truetype.h"
#include "MyProgram.h"
#include "../GLM/glm/detail/type_mat.hpp"
#include "../GLM/glm/vec4.hpp"

struct OLDFONT
{
    FILE*  stream;
    GLchar			name[ 64 ];
    stbtt_bakedchar *character_data;
    GLfloat			font_size;
    GLint				texture_width;
    GLint				texture_height;
    GLint				first_character;
    GLint				count_character;
    MYPROGRAM*		fontprg;
    GLuint	tid;
    GLuint buf;
    GLubyte*            ArrBuffer;
    OLDFONT(GLchar *nm) {  strcpy(name, nm); fontprg = NULL; }
    GLshort FONT_init( GLchar *name );
    void FONT_free();
    GLshort FONT_load( GLchar* apkFilePath, GLchar *filename, GLfloat font_size, GLuint texture_width, GLuint texture_height, GLint first_character, GLint count_character );
    void FONT_print( GLfloat x, GLfloat y, GLchar *text, glm::mat4& MVP, glm::vec4& color );
    GLfloat FONT_length(  GLchar *text );

} ;


#endif
