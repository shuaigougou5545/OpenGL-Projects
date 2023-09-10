#include "Skybox.h"
#include "LoadTexture.h"
#include <string>


Skybox::Skybox(const std::vector<std::string>& vs) : faces(vs)
{
    is_dirty = true;
    initOpenGLObjects();
    initShaders();
    update();
}

Skybox::~Skybox()
{

}

void Skybox::initOpenGLObjects()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(float), skyboxVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Skybox::initShaders()
{
    const std::string vsPath = "./Shaders/skybox_vs.vert";
    const std::string fsPath = "./Shaders/skybox_fs.frag";
    
    sc_ptr = std::make_shared<ShaderConstructor>(vsPath, fsPath);
}

void Skybox::setFaces(const std::vector<std::string>& vs)
{
    if(vs.size() != 6)
    {
        std::cout << "ERROR [Skybox]: Skybox cubemap load failed" << std::endl;
        return;
    }
    
    for(int i = 0; i < vs.size(); i++)
    {
        if(vs[i] != faces[i])
        {
            is_dirty = true;
            break;
        }
    }
    
    if(is_dirty)
    {
        faces = vs;
        update();
    }
}

void Skybox::drawSkybox(const glm::mat4& view, const glm::mat4& projection)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    sc_ptr->use();
    sc_ptr->setInt("cubemap", 0);
    sc_ptr->setMat4("view", glm::value_ptr(view));
    sc_ptr->setMat4("projection", glm::value_ptr(projection));
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    // reset to the default state
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void Skybox::update()
{
    texture = loadCubemap(faces);
    is_dirty = false;
}
