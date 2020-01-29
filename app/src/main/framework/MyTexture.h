//
//
//

#ifndef DUMMY_MYTEXTURE_H
#define DUMMY_MYTEXTURE_H

#define PNG_SETJMP_NOT_SUPPORTED

#include <cstring>
#include "GLES3/gl3.h"
#include "png/pngconf.h"
#include "png/png.h"
#include "ZRreader.h"

#define CLAMP( x, low, high ) ( ( x > high ) ? high : ( ( x < low ) ? low : x ) )

enum FILE_TYPE{
    PNG_FILE = 0,
    JPG_FILE = 1
};
enum TEX_MAP_TYPE{
    AMBIENT = 0,
    DIFFUSE = 1,
    SPECULAR = 2,
    BUMP     = 3,
    UNKNOWN =  4
};
enum {
    TEXTURE_CLAMP			= ( 1 << 0 ),
    TEXTURE_MIPMAP			= ( 1 << 1 ),
    TEXTURE_16_BITS			= ( 1 << 2 ),
    TEXTURE_16_BITS_5551	= ( 1 << 3 )
};

enum {
    TEXTURE_FILTER_0X = 0,
    TEXTURE_FILTER_1X = 1,
    TEXTURE_FILTER_2X = 2,
    TEXTURE_FILTER_3X = 3
};
struct MYTEXTURE
{
    TEX_MAP_TYPE  tex_map_type;
    GLchar	name[128 ];
    GLuint	tid;
    GLuint	width;
    GLuint	height;
    GLushort byte_format;
    GLuint	size;
    GLuint	target;
    GLuint	internal_format;
    GLuint	format;
    GLuint	texel_type;

    GLuint	n_mipmap;
private:
    GLubyte	*pTexelArray;
    static void ReadPNGDataFunc( png_structp structp, png_bytep bytep, png_size_t size );
public:
    MYTEXTURE(GLchar* nam, TEX_MAP_TYPE type = UNKNOWN, GLuint targ = GL_TEXTURE_2D){
        strcpy(name, nam);
        pTexelArray = NULL;
        tex_map_type = type;
        target = targ;
        tid = 0;
    }
    MYTEXTURE(){}
    ~MYTEXTURE(){
        if(tid != 0)
            glDeleteTextures( 1, &tid );
        if(pTexelArray != NULL)
            free(pTexelArray);
    }
    GLubyte* GetTextelArray() { return pTexelArray; }
    GLshort TexCreate(GLchar*  apkFilePath, FILE_TYPE ftype = PNG_FILE );
    GLshort LoadPNG(ZReader* zreader);
    GLshort GenTextureId(GLuint flags, GLbyte filter, GLfloat anisotropic_filter);
    void FreeTexelArray() { free(pTexelArray);}
};
#endif //DUMMY_MYTEXTURE_H
