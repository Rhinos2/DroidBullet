//
//
//

#include <string.h>
#include <stdio.h>
#include "zlib/unzip.h"
#include "ZRreader.h"



GLshort ZReader::ZOpen(GLchar* asset_filename, bool relative_path){
    GLchar fpath[256] = {""}, fname[256] = {""};
    unzFile		    uf;
    unz_file_info   fi;
    unz_file_pos    fp;

   // strcpy( fpath, getenv( "FILESYSTEM" ) );filePathname
    strcpy( fpath, this->filePathname );

    uf = unzOpen( fpath );
    if( !uf )
        return -1;
    if(relative_path)
        sprintf( fname, "assets/%s", asset_filename );
    else
        strcpy( fname, asset_filename );

    unzGoToFirstFile( uf );
    unzGetFilePos( uf, &fp );

    if( unzLocateFile( uf, fname, 1 ) != UNZ_OK ){
        unzClose( uf );
        return -1;
    }
    unzGetCurrentFileInfo(  uf,  &fi, &filePathname[0], 256, NULL, 0, NULL, 0 );


    if( unzOpenCurrentFilePassword( uf, NULL ) != UNZ_OK ){
        unzClose( uf );
        return -1;
    }

    position = 0;
    size = fi.uncompressed_size;
    buffer = (GLchar*)malloc(fi.uncompressed_size+1);
    buffer[fi.uncompressed_size] = '\0';

    while( unzReadCurrentFile( uf, &buffer[0], fi.uncompressed_size ) > 0 ){}

    unzCloseCurrentFile( uf );
    unzClose( uf );
    return 0;

}



GLuint ZReader::Zread(void *dst, GLuint size )
{
    if( (position + size) > this->size)
    {
        size = this->size - position;
    }

    memcpy( dst, &buffer[position], size );
    position += size;
    return size;
}


GLshort ZReader::ZOpenFont(GLchar* asset_filename, bool relative_path){
    GLchar fpath[256] = {""}, fname[256] = {""};
    unzFile		    uf;
    unz_file_info   fi;
    unz_file_pos    fp;

    strcpy( fpath, this->filePathname );

    uf = unzOpen( fpath );
    if( !uf )
        return -1;
    if(relative_path)
        sprintf( fname, "assets/%s", asset_filename );
    else
        strcpy( fname, asset_filename );

    unzGoToFirstFile( uf );
    unzGetFilePos( uf, &fp );

    if( unzLocateFile( uf, fname, 1 ) != UNZ_OK ){
        unzClose( uf );
        return -1;
    }
    unzGetCurrentFileInfo(  uf,  &fi, &filePathname[0], 256, NULL, 0, NULL, 0 );


    if( unzOpenCurrentFilePassword( uf, NULL ) != UNZ_OK ){
        unzClose( uf );
        return -1;
    }

    position = 0;
    size = fi.uncompressed_size;
    buffertoo = (unsigned char*)malloc(fi.uncompressed_size+1);
    buffertoo[fi.uncompressed_size] = 0;

    while( unzReadCurrentFile( uf, &buffertoo[0], fi.uncompressed_size ) > 0 ){}

    unzCloseCurrentFile( uf );
    unzClose( uf );
    return 0;

}
