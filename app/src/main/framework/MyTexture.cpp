//
//
//

#include "MyTexture.h"
//#include "SOIL/SOIL.h"
#include <android/log.h>

GLshort MYTEXTURE::TexCreate(GLchar*  apkFilePath, FILE_TYPE ftype) {
    ZReader zreader;
    zreader.Init(apkFilePath);
    if (0 != zreader.ZOpen(this->name, true)) {
        __android_log_print(ANDROID_LOG_ERROR, "", "[ %s ]\n%s", this->name, "Error reading texture file.");
        return -1;
    }
    if(ftype == JPG_FILE){
       /* const unsigned char* const tex_buffer = (unsigned char*)zreader.ZGetBuffer();
        unsigned char* image = NULL;
        GLint texwidth;
        GLint texheight;
        image = SOIL_load_image_from_memory( tex_buffer, zreader.bufferLen(), &texwidth, &texheight, 0, SOIL_LOAD_AUTO);
//TO DO
        SOIL_free_image_data(image);*/
        return 0;
    }
    else if(ftype ==  PNG_FILE){
        LoadPNG(&zreader);

    }
    zreader.ZClose();
    return 0;
}
void MYTEXTURE::ReadPNGDataFunc( png_structp structp, png_bytep bytep, png_size_t size )
{
    ZReader *p_zreader = ( ZReader * ) png_get_io_ptr( structp );
    p_zreader->Zread(bytep, size );
}

GLshort MYTEXTURE::LoadPNG( ZReader* pzreader){
    png_structp structp;
    png_infop infop;

    int png_bit_depth, png_color_type;
    structp = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );

    infop = png_create_info_struct( structp );
    png_set_read_fn( structp, ( png_voidp * )pzreader, ReadPNGDataFunc );

    png_read_info( structp, infop );
    png_bit_depth = png_get_bit_depth( structp, infop );
    png_color_type = png_get_color_type( structp, infop );

    if( png_color_type == PNG_COLOR_TYPE_PALETTE )
        png_set_expand( structp );

    if( png_color_type == PNG_COLOR_TYPE_GRAY && png_bit_depth < 8 )
        png_set_expand( structp );

    if( png_get_valid( structp, infop, PNG_INFO_tRNS ) )
        png_set_expand( structp );

    if( png_bit_depth == 16 )
        png_set_strip_16( structp );

    if( png_color_type == PNG_COLOR_TYPE_GRAY || png_color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
        png_set_gray_to_rgb( structp );

    png_read_update_info( structp, infop );
    png_get_IHDR( structp, infop, ( png_uint_32 * )&this->width, ( png_uint_32 * )&this->height, &png_bit_depth, &png_color_type,
                  NULL, NULL, NULL );
    switch( png_color_type )
    {
        case PNG_COLOR_TYPE_GRAY:
        {
            byte_format	= 1;
            internal_format = format = GL_LUMINANCE;
            break;
        }

        case PNG_COLOR_TYPE_GRAY_ALPHA:
        {
            byte_format = 2;
            internal_format = format = GL_LUMINANCE_ALPHA;
            break;
        }

        case PNG_COLOR_TYPE_RGB:
        {
            byte_format = 3;
            internal_format = format = GL_RGB;
            break;
        }

        case PNG_COLOR_TYPE_RGB_ALPHA:
        {
            byte_format	= 4;
            internal_format = format = GL_RGBA;
            break;
        }
        default:
            break;
    }
    texel_type = GL_UNSIGNED_BYTE;
    png_bytep *row_ptr = NULL;
    size = this->width * this->height * byte_format;
    pTexelArray = ( GLubyte*) malloc( size );
    row_ptr = ( png_bytep * ) malloc(height * sizeof( png_bytep ) );

    int n;
    for(int j = 0; j < height; j++){
        n = height - ( j + 1 );
        row_ptr[ n ] = pTexelArray + ( n * this->width * byte_format );
    }
    png_read_image( structp, row_ptr );
    png_read_end( structp, NULL );
    png_destroy_read_struct( &structp, &infop, NULL );
    free( row_ptr );
    return 0;
}
GLshort MYTEXTURE::GenTextureId(GLuint flags, GLbyte filter, GLfloat anisotropic_filter){
    if(tid != 0)
    {
        glDeleteTextures( 1, &tid );
        tid = 0;
    }

    glGenTextures( 1, &tid );
    glBindTexture(target, tid );

    if( flags & TEXTURE_CLAMP )
    {
        glTexParameteri( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    }
    else
    {
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT );
    }


    if( anisotropic_filter )
    {
        static float texture_max_anisotropy = 0.0f;
        if( !texture_max_anisotropy )
            ;
         //   glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &texture_max_anisotropy );
       // anisotropic_filter = CLAMP( anisotropic_filter, 0.0f, texture_max_anisotropy );
       // glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropic_filter );
    }


    if( flags & TEXTURE_MIPMAP )
    {
        switch( filter )
        {
            case TEXTURE_FILTER_1X:

                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

                break;


            case TEXTURE_FILTER_2X:

                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

                break;


            case TEXTURE_FILTER_3X:

                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

                break;


            case TEXTURE_FILTER_0X:
            default:

                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

                break;

        }
    }
    else
    {
        switch( filter )
        {
            case TEXTURE_FILTER_0X:

                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

                break;


            default:

                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

                break;

        }
    }

    glTexImage2D( target, 0, internal_format, width, height, 0, format, texel_type, pTexelArray );

    if( flags & TEXTURE_MIPMAP && !n_mipmap )
        glGenerateMipmap(target );
//   free(pTexelArray);
}