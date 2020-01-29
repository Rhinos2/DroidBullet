//
//
//

#include "MyProgram.h"
#include <android/log.h>
#include  "ZRreader.h"


GLshort MYPROGRAM::PROGRAM_create(GLchar *vertex_shader_filename, GLchar *fragment_shader_filename, GLchar*  apkFilePath, bool debug_shader) {

    ZReader zreader1;
    zreader1.Init(apkFilePath);
    if(0 != zreader1.ZOpen(vertex_shader_filename, true)){
        __android_log_print(ANDROID_LOG_ERROR, "", "[ %s ]\n%s", name, "Error reading ver zip file.");
        return -1;
    }
    vertex_shader.SHADER_init(vertex_shader_filename, GL_VERTEX_SHADER );
    if(1 != vertex_shader.SHADER_compile( zreader1.ZGetBuffer(), debug_shader )){
        __android_log_print(ANDROID_LOG_ERROR, "", "[ %s ]\n%s", name, "Error reading compiling ver shader file.");
        return -1;
    }
    zreader1.ZClose();
    zreader1.Init(apkFilePath);
    if(0 != zreader1.ZOpen(fragment_shader_filename, true)){
        __android_log_print(ANDROID_LOG_ERROR, "", "[ %s ]\n%s", name, "Error reading frag zip file.");
        return -1;
    }

    fragment_shader.SHADER_init(fragment_shader_filename, GL_FRAGMENT_SHADER );
    if(1 != fragment_shader.SHADER_compile( zreader1.ZGetBuffer(), debug_shader)){
        __android_log_print(ANDROID_LOG_ERROR, "", "[ %s ]\n%s", name, "Error reading frag zip file.");
        return -1;
    }

    return 0;
}
///////////////////////////////////
GLshort MYPROGRAM::PROGRAM_link(bool debug ){
    if(this->pid)
        return -1; //already exists

    pid = glCreateProgram(); //empty program object
    glAttachShader(pid, vertex_shader.sid);
    glAttachShader(pid, fragment_shader.sid);
    GLint program_linked;
    glLinkProgram(pid);

    glGetProgramiv(pid, GL_LINK_STATUS, &program_linked);
    if (debug) {
        if(program_linked != GL_TRUE)
        {
            GLsizei log_length = 0;
            GLchar message[1024];
            glGetProgramInfoLog(pid, 1024, &log_length, message);

            __android_log_print(ANDROID_LOG_ERROR, "", "[ %s ]\n%s", name, message);

        }


    }
    if (program_linked != GL_TRUE) {
        glDeleteProgram(pid);
        pid = 0;
        return -1;
    }
    GLint program_validated;
    glValidateProgram(pid);
    glGetProgramiv(pid, GL_VALIDATE_STATUS, &program_validated);
    if(debug){
        if(program_validated != GL_TRUE){
            GLsizei log_length = 0;
            GLchar message[1024];
            glGetProgramInfoLog(pid, 1024, &log_length, message);

            __android_log_print(ANDROID_LOG_ERROR, "", "[ %s ]\n%s", name, message);

        }

    }
    if(program_validated != GL_TRUE){
        glDeleteProgram(pid);
        pid = 0;
        return -1;
    }
    // add vertex attributes
    GLint num_attribs;
    glGetProgramiv(pid, GL_ACTIVE_ATTRIBUTES, &num_attribs);
    GLchar attrib_name[256];
    GLsizei length;
    GLint size;
    GLenum type;

    for (int i = 0; i < num_attribs; i++){
        glGetActiveAttrib(pid, (GLuint)i, sizeof(attrib_name), &length, &size, &type, attrib_name);
        vertex_attrib_count++;
        MYVERTEX_ATTRIB ver_attr;
        strcpy(ver_attr.name, attrib_name);
        ver_attr.type = type;
        ver_attr.location = glGetAttribLocation(pid, attrib_name);
        vertex_attrib_array.push_back(ver_attr);

    }
    //add uniforms
    GLint num_uniforms;
    GLchar uniform_name[256];

    glGetProgramiv(pid, GL_ACTIVE_UNIFORMS, &num_uniforms);
    for (int i = 0; i < num_uniforms; i++){
        glGetActiveUniform(pid, (GLuint)i, sizeof(uniform_name), &length, &size, &type, uniform_name);
        uniform_count++;
        MYUNIFORM unif;
        strcpy(unif.name, uniform_name);
        unif.type = type;
        unif.location = glGetUniformLocation(pid, uniform_name);
        uniform_array.push_back(unif);
    }
    return 0;
}
/////////////////////////end of link func
GLint MYPROGRAM::PROGRAM_get_vertex_attrib_location( GLchar *att_name ){
    std::vector<MYVERTEX_ATTRIB>::iterator pos;
    for(pos = vertex_attrib_array.begin(); pos < vertex_attrib_array.end(); pos++){
        if(0 == strcmp(pos->name, att_name))
            return pos->location;
    }
    return -1;
}
GLint MYPROGRAM::PROGRAM_get_uniform_location( GLchar *un_name ){
    std::vector<MYUNIFORM>::iterator pos;
    for(pos = uniform_array.begin(); pos < uniform_array.end(); pos++){
        if(0 == strcmp(pos->name, un_name))
            return pos->location;
    }
    return -1;
}


