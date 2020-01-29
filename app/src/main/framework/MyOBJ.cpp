//
//
//
#include <errno.h>

#include <EGL/egl.h>
#include "GLES3/gl3.h"
#include "MyOBJ.h"
#include "nvtristrip/NvTriStrip.h"
#include <android/log.h>

#define BUFFER_OFFSET( x ) ( ( char * )NULL + x )

GLushort * MESH::GetOptimInxBuffer(){
    n_vecInx = n_vecInxOptim;
    return  &vecInxOptz[0];
}

std::vector<GLushort>  MESH::GetUV_InxBuffer(){
    n_vecInx = vecInxUV.size();
   return  vecInxUV;
}
GLushort * MESH::GetV_InxBuffer(){
    n_vecInx = vecInxV.size();
    return &vecInxV[0];
}

std::vector<glm::vec3>& MESH::GetVPosBuffer() {

    if( 0 == n_vecVPos)
        n_vecVPos = vecVPos.size();

    return vecVPos;
}
std::vector<glm::vec3>& MESH::GetVNormBuffer(){
    if( 0 == n_vecVNorm)
        n_vecVNorm = vecVNorm.size();
    return vecVNorm;
}
std::vector<glm::vec2>& MESH::GetVTexBuffer(){
    return vecVTexCoords;
}
void MESH::BindTexture(MYTEXTURE* pTexture){
    glBindTexture(pTexture->target, pTexture->tid);
}
bool MESH::LoadTextures(GLchar*  apkFilePath, MYTEXTURE* pTexture, GLuint texUnit, GLuint flags, GLchar filter, GLfloat anisotropic_filter) {
    ZReader zreader;
    zreader.Init(apkFilePath);
    if (0 != zreader.ZOpen(pTexture->name, true))
        return false;
    pTexture->LoadPNG(&zreader);
    zreader.ZClose();

    glActiveTexture(texUnit);
    glGenTextures(1, &pTexture->tid);
    glBindTexture(pTexture->target, pTexture->tid);

    glTexImage2D(pTexture->target, 0, pTexture->internal_format, pTexture->width, pTexture->height,
                 0, pTexture->format, pTexture->texel_type, pTexture->GetTextelArray());
    pTexture->FreeTexelArray();




    if( flags & TEXTURE_CLAMP ) {
        glTexParameteri( pTexture->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( pTexture->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    }
    else {
        glTexParameteri(pTexture->target, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri(pTexture->target, GL_TEXTURE_WRAP_T, GL_REPEAT );
    }


    if( anisotropic_filter ) {
        static float texture_max_anisotropy = 0.0f;
        if( !texture_max_anisotropy )
        //    glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &texture_max_anisotropy );
                               ;
       // anisotropic_filter = CLAMP( anisotropic_filter, 0.0f, texture_max_anisotropy );
        //glTexParameterf(pTexture->target, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropic_filter );
    }


    if( flags & TEXTURE_MIPMAP )
    {
        switch( filter )
        {
            case TEXTURE_FILTER_1X:
                glTexParameteri(pTexture->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );
                glTexParameteri(pTexture->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                break;

            case TEXTURE_FILTER_2X:
                glTexParameteri(pTexture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
                glTexParameteri(pTexture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                break;

            case TEXTURE_FILTER_3X:
                glTexParameteri(pTexture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
                glTexParameteri(pTexture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                break;

            case TEXTURE_FILTER_0X:
            default:
                glTexParameteri(pTexture->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
                glTexParameteri(pTexture->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                break;
        }
    }
    else
    {
        switch( filter )
        {
            case TEXTURE_FILTER_0X:
                glTexParameteri(pTexture->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                glTexParameteri(pTexture->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
                break;

            default:
                glTexParameteri(pTexture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                glTexParameteri(pTexture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                break;
        }
    }

    if( flags & TEXTURE_MIPMAP)
        glGenerateMipmap(pTexture->target );

    return true;
}
bool GObjects::ReadMTL(GLchar*  apkFilePath, GLchar *mtl_name) {
    ZReader zreader;
    zreader.Init(apkFilePath);
    if(0 != zreader.ZOpen(mtl_name, true)){
        __android_log_print(ANDROID_LOG_ERROR, "", "[ %s ]\n%s", mtl_name, "Error opening  MTL file.");
        return false;
    }
    for(GLchar *line = strtok(zreader.ZGetBuffer(), "\n" ); line != NULL; line = strtok(NULL, "\n" )){
        if(line[ 0 ] == '#') //skip
            continue;
        else if( line[ 0 ] == 'n' && line[1] == 'e' && line[2] == 'w') { //newmtl
          if( sscanf( line, "newmtl %s", &Material.m_name[0] ) != 1 )
                return false;
        }
        else if(line[ 0 ] == 'N' && line[1] == 's' && line[2] == ' ') {  //Ns
            if( sscanf( line, "Ns %f", &Material.specular_exponent ) != 1 )
                return false;
        }

        else if( line[ 0 ] == 'K' && line[1] == 'a' && line[2] == ' ') { //Ka
            if( sscanf( line, "Ka %f %f %f", &Material.ambient.x, &Material.ambient.y, &Material.ambient.z ) != 3 )
                return false;
        }
        else if( line[ 0 ] == 'K' && line[1] == 'd' && line[2] == ' ') { //Kd
            if( sscanf( line, "Kd %f %f %f", &Material.diffuse.x, &Material.diffuse.y, &Material.diffuse.z ) != 3 )
                return false;
        }
        else if( line[ 0 ] == 'K' && line[1] == 's' && line[2] == ' ') { //Ks
            if( sscanf( line, "Ks %f %f %f", &Material.specular.x, &Material.specular.y, &Material.specular.z ) != 3 )
                return false;
        }
        else if( line[ 0 ] == 'T' && line[1] == 'f' && line[2] == ' ') { //Tf
            if( sscanf( line, "Tf %f %f %f", &Material.transmission_filter.x, &Material.transmission_filter.y, &Material.transmission_filter.z ) != 3 )
                return false;
        }
        else if(line[ 0 ] == 'N' && line[1] == 'i' && line[2] == ' ') {  //Ni
            if( sscanf( line, "Ni %f", &Material.optical_density ) != 1 )
                return false;
        }
        else if( line[ 0 ] == 'd' && line[1] == ' ') { //d
            if (sscanf(line, "d %f", &Material.dissolve) != 1)
                return false;
            Material.ambient.w = Material.dissolve;
            Material.diffuse.w = Material.dissolve;
            Material.specular.w = Material.dissolve;
        }
        else if( line[ 0 ] == 'i' && line[1] == 'l' && line[2] == 'l') { //illum
            if (sscanf(line, "illum %d", &Material.illumination_model) != 1)
                return false;
        }
        /////////////////////////////////////////////////Texture maps
        else if( line[ 0 ] == 'm' && line[1] == 'a' && line[2] == 'p' && line[5] == 'a') { //map_Ka
            if( sscanf( line, "map_Ka %s", &Material.map_ambient_file[0] ) != 1 )
                return false;
            Material.bAmbient = true;
        }
        else if( line[ 0 ] == 'm' && line[1] == 'a' && line[2] == 'p' && line[5] == 'd') { //map_Kd
            if( sscanf( line, "map_Kd %s", &Material.map_diffuse_file[0] ) != 1 )
                return false;
            Material.bDiffuse = true;
        }
        else if( line[ 0 ] == 'm' && line[1] == 'a' && line[2] == 'p' && line[5] == 's') { //map_Ks
            if( sscanf( line, "map_Ks %s", &Material.map_specular_file[0] ) != 1 )
                return false;
            Material.bSpecular = true;
        }
    } //end of reading loop
    zreader.ZClose();
    return true;
}
bool GObjects::ReadOBJ(GLchar*  apkFilePath, GLchar *obj_name) {

    ZReader zreader;
    zreader.Init(apkFilePath);
    if(0 != zreader.ZOpen(obj_name, true)){
        __android_log_print(ANDROID_LOG_ERROR, "", "[ %s ]\n%s", obj_name, "Error opening  OBJ file.");
        return false;
    }

        for (GLchar *line = strtok(zreader.ZGetBuffer(), "\n"); line != NULL; line = strtok(NULL, "\n")) //reading loop
        {
            if (line[0] == '#') //skip
                continue;

            else if( line[ 0 ] == 'm' && line[1] == 't' && line[2] == 'l') {
                isMTL = true;
                if( sscanf( line, "mtllib %s", &MtlLib[0] ) != 1 )
                    return false;
                
            }
            else if (line[0] == 'o' && line[1] == ' ') {  //new object mesh name
                GLchar name[64];
                if (sscanf(line, "o %s", &name[0]) != 1)
                    return false;
                GObjs.push_back(new MESH(&name[0]));
                numMesh = GObjs.size() -1;//zero-based
                LookUpMesh[std::string(name)] =  numMesh;
               continue;
            }
            
            if (line[0] == 'u' && line[1] == 's' && line[2] == 'e' &&
                     line[3] == 'm') {  //usemtl
                if (sscanf(line, "usemtl %s", &GObjs[numMesh]->UseMtl[0]) != 1)
                    return false;
            }
                ////////////////////////////////////////////////////////////vertex position reading
            else if (line[0] == 'v' && line[1] == ' ') {
                glm::vec3 PosCoords;
                if (sscanf(line, "v %f %f %f", &PosCoords.x, &PosCoords.y, &PosCoords.z) != 3)
                    return false;
                GObjs[numMesh]->vecVPos.push_back(PosCoords);
                continue;
            }
                ///////////////////////////////////////////////////////////////texture coords reading
                //Vt
            else if (line[0] == 'v' && line[1] == 't') {
                glm::vec2 TexCoords;
                if (sscanf(line, "vt %f %f", &TexCoords.x, &TexCoords.y) != 2)
                    return false;
                TexCoords.y = 1.0f - TexCoords.y;
                GObjs[numMesh]->vecVTexCoords.push_back(TexCoords);
            }

                ////////////////////////////////////////////////////////////Faces
            else if (line[0] == 'f' && line[1] == ' ') {
                IFS triangle;


                if (sscanf(line, "f %d %d %d", &triangle.PosInx[0], &triangle.PosInx[1],
                           &triangle.PosInx[2]) == 3) {//v 1 2 3
                    triangle--; //correction to zero based indices
                    //cross counterclockwise ([1] -[0]), ([2] - [0])
                    glm::vec3 ONE = GObjs[numMesh]->vecVPos[triangle.PosInx[1]];
                    glm::vec3 ZERO = GObjs[numMesh]->vecVPos[triangle.PosInx[0]];
                    glm::vec3 TWO = GObjs[numMesh]->vecVPos[triangle.PosInx[2]];
                    triangle.triNormal = glm::cross(ONE - ZERO, TWO - ZERO);
                    GObjs[numMesh]->Triangles.push_back(triangle);
                    //////////////////////////////////////////
                    GObjs[numMesh]->vecInxV.push_back((GLushort) triangle.PosInx[0]);
                    GObjs[numMesh]->vecInxV.push_back((GLushort) triangle.PosInx[1]);
                    GObjs[numMesh]->vecInxV.push_back((GLushort) triangle.PosInx[2]);
                    continue;
                }

                else if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &triangle.PosInx[0], &triangle.VtInx[0], &triangle.VnInx[0],
                                &triangle.PosInx[1], &triangle.VtInx[1], &triangle.VnInx[1],
                                &triangle.PosInx[2], &triangle.VtInx[2], &triangle.VnInx[2]) == 9) {// f 1/2/3 4/5/6 6/7/8
                    triangle--; //correction to zero based indices
                    //cross counterclockwise ([1] -[0]), ([2] - [0])
                    glm::vec3 ONE = GObjs[numMesh]->vecVPos[triangle.PosInx[1]];
                    glm::vec3 ZERO = GObjs[numMesh]->vecVPos[triangle.PosInx[0]];
                    glm::vec3 TWO = GObjs[numMesh]->vecVPos[triangle.PosInx[2]];
                    triangle.triNormal = glm::cross(ONE - ZERO, TWO - ZERO);
                    GObjs[numMesh]->Triangles.push_back(triangle);
                    //////////////////////////////////////////////
                    GObjs[numMesh]->vecInxV.push_back((GLushort) triangle.PosInx[0]);
                    GObjs[numMesh]->vecInxV.push_back((GLushort) triangle.PosInx[1]);
                    GObjs[numMesh]->vecInxV.push_back((GLushort) triangle.PosInx[2]);
                    ////////////////////////////////////////////////
                    GObjs[numMesh]->vecInxUV.push_back((GLushort) triangle.VtInx[0]);
                    GObjs[numMesh]->vecInxUV.push_back((GLushort) triangle.VtInx[1]);
                    GObjs[numMesh]->vecInxUV.push_back((GLushort) triangle.VtInx[2]);
                    //maps
                    //   VPind_to_VUind[triangle.PosInx[0]] = triangle.VtInx[0];
                    //   VPind_to_VUind[triangle.PosInx[1]] = triangle.VtInx[1];
                    //   VPind_to_VUind[triangle.PosInx[2]] = triangle.VtInx[2];

                    GObjs[numMesh]->VUinx_to_VTinx[triangle.VtInx[0]] = triangle.PosInx[0];
                    GObjs[numMesh]->VUinx_to_VTinx[triangle.VtInx[1]] = triangle.PosInx[1];
                    GObjs[numMesh]->VUinx_to_VTinx[triangle.VtInx[2]] = triangle.PosInx[2];
                    continue;
                }

                else if (sscanf(line, "f %d//%d %d//%d %d//%d", &triangle.PosInx[0],
                                &triangle.VnInx[0],
                                &triangle.PosInx[1], &triangle.VnInx[1],
                                &triangle.PosInx[2], &triangle.VnInx[2]) ==
                         6) { //		f 1//2 3//4 8//9
                    triangle--;
                    //cross counterclockwise ([1] -[0]), ([2] - [0])
                    glm::vec3 ONE = GObjs[numMesh]->vecVPos[triangle.PosInx[1]];
                    glm::vec3 ZERO = GObjs[numMesh]->vecVPos[triangle.PosInx[0]];
                    glm::vec3 TWO = GObjs[numMesh]->vecVPos[triangle.PosInx[2]];
                    triangle.triNormal = glm::cross(ONE - ZERO, TWO - ZERO);
                    GObjs[numMesh]->Triangles.push_back(triangle);
                    /////////////////////////////////////////////////
                    GObjs[numMesh]->vecInxV.push_back((GLushort) triangle.PosInx[0]);
                    GObjs[numMesh]->vecInxV.push_back((GLushort) triangle.PosInx[1]);
                    GObjs[numMesh]->vecInxV.push_back((GLushort) triangle.PosInx[2]);
                    ///////////////////////////////////////////////
                    GObjs[numMesh]->vecInxUV.push_back((GLushort) triangle.VtInx[0]);
                    GObjs[numMesh]->vecInxUV.push_back((GLushort) triangle.VtInx[1]);
                    GObjs[numMesh]->vecInxUV.push_back((GLushort) triangle.VtInx[2]);

                    //  VPind_to_VUind[triangle.PosInx[0]] = triangle.VtInx[0];
                    //   VPind_to_VUind[triangle.PosInx[1]] = triangle.VtInx[1];
                    //  VPind_to_VUind[triangle.PosInx[2]] = triangle.VtInx[2];

                    GObjs[numMesh]->VUinx_to_VTinx[triangle.VtInx[0]] = triangle.PosInx[0];
                    GObjs[numMesh]->VUinx_to_VTinx[triangle.VtInx[1]] = triangle.PosInx[1];
                    GObjs[numMesh]->VUinx_to_VTinx[triangle.VtInx[2]] = triangle.PosInx[2];
                    continue;
                }
                else if (
                        sscanf(line, "f %d/%d %d/%d %d/%d", &triangle.PosInx[0], &triangle.VtInx[0],
                               &triangle.PosInx[1], &triangle.VtInx[1],
                               &triangle.PosInx[2], &triangle.VtInx[2]) == 6) { //		f 1/2 3/4 8/9
                    triangle--;
                    //cross counterclockwise ([1] -[0]), ([2] - [0])
                    glm::vec3 ONE = GObjs[numMesh]->vecVPos[triangle.PosInx[1]];
                    glm::vec3 ZERO = GObjs[numMesh]->vecVPos[triangle.PosInx[0]];
                    glm::vec3 TWO = GObjs[numMesh]->vecVPos[triangle.PosInx[2]];
                    triangle.triNormal = glm::cross(ONE - ZERO, TWO - ZERO);
                    GObjs[numMesh]->Triangles.push_back(triangle);
                    //////////////////////////////////////////////////
                    GObjs[numMesh]->vecInxV.push_back((GLushort) triangle.PosInx[0]);
                    GObjs[numMesh]->vecInxV.push_back((GLushort) triangle.PosInx[1]);
                    GObjs[numMesh]->vecInxV.push_back((GLushort) triangle.PosInx[2]);
                    //////////////////////////////////////////////////
                    GObjs[numMesh]->vecInxUV.push_back((GLushort) triangle.VtInx[0]);
                    GObjs[numMesh]->vecInxUV.push_back((GLushort) triangle.VtInx[1]);
                    GObjs[numMesh]->vecInxUV.push_back((GLushort) triangle.VtInx[2]);

                    //  VPind_to_VUind[triangle.PosInx[0]] = triangle.VtInx[0];
                    //    VPind_to_VUind[triangle.PosInx[1]] = triangle.VtInx[1];
                    //  VPind_to_VUind[triangle.PosInx[2]] = triangle.VtInx[2];

                    GObjs[numMesh]->VUinx_to_VTinx[triangle.VtInx[0]] = triangle.PosInx[0];
                    GObjs[numMesh]->VUinx_to_VTinx[triangle.VtInx[1]] = triangle.PosInx[1];
                    GObjs[numMesh]->VUinx_to_VTinx[triangle.VtInx[2]] = triangle.PosInx[2];
                    continue;
                }

            }////////////////////////////////////////////////////////////

            else if (line[0] == 'v' && line[1] == 'n') { ;//ignore
            }

        }
        // end of reading loop////////////////////////////////////////////////////////////////////////////
        //weighted vertex normals calculation
    for(GLuint mesh = 0; mesh < GObjs.size(); mesh++) {
        GLuint vn_size = GObjs[mesh]->vecVPos.size(); //same size
        GObjs[mesh]->vecVNorm.reserve(vn_size);

        for (GLuint i = 0; i < GObjs[mesh]->vecVPos.size(); i++) {
            glm::vec3 temp(0.0f, 0.0f, 0.0f);
            for (GLuint t = 0; t < GObjs[mesh]->Triangles.size(); t++) {
                if (GObjs[mesh]->Triangles[t].PosInx[0] == i ||
                    GObjs[mesh]->Triangles[t].PosInx[1] == i ||
                    GObjs[mesh]->Triangles[t].PosInx[2] == i) {
                    temp += GObjs[mesh]->Triangles[t].triNormal;
                }
            }
            GObjs[mesh]->vecVNorm.push_back(temp);

        }
        for (GLuint k = 0; k < GObjs[mesh]->vecVNorm.size(); k++) {
            GObjs[mesh]->vecVNorm[k] = glm::normalize(GObjs[mesh]->vecVNorm[k]);
        }
    }
    zreader.ZClose();
    return true;
}
bool MESH::OptimizeMesh(GLuint vertex_cache_size, std::vector<GLushort> &vecIndeces){
    GLuint s = 0;
    GLushort n_group = 0;
    bool ret = false;
    if( vertex_cache_size )
        SetCacheSize( vertex_cache_size );

    PrimitiveGroup *primitivegroup;
    if( GenerateStrips(vecIndeces, vecIndeces.size(), &primitivegroup, &n_group, false ) ){
        if( primitivegroup[ 0 ].numIndices < vecIndeces.size() ){
            mode = GL_TRIANGLE_STRIP;
            n_vecInxOptim = primitivegroup[ 0 ].numIndices;
            s = primitivegroup[ 0 ].numIndices * sizeof(GLushort );
            vecInxOptz.reserve(n_vecInxOptim);
            memcpy(&vecInxOptz[0], &primitivegroup[ 0 ].indices[ 0 ], s);
            ret = true;
        }
    }
    delete[] primitivegroup;
    return ret;
}

void MESH::UpdateBoundMesh(){
    //GLuint i, index;
    // Get the mesh min and max.
    min.x = min.y = min.z = 99999.999f;
    max.x = max.y = max.z = -99999.999f;
    for(GLuint i= 0; i < vecVPos.size(); i++){
        if(vecVPos[i].x < min.x) min.x = vecVPos[i].x;
        if(vecVPos[i].y < min.y) min.y = vecVPos[i].y;
        if(vecVPos[i].z < min.z) min.z = vecVPos[i].z;

        if(vecVPos[i].x > max.x) max.x = vecVPos[i].x;
        if(vecVPos[i].y > max.y) max.y = vecVPos[i].y;
        if(vecVPos[i].z > max.z) max.z = vecVPos[i].z;
    }
    location = (min + max)/2.0f;
    dimension = max - min;

     // Bounding sphere radius
	radius = dimension.x > dimension.y ? dimension.x : dimension.y;
	radius = radius > dimension.z ? radius * 0.5f : dimension.z * 0.5f;

}
void MESH::BuildVBO() {

   // GLint array_buffer_size;
    bool opt = false;
    GLint nTexCoors = vecVTexCoords.size();

    if (nTexCoors > 0) {
        array_buffer_size = (sizeof(glm::vec3)  + sizeof(glm::vec3)  + sizeof(glm::vec2) ) * vecVTexCoords.size();
        stride = sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2);
        opt = OptimizeMesh(0, vecInxUV);
    }
    else {                                                                   // no Vt
        array_buffer_size = (sizeof(glm::vec3)  + sizeof(glm::vec3) ) * vecVPos.size();
        stride = sizeof(glm::vec3) + sizeof(glm::vec3);
        opt = OptimizeMesh(0, vecInxV);
    }
    vecVBO.reserve((GLuint) array_buffer_size);

    if (nTexCoors > 0) {                          // build on Vt list
        for (int pos = 0; pos < vecVTexCoords.size(); pos++) {

            GLuint indexV = VUinx_to_VTinx[pos];        //map Vt index to V index
            //center pivot
            vecVPos[indexV] = vecVPos[indexV] - location;

            vecVBO.push_back(vecVPos[indexV].x);
            vecVBO.push_back(vecVPos[indexV].y);
            vecVBO.push_back(vecVPos[indexV].z);

            vecVBO.push_back(vecVNorm[indexV].x);
            vecVBO.push_back(vecVNorm[indexV].y);
            vecVBO.push_back(vecVNorm[indexV].z);
            // GLuint indexUV = mapV_UV[pos];
            vecVBO.push_back(vecVTexCoords[pos].x);
            vecVBO.push_back(vecVTexCoords[pos].y);
        }
    }
    else {               //builed on V list
        for (int pos = 0; pos < vecVPos.size(); pos++) {

            vecVPos[pos] = vecVPos[pos] - location;

            vecVBO.push_back(vecVPos[pos].x);
            vecVBO.push_back(vecVPos[pos].y);
            vecVBO.push_back(vecVPos[pos].z);

            vecVBO.push_back(vecVNorm[pos].x);
            vecVBO.push_back(vecVNorm[pos].y);
            vecVBO.push_back(vecVNorm[pos].z);
        }
    }

    //////////////////////////////////////////////////VBO build end
}
void MESH::BindBuffers(){
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
}
void MESH::BindLoadBuffers(bool tex) {
    glGenBuffers(2, vboIds);
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, array_buffer_size, &vecVBO[0], GL_STATIC_DRAW);
    //vecVBO.clear();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
    if(n_vecInxOptim > 0)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * n_vecInxOptim, &vecInxOptz[0], GL_STATIC_DRAW);
    else if(tex)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * vecInxUV.size(), &vecInxUV[0], GL_STATIC_DRAW);
    else
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * vecInxV.size(), &vecInxV[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MESH::SetMeshAttribs(  MYPROGRAM *prg){

    for(GLint i = 0; i < prg->GetVertAttribArray().size(); i++){
        GLint attribute;
        if(0 == strcmp(prg->GetVertAttribArray()[i].name, "POSITION")){
            attribPos = prg->PROGRAM_get_vertex_attrib_location(( GLchar * )"POSITION");

            attribs.push_back(attribPos);
            continue;
        }
        else if(0 == strcmp(prg->GetVertAttribArray()[i].name, "NORMAL")){
            attribNor = prg->PROGRAM_get_vertex_attrib_location(( GLchar * )"NORMAL");

            attribs.push_back(attribNor);
            continue;
        }
        else if(0 == strcmp(prg->GetVertAttribArray()[i].name, "VERTEXTURECOORD")){

            attribTex = prg->PROGRAM_get_vertex_attrib_location(( GLchar * )"VERTEXTURECOORD");

            attribs.push_back(attribTex);
            continue;
        }
    }
}

void MESH::DrawMesh(){

    glEnableVertexAttribArray((GLuint)attribPos);
    glVertexAttribPointer((GLuint) attribPos, 3, GL_FLOAT, GL_FALSE, stride , ( void * )NULL);

    glEnableVertexAttribArray((GLuint)attribNor);
    glVertexAttribPointer( (GLuint) attribNor, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET( sizeof( glm::vec3 ) ) );

    if(attribTex >= 0) {
        glEnableVertexAttribArray((GLuint) attribTex);
        glVertexAttribPointer( (GLuint) attribTex, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET( sizeof( glm::vec3 ) + sizeof( glm::vec3 ) ) );
    }
    glDrawElements ( (GLenum)mode, InxBufferSize(), GL_UNSIGNED_SHORT, 0 );

    glDisableVertexAttribArray((GLuint)attribPos);
    glDisableVertexAttribArray((GLuint)attribNor);
    if(attribTex >= 0)
        glDisableVertexAttribArray((GLuint)attribTex);
}
GLuint MESH::InxBufferSize(){
    if(n_vecInxOptim > 0)
        return n_vecInxOptim;
    else if(vecVTexCoords.size() > 0)
        return vecInxUV.size();
    else
        return vecInxV.size();
}

GObjects::~GObjects(){

    for(std::map<std::string, MYPROGRAM*>::iterator pos = mProgs.begin(); pos != mProgs.end(); pos++)
        delete pos->second ;
    for(std::vector<MESH*>::iterator pos1 = GObjs.begin(); pos1 != GObjs.end(); pos1++)
        delete pos1;
}