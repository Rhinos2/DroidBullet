//
//
//

#include <android/log.h>
#include <errno.h>
#include "0ldFont.h"
#include "GLES3/gl3.h"

#include "ZRreader.h"
#include "../GLM/glm/vec2.hpp"
#include "../GLM/glm/gtc/type_ptr.hpp"
#include "GLES3/gl3ext.h"


GLshort OLDFONT::FONT_init( GLchar* apk_name )
{
    fontprg = new MYPROGRAM((GLchar *) "FontProgram");
    if(0 != fontprg->PROGRAM_create((GLchar*)"font_shader.vert", (GLchar*)"font_shader.frag", apk_name, true))
        return -1;
    if(0 != fontprg->PROGRAM_link(true))
        return -1;
    return 0;
}


void OLDFONT::FONT_free( )
{
    if( character_data ) free( character_data );
    if( tid ) glDeleteTextures( 1, &tid );
}


GLshort OLDFONT::FONT_load(GLchar* apkFilePath, char *filename,  GLfloat font_size, GLuint texture_width, GLuint texture_height, GLint	first_character, GLint count_character)
{
      ZReader zreader12;
    zreader12.Init(apkFilePath);
    if(0 != zreader12.ZOpenFont(filename, true)){
        __android_log_print(ANDROID_LOG_ERROR, "", "[ %s ]\n%s", name, "Error reading font zip file.");
        return -1;
    }
        unsigned char *texel_array = ( unsigned char * ) malloc( texture_width * texture_height );
        character_data = ( stbtt_bakedchar * ) malloc( count_character * sizeof( stbtt_bakedchar ) );
        this->font_size = font_size;
        this->texture_width = texture_width;
        this->texture_height = texture_height;
        this->first_character = first_character;
        this->count_character = count_character;

        stbtt_BakeFontBitmap( zreader12.RetBufToo(), 0, font_size, texel_array, texture_width, texture_height, first_character, count_character, this->character_data );

        glActiveTexture( GL_TEXTURE0 );
        glGenTextures(1, &this->tid );

        glBindTexture( GL_TEXTURE_2D, this->tid );

        glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, texture_width, texture_height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, texel_array );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        free( texel_array );

    return 0;
}


void OLDFONT::FONT_print(GLfloat x, GLfloat y, GLchar *text, glm::mat4& prj, glm::vec4& color)
{
////////////////////////////////////////////////////////////////////////////
    GLint vertex_attrib = fontprg->PROGRAM_get_vertex_attrib_location((GLchar *) "POSITIONF");
    GLint texcoord_attrib = fontprg->PROGRAM_get_vertex_attrib_location((GLchar *) "TEXCOORD0F");

   // glBindBuffer( GL_ARRAY_BUFFER, 0 );
   // glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glDepthMask( GL_FALSE );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glUseProgram(fontprg->GetPID());
    GLint uniform;
    glm::mat4 MVP = prj;
    uniform = fontprg->PROGRAM_get_uniform_location((GLchar *) "MODELVIEWPROJECTIONMATRIXF");
    glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(MVP));

    uniform = fontprg->PROGRAM_get_uniform_location((GLchar *) "DIFFUSEF");
    glUniform1i(uniform, 0);


        uniform = fontprg->PROGRAM_get_uniform_location((GLchar *) "COLORF");
        glUniform4fv(uniform, 1, glm::value_ptr(color));


    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, this->tid );
    glEnableVertexAttribArray( (GLuint)vertex_attrib );
    glEnableVertexAttribArray( (GLuint)texcoord_attrib );

    while( *text )
    {
        if( *text >= this->first_character && *text <= ( this->first_character + this->count_character ) )
        {
            glm::vec2 vert[ 4 ];
            glm::vec2 uv[ 4 ];

            stbtt_aligned_quad quad;

            stbtt_bakedchar *bakedchar = this->character_data + ( *text - this->first_character );

            int round_x = STBTT_ifloor( x + bakedchar->xoff );
            int round_y = STBTT_ifloor( y - bakedchar->yoff );
            quad.x0 = ( float )round_x;
            quad.y0 = ( float )round_y;
            quad.x1 = ( float )round_x + bakedchar->x1 - bakedchar->x0;
            quad.y1 = ( float )round_y - bakedchar->y1 + bakedchar->y0;

            quad.s0 = bakedchar->x0 / ( float )this->texture_width;
            quad.t0 = bakedchar->y0 / ( float )this->texture_width;
            quad.s1 = bakedchar->x1 / ( float )this->texture_height;
            quad.t1 = bakedchar->y1 / ( float )this->texture_height;

            x += bakedchar->xadvance;

            vert[ 0 ].x = quad.x1; vert[ 0 ].y = quad.y0;
            uv  [ 0 ].x = quad.s1; uv  [ 0 ].y = quad.t0;

            vert[ 1 ].x = quad.x0; vert[ 1 ].y = quad.y0;
            uv  [ 1 ].x = quad.s0; uv  [ 1 ].y = quad.t0;


            vert[ 2 ].x = quad.x1; vert[ 2 ].y = quad.y1;
            uv  [ 2 ].x = quad.s1; uv  [ 2 ].y = quad.t1;


            vert[ 3 ].x = quad.x0; vert[ 3 ].y = quad.y1;
            uv  [ 3 ].x = quad.s0; uv  [ 3 ].y = quad.t1;

            //////////////////////////////////////////////////////////////////
            glVertexAttribPointer( (GLuint)vertex_attrib, 2, GL_FLOAT, GL_FALSE, 0, ( float * )&vert[ 0 ] );
            glVertexAttribPointer( (GLuint)texcoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, ( float * )&uv[ 0 ] );
            /////////////////////////////////////////////////////////////

            glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
        }
        ++text;
    }
    glDisableVertexAttribArray( (GLuint)vertex_attrib );
    glDisableVertexAttribArray( (GLuint)texcoord_attrib );
    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );
    glDepthMask( GL_TRUE );
    glDisable( GL_BLEND );

}


float OLDFONT::FONT_length( char *text )
{
    float length = 0;

    while( *text )
    {
        if( *text >= this->first_character &&
            *text <= ( this->first_character + this->count_character ) )
        {
            stbtt_bakedchar *bakedchar = this->character_data + ( *text - this->first_character );
            length += bakedchar->xadvance;
        }

        ++text;
    }

    return length;
}
