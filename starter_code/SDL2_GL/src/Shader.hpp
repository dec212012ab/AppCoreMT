#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Alias.hpp"

namespace AppCoreGui{

class Shader{
public:
    enum class Type{
        VERTEX,
        FRAGMENT,
        //GEOMETRY//TODO
    };
    Shader();
    Shader(const std::filesystem::path vertex_shader_path, const std::filesystem::path fragment_shader_path);
    
    bool loadFromFile(const std::filesystem::path vertex_shader_path, const std::filesystem::path fragment_shader_path);
    bool loadFromFile(const std::filesystem::path shader_file_path, Type shader_type);
    bool loadFromMemory(const std::string& vertex_shader, const std::string& fragment_shader);
    bool loadFromMemory(const std::string& shader_source, Type shader_type);

    bool compile();
    void activate();
    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setMat4(const std::string& name, Mat4f value);

    unsigned int getID();
    bool getCompiled();

private:
    bool compiled;
    bool loaded_vertex_shader;
    bool loaded_fragment_shader;
    bool using_default_vertex_shader;
    bool using_default_fragment_shader;
    unsigned int ID;
    unsigned int vshader;
    unsigned int fshader;
    
    static const inline std::string default_vertex_shader = "#version 460 core\n"
        "layout (location=0) in vec3 vertex_pos;\n"
        "layout (location=1) in vec3 vertex_color_in;\n"
        "layout (location=2) in vec2 tex_coord;\n"
        
        "uniform mat4 transform;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        
        "out vec3 frag_vertex_color;\n"
        "out vec2 frag_tex_coord;\n"
        
        "void main(){\n"
        "   gl_Position = projection * view * transform * vec4(vertex_pos,1.0);\n"
        "   frag_vertex_color = vertex_color_in;\n"
        "   frag_tex_coord=tex_coord;\n"
        "}";
    static const inline std::string default_fragment_shader = "#version 460 core\n"
        "out vec4 fragment_color;\n"
        "in vec3 frag_vertex_color;\n"
        "in vec2 frag_tex_coord;\n"
        "uniform float use_vertex_colors=0.0;\n"
        "uniform float use_texture=0.0;\n"
        "uniform sampler2D frag_texture;\n"
        "uniform vec4 color;"
        "void main(){\n"
        "   fragment_color = mix(color,vec4(frag_vertex_color,1.0),use_vertex_colors);\n"
        "   fragment_color = mix(fragment_color,texture(frag_texture,frag_tex_coord),use_texture);\n"
        "}";
};

};