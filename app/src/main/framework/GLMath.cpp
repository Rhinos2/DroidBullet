//
//
//

#include "GLMath.h"


MYTRANS::MYTRANS(){
    mat_mode = NOT_SET; MV_pointer = PRJ_pointer = MOD_pointer = 0;
    model = glm::mat4(1.0f);
    glm::mat4 init = glm::mat4(0.0f);
    MVm.push_back(init);
    MVm.push_back(init); // MV 4 levels
    MVm.push_back(init);
    MVm.push_back(init);
    PRJm.push_back(init); //PRJ 2 levels
    PRJm.push_back(init);
    MODm.push_back(model);
    MODm.push_back(model);

}

void MYTRANS::SetIdentity(){
    switch (mat_mode) {
        case MODEL_VIEW_MAT: {
            MVm[MV_pointer][0][1] = MVm[MV_pointer][0][2] = MVm[MV_pointer][0][3] =
            MVm[MV_pointer][1][0] = MVm[MV_pointer][1][2] = MVm[MV_pointer][1][3] =
            MVm[MV_pointer][2][0] = MVm[MV_pointer][2][1] = MVm[MV_pointer][2][3] =
            MVm[MV_pointer][3][0] = MVm[MV_pointer][3][1] = MVm[MV_pointer][3][2] = 0.0f;
            MVm[MV_pointer][0][0] = MVm[MV_pointer][1][1] = MVm[MV_pointer][2][2] = MVm[MV_pointer][3][3] = 1.0f;
            break;
        }
        case PROJECTION_MAT: {
            PRJm[PRJ_pointer][0][1] =  PRJm[PRJ_pointer][0][2] =  PRJm[PRJ_pointer][0][3] =
            PRJm[PRJ_pointer][1][0] =  PRJm[PRJ_pointer][1][2] =  PRJm[PRJ_pointer][1][3] =
            PRJm[PRJ_pointer][2][0] =  PRJm[PRJ_pointer][2][1] =  PRJm[PRJ_pointer][2][3] =
            PRJm[PRJ_pointer][3][0] =  PRJm[PRJ_pointer][3][1] =  PRJm[PRJ_pointer][3][2] = 0.0f;
            PRJm[PRJ_pointer][0][0] = PRJm[PRJ_pointer][1][1] = PRJm[PRJ_pointer][2][2] = PRJm[PRJ_pointer][3][3] = 1.0f;
            break;
        }
        case MODEL_MAT: {
            MODm[MOD_pointer][0][1] =  MODm[MOD_pointer][0][2] =  MODm[MOD_pointer][0][3] =
            MODm[MOD_pointer][1][0] =  MODm[MOD_pointer][1][2] =  MODm[MOD_pointer][1][3] =
            MODm[MOD_pointer][2][0] =  MODm[MOD_pointer][2][1] =  MODm[MOD_pointer][2][3] =
            MODm[MOD_pointer][3][0] =  MODm[MOD_pointer][3][1] =  MODm[MOD_pointer][3][2] = 0.0f;
            MODm[MOD_pointer][0][0] = MODm[MOD_pointer][1][1] = MODm[MOD_pointer][2][2] = MODm[MOD_pointer][3][3] = 1.0f;
            break;
        }
        default:
            break;
    }

}
glm::mat4& MYTRANS::GetInverseMV(){
    InvMV = glm::affineInverse(MVm[MV_pointer]);
    return InvMV;
}
glm::mat3& MYTRANS::GetNormalmat(){
    glm::mat4 normal = glm::inverseTranspose(MVm[MV_pointer] * MODm[MOD_pointer]);
    NORm = glm::mat3(normal);
    return NORm;
}
void MYTRANS::PushMatrix() {

    switch (mat_mode) {
        case MODEL_VIEW_MAT: {
            MVm[MV_pointer + 1] = MVm[MV_pointer];
            MV_pointer++;
            break;
        }
        case PROJECTION_MAT: {
            PRJm[PRJ_pointer + 1] = PRJm[PRJ_pointer];
            PRJ_pointer++;
            break;
        }
        case MODEL_MAT: {
            MODm[MOD_pointer + 1] = MODm[MOD_pointer];
            MOD_pointer++;
            break;
        }
        default:
            break;
    }

}

void MYTRANS::PopMatrix(){
    switch (mat_mode) {
        case MODEL_VIEW_MAT: {
            if(MV_pointer > 0)
                MV_pointer--;
            break;
        }
        case PROJECTION_MAT: {
            if(PRJ_pointer > 0)
                PRJ_pointer--;
            break;
        }
        case MODEL_MAT: {
            if(MOD_pointer > 0)
                MOD_pointer--;
            break;
        }
        default:
            break;
    }
}

void MYTRANS::SetPerspective(GLfloat fovy, GLfloat width, GLfloat height, GLfloat near,  GLfloat far){
    glm:: mat4 Projection = glm::perspectiveFov( glm::radians(fovy), width, height, near, far);
    PRJm[PRJ_pointer] = PRJm[PRJ_pointer] * Projection;

}
void MYTRANS::SetOrthographic(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,  GLfloat znear, GLfloat zfar){
    glm:: mat4 Projection =  glm::ortho(left, right, bottom, top, znear, zfar);
    PRJm[PRJ_pointer] = PRJm[PRJ_pointer] * Projection;

}
void MYTRANS::SetOrthographic2D(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top){
    glm:: mat4 Projection = glm::ortho(left, right, bottom, top);
    PRJm[PRJ_pointer] = PRJm[PRJ_pointer] * Projection;

}
void MYTRANS::SetLookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up){
    Vm =  glm::lookAt(eye, center, up);
    MVm[MV_pointer] = MVm[MV_pointer] * Vm;

}
void MYTRANS::Rotate(GLfloat angle, glm::vec3 axis ){
    glm::mat4 rotation = glm::rotate(model, glm::radians(angle), axis);
    switch (mat_mode) {
        case MODEL_VIEW_MAT: {
            MVm[MV_pointer] = MVm[MV_pointer] * rotation;
            break;
        }
        case PROJECTION_MAT: {
            PRJm[PRJ_pointer] = PRJm[PRJ_pointer] * rotation;
            break;
        }
        case MODEL_MAT: {
            MODm[MOD_pointer] = MODm[MOD_pointer] * rotation;
            break;
        }
        default:
            break;
    }
}
void MYTRANS::Translate(glm::vec3 v) {
    glm::mat4 translation = glm::translate(model, v);
    switch (mat_mode) {
        case MODEL_VIEW_MAT: {
            MVm[MV_pointer] = MVm[MV_pointer] * translation;
            break;
        }
        case PROJECTION_MAT: {
            PRJm[PRJ_pointer] = PRJm[PRJ_pointer] * translation;
            break;
        }
        case MODEL_MAT: {
            MODm[MOD_pointer] = MODm[MOD_pointer] * translation;
            break;
        }
        default:
            break;
    }

}
void MYTRANS::Scale(glm::vec3 v){
    glm::mat4 scale = glm::scale(model, v);
    switch (mat_mode) {
        case MODEL_VIEW_MAT: {
            MVm[MV_pointer] = MVm[MV_pointer] * scale;
            break;
        }
        case PROJECTION_MAT: {
            PRJm[PRJ_pointer] = PRJm[PRJ_pointer] * scale;
            break;
        }
        case MODEL_MAT: {
            MODm[MOD_pointer] = MODm[MOD_pointer] * scale;
            break;
        }
        default:
            break;
    }
}
void MYTRANS::MultiplyMat4(glm::mat4 m){
    switch (mat_mode) {
        case MODEL_VIEW_MAT: {
            MVm[MV_pointer] = MVm[MV_pointer] * m;
            break;
        }
        case PROJECTION_MAT: {
            PRJm[PRJ_pointer] = PRJm[PRJ_pointer] * m;
            break;
        }
        case MODEL_MAT: {
            MODm[MOD_pointer] = MODm[MOD_pointer] * m;
            break;
        }
        default:
            break;
    }
}