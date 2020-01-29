#include "DemoApp.h"
#include <errno.h>

#include "../framework/MyProgram.h"
#include "../framework/GLMath.h"
#include "../framework/GLES.h"

#include "../framework/0ldFont.h"
#include "../framework/MyOBJ.h"
#include "../framework/bullet/btDefaultCollisionConfiguration.h"
#include "../framework/bullet/btCollisionDispatcher.h"
#include "../framework/bullet/btBroadphaseInterface.h"
#include "../framework/bullet/btConstraintSolver.h"
#include "../framework/bullet/btDiscreteDynamicsWorld.h"
#include "../framework/bullet/btSequentialImpulseConstraintSolver.h"
#include "../framework/bullet/btDbvtBroadphase.h"
#include "../framework/bullet/btDefaultMotionState.h"
#include "../framework/bullet/btBoxShape.h"
#include "../framework/Timing.h"
#include "../framework/bullet/btConeShape.h"
#include "../framework/bullet/btCylinderShape.h"


#define VERTEX_SHADER ( char * )"ShdrPosNorm.vers"
#define FRAGMENT_SHADER ( char * )"ShdrPosNorm.frag"

#define DEBUG_SHADERS 1
#define FONTT (char* )"foo.ttf"
char msg1[100] ;

glm::vec2 touch (0.0f, 0.0f);
glm::vec3 rot_angle  (0.0f, 0.0f, 0.0f);
GLint viewport_matrix[ 4 ];

MYPROGRAM* prg;
GObjects GOs;
OLDFONT* oldf = NULL;
MYTRANS trans;
glm::mat4 model = glm::mat4(1.0f);

//////////////////////////////////////////////////Bullet
btDefaultCollisionConfiguration *collisionconfiguration = NULL;
btCollisionDispatcher *dispatcher = NULL;
btBroadphaseInterface *broadphase = NULL;
btConstraintSolver *solver = NULL;
btDiscreteDynamicsWorld *dynamicsworld = NULL;


///////////////////////////////////////////////
void UniformsRed(bool bSampler){
    GLint uniform;
     uniform = prg->PROGRAM_get_uniform_location((GLchar *) "MODELVIEWPROJECTIONMATRIX");
     glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(trans.GetPRJmat() * trans.GetMVmat() * trans.GetMODmat()));

    uniform = prg->PROGRAM_get_uniform_location((GLchar *) "PROJECTIONMATRIX");
    glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(trans.GetPRJmat()));

    uniform = prg->PROGRAM_get_uniform_location((GLchar *) "NORMALMATRIX");
    glUniformMatrix3fv(uniform, 1, GL_FALSE, glm::value_ptr(trans.GetNormalmat()));

    uniform = prg->PROGRAM_get_uniform_location((GLchar *) "MODELVIEWMATRIX");
    glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(trans.GetMVmat() * trans.GetMODmat()));

    uniform = prg->PROGRAM_get_uniform_location((GLchar *) "LIGHTPOSITION");
    glm::vec4 Lpos4 = trans.GetVmat() * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 Lposit = glm::vec3(Lpos4);
    glUniform3fv(uniform, 1, (float *) &Lposit);
    if(bSampler) {
        uniform = prg->PROGRAM_get_uniform_location((GLchar *) "DIFFUSE");
        glUniform1i(uniform, 0);
    }
}
//////////////////////////////////////////////////////////////////////////////
void init_physic_world( void )
{
    collisionconfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher( collisionconfiguration );
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsworld = new  btDiscreteDynamicsWorld( dispatcher, broadphase, solver, collisionconfiguration );
    dynamicsworld->setGravity( btVector3( 0.0f, 0.0f, -9.8f ) );
}
void add_rigid_body( MESH *objmesh, glm::vec3 location, GLfloat mass, GLfloat angle )
{
    btCollisionShape *btcollisionshape = NULL;
    if(0 == strcmp(objmesh->o_name, "Plane"))
            btcollisionshape = new btBoxShape( btVector3( objmesh->GetMeshDim().x * 0.5f, objmesh->GetMeshDim().y * 0.5f, objmesh->GetMeshDim().z * 0.5f ) );
    else if(0 == strcmp(objmesh->o_name, "Cone"))
            btcollisionshape = new btConeShape(btScalar(0.5f),btScalar(1.5f));
    else if(0 == strcmp(objmesh->o_name, "Cube"))
            btcollisionshape = new btBoxShape( btVector3(1.0f * 0.5f, 1.0f * 0.5f, 1.0f * 0.5f ) );
    else if(0 == strcmp(objmesh->o_name, "Cylinder"))
            btcollisionshape = new btCylinderShape( btVector3(1.0f * 0.5f, 1.5f * 0.5f, 1.0f * 0.5f ) );

    btTransform bttransform;
    glm:: mat4 mat(0.0f);
    glm:: mat4 model(1.0f);

    glm::vec3 rotx ( 1.0f, 0.0f, 0.0f),
            roty ( 0.0f, 1.0f, 0.0f),
            rotz (0.0f, 0.0f, 1.0f);

    mat[0][0] = mat[1][1] = mat[2][2] = mat[3][3] = 1.0f;

      mat = mat * glm::rotate( model, glm::radians(angle), rotz);
      mat = mat * glm::rotate( model, glm::radians(angle),roty );
      mat = mat * glm::rotate( model, glm::radians(angle),rotx );
      mat = mat * glm::translate( model, location);

    bttransform.setFromOpenGLMatrix( ( GLfloat * )&mat );
    btDefaultMotionState *btdefaultmotionstate = NULL;
    btdefaultmotionstate = new btDefaultMotionState( bttransform );

    btVector3 localinertia( 0.0f, 0.0f, 0.0f );
    if(0 != strcmp(objmesh->o_name, "Plane" )){
        btcollisionshape->calculateLocalInertia( mass, localinertia );
    }

    objmesh->btrigidbody = new btRigidBody( mass, btdefaultmotionstate, btcollisionshape, localinertia );
    objmesh->btrigidbody->setUserPointer( objmesh );
    if( mass )
        objmesh->btrigidbody->setCollisionFlags( objmesh->btrigidbody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK );

    dynamicsworld->addRigidBody( objmesh->btrigidbody );
}
bool contact_added_callback( btManifoldPoint &btmanifoldpoint, const btCollisionObject *btcollisionobject0, int part_0, int index_0,
                             const btCollisionObject *btcollisionobject1, int part_1, int index_1 )
{


    MESH *objmesh0 = ( MESH * )( ( btRigidBody * )btcollisionobject0 )->getUserPointer();
    MESH *objmesh1 = ( MESH * )( ( btRigidBody * )btcollisionobject1 )->getUserPointer();
    if(0 == strcmp(objmesh1->o_name, "Cylinder")){
        static float dx = 99999.0f;
        static float dy = 99999.0f;
        static float dz = 99999.0f;
        if(abs(btmanifoldpoint.m_normalWorldOnB.x() - dx) < 0.000001f && abs(btmanifoldpoint.m_normalWorldOnB.y()-dy) < 0.000001f &&
                abs(btmanifoldpoint.m_normalWorldOnB.z()-dz) < 0.000001f)
            sprintf(&msg1[0], "GAME OVER");
        else{
            dx = btmanifoldpoint.m_normalWorldOnB.x();
            dy = btmanifoldpoint.m_normalWorldOnB.y();
            dz = btmanifoldpoint.m_normalWorldOnB.z();
        }
    }
    return false;
}
void free_physic_world( void )
{
    while( dynamicsworld->getNumCollisionObjects() )
    {
        btCollisionObject *btcollisionobject = dynamicsworld->getCollisionObjectArray()[ 0 ];
        btRigidBody *btrigidbody = btRigidBody::upcast( btcollisionobject );

        if( btrigidbody )
        {
            delete btrigidbody->getCollisionShape();
            delete btrigidbody->getMotionState();
            dynamicsworld->removeRigidBody( btrigidbody );
            dynamicsworld->removeCollisionObject( btcollisionobject );
            delete btrigidbody;
        }
    }

    delete collisionconfiguration; collisionconfiguration = NULL;
    delete dispatcher; dispatcher = NULL;
    delete broadphase; broadphase = NULL;
    delete solver; solver = NULL;
    delete dynamicsworld; dynamicsworld = NULL;
}
/////////////////////////////////////////////////////////////////

void DemoAppInit(GLchar*  apkFilePath, GLint width, GLint height ) {

    GLchar buffer[256];
    strcpy(buffer, apkFilePath);
    atexit(DemoAppExit);
    sprintf(&msg1[0], "");
    GLES::GLES_Init();
    init_physic_world();
    gContactAddedCallback = contact_added_callback;

    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glGetIntegerv(GL_VIEWPORT, viewport_matrix);

    trans.SetMmatrixMode(PROJECTION_MAT);
    trans.SetIdentity();
    trans.SetPerspective(120.0f, (GLfloat) width, (GLfloat) height, 0.01f, 100.0f);

    glm::vec3 eye (5.0f, 5.0f, 5.0f);
    glm::vec3 cent (0.0f, 0.0f, 0.0f);
    glm::vec3 up (0.0f, 0.0f, 1.0f);
    trans.SetMmatrixMode(MODEL_VIEW_MAT);
    trans.SetIdentity();
    trans.SetLookAt(eye, cent, up);
////////////////////////////////////////////////////////////////////////////// font init
    oldf = new OLDFONT(FONTT);
    if (0 != oldf->FONT_init(buffer))
        exit(1);
    GLfloat fsize;
    if(width <= 320) fsize = 48.0f;
    else if(width <= 480) fsize = 64.0f;
    else fsize = 80.f;
    if (0 != oldf->FONT_load(buffer, oldf->name, fsize, 512, 512, 32, 96))
        exit(1);

    //////////////////////////////////////////////////////////////////////////////////////////shader program
    prg = new MYPROGRAM((GLchar *) "VertOnlyProg");
    if (0 != prg->PROGRAM_create(VERTEX_SHADER, FRAGMENT_SHADER, buffer, DEBUG_SHADERS))
        exit(1);
    if (0 != prg->PROGRAM_link(DEBUG_SHADERS))
        exit(2);
    GOs.AddProgram(std::string("VertOnlyProg"), prg);

    if (!GOs.ReadOBJ(buffer, (GLchar *) "plane2.obj"))
        exit(2);
    GLint last_mesh = GOs.LookupMeshNum(std::string( "Plane"));
    GOs.GObjs[last_mesh]->UpdateBoundMesh();
    GOs.GObjs[last_mesh]->BuildVBO();
    GOs.GObjs[last_mesh]->BindLoadBuffers(false);
    GOs.GObjs[last_mesh]->SetMeshAttribs(prg);

    add_rigid_body( GOs.GObjs[last_mesh], glm::vec3(0.0f, 0.0f, 0.0f),  0.0f, 0.0f );

    if (!GOs.ReadOBJ(buffer, (GLchar *) "cubeVertOnly.obj"))
        exit(2);
    last_mesh = GOs.LookupMeshNum(std::string( "Cube"));
    GOs.GObjs[last_mesh]->UpdateBoundMesh();
    GOs.GObjs[last_mesh]->BuildVBO();
    GOs.GObjs[last_mesh]->BindLoadBuffers(false);
    GOs.GObjs[last_mesh]->SetMeshAttribs(prg);

    add_rigid_body( GOs.GObjs[last_mesh], glm::vec3(-7.0f, 5.0f, 7.0f), 1.5f, 35.0f );

    if (!GOs.ReadOBJ(buffer, (GLchar *) "cone.obj"))
        exit(2);
    last_mesh = GOs.LookupMeshNum(std::string( "Cone"));
    GOs.GObjs[last_mesh]->UpdateBoundMesh();
    GOs.GObjs[last_mesh]->BuildVBO();
    GOs.GObjs[last_mesh]->BindLoadBuffers(false);
    GOs.GObjs[last_mesh]->SetMeshAttribs(prg);

    add_rigid_body( GOs.GObjs[last_mesh], glm::vec3(-5.0f, 7.0f, 9.0f), 0.5f, 35.0f );

    if (!GOs.ReadOBJ(buffer, (GLchar *) "cylinder.obj"))
        exit(2);
    last_mesh = GOs.LookupMeshNum(std::string( "Cylinder"));
    GOs.GObjs[last_mesh]->UpdateBoundMesh();
    GOs.GObjs[last_mesh]->BuildVBO();
    GOs.GObjs[last_mesh]->BindLoadBuffers(false);
    GOs.GObjs[last_mesh]->SetMeshAttribs(prg);

    add_rigid_body( GOs.GObjs[last_mesh], glm::vec3(-4.0f, 9.0f, 11.0f), 1.0f, 35.0f );

}


void DemoAppDraw( void )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    TimingData::get()->update();
    trans.SetMmatrixMode(MODEL_VIEW_MAT);
    trans.PushMatrix();
    trans.Rotate(rot_angle.x, glm::vec3(1.0f, 0.0f, 0.0f));
    trans.Rotate(rot_angle.z, glm::vec3(0.0f, 0.0f, 1.0f));

    prg = GOs.GetProgram(std::string("VertOnlyProg"));
    glUseProgram(prg->GetPID());
    GLint mesh_ind = GOs.LookupMeshNum(std::string( "Plane"));
    GOs.GObjs[mesh_ind]->BindBuffers();
    glm::mat4 mat1;
    GOs.GObjs[mesh_ind]->btrigidbody->getWorldTransform().getOpenGLMatrix(&mat1[0][0] );
    trans.MultiplyMat4(mat1);
    UniformsRed(false);
    GOs.GObjs[mesh_ind]->DrawMesh();
    trans.PopMatrix();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//////////////////////////////////////////////////////////////////////////////
    float delta = TimingData::get()->averageFrameDuration;
///////////////////////////////////////////////////////////////////////////
    trans.SetMmatrixMode(MODEL_VIEW_MAT);
    trans.PushMatrix();
    trans.Rotate(rot_angle.x, glm::vec3(1.0f, 0.0f, 0.0f));
    trans.Rotate(rot_angle.z, glm::vec3(0.0f, 0.0f, 1.0f));
    prg = GOs.GetProgram(std::string("VertOnlyProg"));
    glUseProgram(prg->GetPID());
    mesh_ind = GOs.LookupMeshNum(std::string("Cube"));
    GOs.GObjs[mesh_ind]->BindBuffers();
    glm::mat4 mat2;
    GOs.GObjs[mesh_ind]->btrigidbody->getWorldTransform().getOpenGLMatrix(&mat2[0][0]);
    trans.MultiplyMat4(mat2);
    UniformsRed(false);
    GOs.GObjs[mesh_ind]->DrawMesh();
    trans.PopMatrix();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
/////////////////////////////////////////////////////////////////////////
    trans.SetMmatrixMode(MODEL_VIEW_MAT);
    trans.PushMatrix();
    trans.Rotate(rot_angle.x, glm::vec3(1.0f, 0.0f, 0.0f));
    trans.Rotate(rot_angle.z, glm::vec3(0.0f, 0.0f, 1.0f));
    prg = GOs.GetProgram(std::string("VertOnlyProg"));
    glUseProgram(prg->GetPID());
    mesh_ind = GOs.LookupMeshNum(std::string("Cone"));
    GOs.GObjs[mesh_ind]->BindBuffers();
    glm::mat4 mat3;
    GOs.GObjs[mesh_ind]->btrigidbody->getWorldTransform().getOpenGLMatrix(&mat3[0][0]);
    trans.MultiplyMat4(mat3);
    UniformsRed(false);
    GOs.GObjs[mesh_ind]->DrawMesh();
    trans.PopMatrix();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//////////////////////////////////////////////////////////////////////////////////
    trans.SetMmatrixMode(MODEL_VIEW_MAT);
    trans.PushMatrix();
    trans.Rotate(rot_angle.x, glm::vec3(1.0f, 0.0f, 0.0f));
    trans.Rotate(rot_angle.z, glm::vec3(0.0f, 0.0f, 1.0f));
    prg = GOs.GetProgram(std::string("VertOnlyProg"));
    glUseProgram(prg->GetPID());
    mesh_ind = GOs.LookupMeshNum(std::string("Cylinder"));
    GOs.GObjs[mesh_ind]->BindBuffers();
    glm::mat4 mat4;
    GOs.GObjs[mesh_ind]->btrigidbody->getWorldTransform().getOpenGLMatrix(&mat4[0][0]);
    trans.MultiplyMat4(mat4);
    UniformsRed(false);
    GOs.GObjs[mesh_ind]->DrawMesh();
    trans.PopMatrix();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    /////////////////////////////////////////////////////////////////////////////// font draw
    trans.SetMmatrixMode(PROJECTION_MAT);
    trans.PushMatrix();
    trans.SetIdentity();
    float half_width  = ( float )viewport_matrix[ 2 ] * 0.5f;
    float	half_height = ( float )viewport_matrix[ 3 ] * 0.5f;
    trans.SetOrthographic2D(-half_width, half_width, -half_height, half_height);
    trans.Translate(glm::vec3(-half_width, -half_height, 0.0f ));
    glm::vec3 scl (0.5f, 0.5f, 0.5f);
    trans.Scale(scl);
    glm::vec4 color (1.0f, 1.0f, 1.0f, 1.0f );
    glUseProgram(oldf->fontprg->GetPID());
    oldf->FONT_print(half_width, half_height*3, &msg1[0], trans.GetPRJmat(), color);
    trans.PopMatrix();
/////////////////////////////////////////////////////////////////////////
   dynamicsworld->stepSimulation( delta, 1, 1.0f / 60.0f );
}
void DemoAppTouchBegan( float x, float y, int tap_count ) {
    touch.x = x;
    touch.y = y;
}

void DemoAppTouchMoved( float x, float y, int tap_count ) {
    rot_angle.z += -( touch.x - x )/20.0f;
    rot_angle.x += -( touch.y - y )/20.0f;
    touch.x = x;
    touch.y = y;
}
void DemoAppTouchEnded( float x, float y, int tap_count ){
    touch.x = 0.0f;
    touch.y = 0.0f;
}
void DemoAppExit( void )
{
   free_physic_world();
    delete prg;
    delete oldf;
}


