#pragma once

#include "RenderShapes.hpp"

namespace AppCoreGui{
    Shader BaseShape::default_shader = Shader();
    Shader BaseShape::default_outline_shader = Shader();

    BaseShape::BaseShape()
    {
        if(!default_shader.getCompiled()){
            std::cout<<"compiling default shader"<<std::endl;
            default_shader.compile();
        }
        if(!default_outline_shader.getCompiled()){
            std::cout<<"compiling default outline shader"<<std::endl;
            default_outline_shader.compile();
        }
        fill_color = Color3{0,128,255};
        outline_color = Color3(255,255,0);
        outline_thickness = 1.0f;
        texture_loaded = false;
        texture_refresh = true;
        has_alpha_channel = false;
    }

    bool BaseShape::setFillColor(std::filesystem::path path)
    {
        if(!std::filesystem::is_regular_file(path)){
            return false;
        }
        texture_refresh = true;
        texture_path = path;
        //int width=0, height=0, channels=0;
        //unsigned char* data = stbi_load(path.string().c_str(),&width,&height,&channels,0);
        //stbi_image_free(data);
        //return width>0 && height>0 && channels>0;
        return true;
    }

    RectangleShape::RectangleShape(float width, float height) :
        rect_width(width), rect_height(height)
    {}

    RectangleShape::RectangleShape(Vec2f size) : rect_width(size.x), rect_height(size.y)
    {}

    void RectangleShape::setSize(float width, float height)
    {
        rect_width = width;
        rect_height = height;
    }

    void RectangleShape::setSize(Point2f size)
    {
        rect_width = size.x;
        rect_height = size.y;
    }

    void RectangleShape::create()
    {
        VAOs.clear();
        VAOs.emplace_back(0);

        unsigned int VBO;
        unsigned int EBO;
        
        //TODO: Scale based on width and height
        std::vector<float> vertices = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f
        };
        std::vector<uint32_t> indices = {
            0,1,3,
            1,2,3
        };

        std::vector<float> vertex_colors = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 0.0f
        };

        std::vector<float> texture_coords = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

        std::vector<float> combined_vertex_data;

        for(int i=0, j=0, k=0;i<vertices.size(); i+=3,j+=3,k+=2){
            combined_vertex_data.emplace_back(vertices[i]);
            combined_vertex_data.emplace_back(vertices[i+1]);
            combined_vertex_data.emplace_back(vertices[i+2]);

            combined_vertex_data.emplace_back(vertex_colors[j]);
            combined_vertex_data.emplace_back(vertex_colors[j+1]);
            combined_vertex_data.emplace_back(vertex_colors[j+2]);

            combined_vertex_data.emplace_back(texture_coords[k]);
            combined_vertex_data.emplace_back(texture_coords[k+1]);
        }

        glGenVertexArrays(1,&VAOs[0]);
        glGenBuffers(1,&VBO);

        //Vertex Buffer
        glBindVertexArray(VAOs[0]);
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER, combined_vertex_data.size()*sizeof(float),combined_vertex_data.data(),GL_STATIC_DRAW);
        
        //Position Attribute
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);

        //Vertex Color Attribute
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),reinterpret_cast<void*>(3*sizeof(float)));
        glEnableVertexAttribArray(1);

        //Texture Coordinate Attribute
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),reinterpret_cast<void*>(6*sizeof(float)));
        glEnableVertexAttribArray(2);

        //Element Buffer
        glGenBuffers(1,&EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(float),indices.data(),GL_STATIC_DRAW);

        //Textures
        if(!texture_loaded){
            glGenTextures(1,&texture);
            glBindTexture(GL_TEXTURE_2D,texture);
            
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            texture_loaded=true;
        }
        glBindVertexArray(0);

        num_elements.emplace_back(indices.size());
        
    }

    void RectangleShape::draw(SDL_Window* window,SDL_GLContext* ctx)
    {
        SDL_GL_MakeCurrent(window,*ctx);

        default_shader.activate();

        Point3f color;
        color.x = static_cast<float>(fill_color.x)/255.0f;
        color.y = static_cast<float>(fill_color.y)/255.0f;
        color.z = static_cast<float>(fill_color.z)/255.0f;
        glUniform4f(glGetUniformLocation(default_shader.getID(),"color"),color.x,color.y,color.z,1.0f);
        default_shader.setFloat("use_vertex_colors",(show_vertex_colors?1.0f:0.0f));
        default_shader.setFloat("use_texture",(show_shaded? show_vertex_colors?0.5:1.0f :0.0f));
        //Set Transform
        default_shader.setMat4("transform",getRootComponent().getLocalTransformMatrix());
        
        glBindVertexArray(VAOs[0]);
        glPolygonMode(GL_FRONT_AND_BACK,(wireframe?GL_LINE:GL_FILL));
        if(texture_refresh){
            int width, height, channels;
            stbi_set_flip_vertically_on_load(true);
            unsigned char* data = stbi_load(texture_path.string().c_str(),&width,&height,&channels,0);
            if(data){
                if(channels==4)has_alpha_channel=true;
                else has_alpha_channel=false;
                std::cout<<"Has alpha: "<<has_alpha_channel<<std::endl;
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,(has_alpha_channel?GL_RGBA:GL_RGB),GL_UNSIGNED_BYTE,data);
                glGenerateMipmap(GL_TEXTURE_2D);
                texture_refresh = false;
            }
            else{
                std::cout<<"Failed to load texture: "<<texture_path<<std::endl;
            }
            stbi_image_free(data);
        }
        glBindTexture(GL_TEXTURE_2D,texture);
        glDrawElements(GL_TRIANGLES,num_elements[0],GL_UNSIGNED_INT,0);
    }

};