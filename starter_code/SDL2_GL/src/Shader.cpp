#include "Shader.hpp"

namespace AppCoreGui{

    Shader::Shader():
        loaded_vertex_shader(false), loaded_fragment_shader(false),
        using_default_vertex_shader(true), using_default_fragment_shader(true),
        compiled(false)
    {}

    Shader::Shader(const std::filesystem::path vertex_shader_path, const std::filesystem::path fragment_shader_path):
        loaded_vertex_shader(false), loaded_fragment_shader(false),
        using_default_vertex_shader(true), using_default_fragment_shader(true),
        compiled(false)
    {
        if(!loadFromFile(vertex_shader_path,Type::VERTEX)){
            std::cerr<<"Failed to load vertex shader from "<<vertex_shader_path<<std::endl;
        }
        if(!loadFromFile(fragment_shader_path,Type::FRAGMENT)){
            std::cerr<<"Failed to load vertex shader from "<<vertex_shader_path<<std::endl;
        }
    }

    bool Shader::loadFromFile(const std::filesystem::path vertex_shader_path, const std::filesystem::path fragment_shader_path)
    {
        if(!loadFromFile(vertex_shader_path,Type::VERTEX)){
            std::cerr<<"Failed to load vertex shader from "<<vertex_shader_path<<std::endl;
            return false;
        }
        if(!loadFromFile(fragment_shader_path,Type::FRAGMENT)){
            std::cerr<<"Failed to load vertex shader from "<<vertex_shader_path<<std::endl;
            return false;
        }
        return true;
    }

    bool Shader::loadFromFile(const std::filesystem::path shader_source, Type shader_type)
    {
        std::string source;
        std::ifstream source_file;
        source_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try{
            source_file.open(shader_source.string());
            std::stringstream ss;
            ss<<source_file.rdbuf();
            source_file.close();
            source = ss.str();
        }
        catch(std::ifstream::failure e){
            std::cerr<<"Shader::loadFromFile: Failed to read source file: "<<shader_source<<std::endl;
            return false;
        }

        const char* source_ptr = source.c_str();

        unsigned int* target_shader;
        if(shader_type == Type::VERTEX){
            target_shader = &vshader;
            *target_shader = glCreateShader(GL_VERTEX_SHADER);            
            glShaderSource(*target_shader,1,&source_ptr,nullptr);
            using_default_vertex_shader = false;
            loaded_vertex_shader = true;
        }
        else if(shader_type == Type::FRAGMENT){
            target_shader = &fshader;
            *target_shader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(*target_shader,1,&source_ptr,nullptr);
            using_default_fragment_shader = false;
            loaded_fragment_shader = true;
        }
        else{
            std::cerr<<"Shader::loadFromFile: Unsupported shader type value: "<<static_cast<int>(shader_type)<<std::endl;
            return false;
        }       
        return true;
    }

    bool Shader::loadFromMemory(const std::string& vertex_shader, const std::string& fragment_shader)
    {
        if(!loadFromMemory(vertex_shader,Type::VERTEX)){
            std::cerr<<"Shader::loadFromMemory: Failed to load vertex shader!"<<std::endl;
            return false;
        }
        if(!loadFromMemory(fragment_shader,Type::FRAGMENT)){
            std::cerr<<"Shader::loadFromMemory: Failed to load fragment shader!"<<std::endl;
            return false;
        }
        return true;
    }

    bool Shader::loadFromMemory(const std::string& shader_source, Type shader_type)
    {
        unsigned int* target_shader;
        const char* source_ptr = shader_source.c_str();
        if(shader_type == Type::VERTEX){
            target_shader = &vshader;
            *target_shader = glCreateShader(GL_VERTEX_SHADER);            
            glShaderSource(*target_shader,1,&source_ptr,nullptr);
            using_default_vertex_shader = false;
            loaded_vertex_shader = true;
        }
        else if(shader_type == Type::FRAGMENT){
            target_shader = &fshader;
            *target_shader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(*target_shader,1,&source_ptr,nullptr);
            using_default_fragment_shader = false;
            loaded_fragment_shader = true;
        }
        else{
            std::cerr<<"Shader::loadFromFile: Unsupported shader type value: "<<static_cast<int>(shader_type)<<std::endl;
            return false;
        }       
        return true;
    }

    bool Shader::compile()
    {
        compiled = false;
        if(using_default_vertex_shader){
            if(!loadFromMemory(default_vertex_shader,Type::VERTEX)){
                std::cerr<<"Shader::compile: Failed to load default vertex shader!"<<std::endl;
                return false;
            }
            using_default_vertex_shader = true;
        }
        if(using_default_fragment_shader){
            if(!loadFromMemory(default_fragment_shader,Type::FRAGMENT)){
                std::cerr<<"Shader::compile: Failed to load default fragment shader!"<<std::endl;
                return false;
            }
            using_default_fragment_shader = true;
        }

        int success;
        char info_log[512];
        
        glCompileShader(vshader);
        glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(vshader,512,nullptr,info_log);
            std::cerr<<"Shader::compile: Vertex shader compile error: "<<info_log<<std::endl;
            return false;
        }
        success=0;

        glCompileShader(fshader);
        glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(fshader,512,nullptr,info_log);
            std::cerr<<"Shader::compile: Fragment shader compile error: "<<info_log<<std::endl;
            return false;
        }
        success = 0;

        ID = glCreateProgram();
        glAttachShader(ID, vshader);
        glAttachShader(ID,fshader);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success){
            glGetProgramInfoLog(ID,512,nullptr,info_log);
            std::cerr<<"Shader::compile: Error linking shader: "<<info_log<<std::endl;
            return false;
        }

        glDeleteShader(vshader);
        glDeleteShader(fshader);

        compiled=true;

        return true;

    }

    void Shader::activate()
    {
        if(compiled){
            glUseProgram(ID);
        }
        else std::cerr<<"Error activating shader!"<<std::endl;
    }

    void Shader::setBool(const std::string& name, bool value)
    {
        glUniform1i(glGetUniformLocation(ID,name.c_str()),static_cast<int>(value));
    }

    void Shader::setInt(const std::string& name, int value)
    {
        glUniform1i(glGetUniformLocation(ID,name.c_str()),value);
    }

    void Shader::setFloat(const std::string& name, float value)
    {
        glUniform1f(glGetUniformLocation(ID,name.c_str()),value);
    }

    void Shader::setMat4(const std::string& name, Mat4f value)
    {
        glUniformMatrix4fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,glm::value_ptr(value));
    }

    unsigned int Shader::getID()
    {
        return ID;
    }

    bool Shader::getCompiled()
    {
        return compiled;
    }
};