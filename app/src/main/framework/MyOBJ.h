//
//
//

#ifndef DUMMY_MYOBJ_H
#define DUMMY_MYOBJ_H

#include <map>
#include <vector>
#include <string.h>
#include <stl/_string_fwd.h>
#include <stl/_string.h>

#include "../GLM/glm/glm.hpp"
#include "../GLM/glm/fwd.hpp"

#include "../GLM/glm/vec4.hpp"
#include "../GLM/glm/vec3.hpp"
#include "../GLM/glm/vec2.hpp"
#include "../GLM/glm/gtx/normal.hpp"
#include "MyTexture.h"
#include "MyProgram.h"
#include "bullet/btRigidBody.h"


struct OBJ_MATERIAL
{
    GLchar					m_name[64];				// newmtl
    glm::vec4			    ambient;						// Ka
    glm::vec4				diffuse;						// Kd
    glm::vec4				specular;						// Ks
    glm::vec3				transmission_filter;			// Tf

    GLint					illumination_model;				// illum
    GLfloat					dissolve;						// d
    GLfloat					specular_exponent;				// Ns
    GLfloat					optical_density;				// Ni

    GLchar					map_ambient_file[ 64 ];		// map_Ka
    GLchar					map_diffuse_file[ 64 ];		// map_Kd
    GLchar					map_specular_file[ 64 ];		// map_Ks
    //GLchar					map_translucency_file[ 128 ];	// map_Tr
   // GLchar					map_disp_file[ 128 ];			// disp or map_disp
   // GLchar					map_bump_file[ 128 ];			// bump or map_bump
    bool bAmbient;
    bool bDiffuse;
    bool bSpecular;

    OBJ_MATERIAL() {bAmbient = bDiffuse = bSpecular = false;}
} ;



struct VERTX
{
    glm::vec3   V;

    glm::vec3   Vn;
    glm::vec4   Tgt;
    GLuint      nIndex;
    VERTX()
    {
        V.x  = V.y = V.z = 0.0f;

        Vn.x    =  Vn.y  =  Vn.z = 0.0f;
        Tgt.x   = Tgt.y = Tgt.z = 0.0f;
        nIndex = 0;
    }
};

struct  IFS
{
    GLuint PosInx[3];
    GLuint VtInx[3];
    GLuint VnInx[3];
    glm::vec3 triNormal;
    IFS(){
        PosInx[0] = PosInx[1] = PosInx[2] = 0;
        VtInx[0] = VtInx[1] = VtInx[2] = 0;
    }
    inline IFS& operator --(int) {
        PosInx[0]--;PosInx[1]--; PosInx[2]--;
        VtInx[0]--;VtInx[1]--; VtInx[2]--;
        return *this;
    }
};
class GObjects;

class MESH
{
    friend class GObjects;

    GLchar                      UseMtl[64]; //usemtl
   // glm::vec3               location;
    glm::vec3			        min;
    glm::vec3			        max;
    glm::vec3			        dimension;
    GLfloat			            radius;
   // GLfloat			        distance;
    GLint                       mode;

    std::vector<glm::vec3>      vecVPos;
    GLuint                      n_vecVPos;
    std::vector<glm::vec3>      vecVNorm;
    GLuint                      n_vecVNorm;
    std::vector<glm::vec2>      vecVTexCoords;

    std::vector<GLushort>        vecInxUV;
    std::vector<GLushort>        vecInxV;
    std::vector<GLushort>        vecInxOptz;
    GLuint                       n_vecInx;
   // GLuint                       n_vecInxOptim;
    GLint                        array_buffer_size;
    std::vector<IFS>             Triangles;

    MYTEXTURE*                  pTextures;
    //std::map<GLuint,GLuint>     VPind_to_VUind;
    std::map<GLuint,GLuint>     VUinx_to_VTinx;

    GLuint                      stride;
    std::vector<GLfloat>        vecVBO;
    GLuint                      vboIds[2];
    std::vector<GLint>          attribs;
    GLint                       attribPos;
    GLint                       attribNor;
    GLint                       attribTex;
public:
    GLchar                      o_name[64]; //o
    glm::vec3                   location;
    GLuint                      n_vecInxOptim;
    btRigidBody		            *btrigidbody;
    GLint                       texCount;
    GLboolean 			useVt;// = false;
   // GLboolean 			useSmoothNor;// Vn supercedes s


public:
    MESH(GLchar* name){
        strcpy(o_name, name);
        n_vecVPos = 0;  n_vecInx = 0; n_vecVNorm = 0;
        attribPos = attribNor = attribTex = - 10;
        mode = GL_TRIANGLES;
        n_vecInxOptim = 0;
    }
    ~MESH() {
    }
    GLint Mode() { return mode; }

   // void SetShdrProg(GLchar* prog_name);
    void BindTexture(MYTEXTURE* pTexture);
    bool LoadTextures(GLchar*  apkFilePath, MYTEXTURE* pTexture, GLuint texUnit, GLuint flags, GLchar filter, GLfloat anisotropic_filter);
    bool OptimizeMesh(GLuint vertex_cache_size, std::vector<GLushort> &vecIndeces);
    void UpdateBoundMesh();
    void BuildVBO();
    void BindLoadBuffers(bool tex);
    void BindBuffers();
    void SetMeshAttribs( MYPROGRAM *prg);
    void DrawMesh();
    GLushort* GetV_InxBuffer();
    std::vector<GLushort> GetUV_InxBuffer();
    GLushort* GetOptimInxBuffer();
    //std::map<GLuint,GLuint>& GetV_toVU_Map() { return VPind_to_VUind; };
    std::map<GLuint,GLuint>& GetUV_toV_Map() { return  VUinx_to_VTinx; };
    std::vector<glm::vec3>& GetVPosBuffer();
    std::vector<glm::vec3>& GetVNormBuffer();
    std::vector<glm::vec2>& GetVTexBuffer();
    GLuint VPosBufferSize() { return n_vecVPos; }
    GLuint TexBufferSize() { return vecVTexCoords.size(); }
    GLuint NormBufferSize() { return  n_vecVNorm; }
    GLuint InxBufferSize();// { return  n_vecInx; }
    std::vector<IFS> GetTriangles() { return Triangles;}
    glm::vec3 GetMeshDim() { return dimension; }

};
class GObjects{
    std::map<std::string, GLint>    LookUpMesh;
    GLint                           numMesh;
public:
    std::vector<MESH*>              GObjs;
    std::map<std::string, MYPROGRAM*>   mProgs;
    OBJ_MATERIAL                    Material;
    bool                            isMTL;
    GLchar                          MtlLib[64]; //mtllib

    GObjects() { isMTL = false; numMesh = 0;}
    ~GObjects();
    GLint LookupMeshNum(std::string mesh_name) { return LookUpMesh[mesh_name]; }
    void AddProgram(std::string name, MYPROGRAM* prg) { mProgs[name] = prg; }
    MYPROGRAM* GetProgram(std::string name) { return mProgs[name]; }
    bool ReadOBJ(GLchar*  apkFilePath, GLchar* obj_name);
    bool ReadMTL(GLchar*  apkFilePath, GLchar* mtl_name);
    GLchar* MtlFileName() { return &MtlLib[0]; }
};

#endif //DUMMY_MYOBJ_H
