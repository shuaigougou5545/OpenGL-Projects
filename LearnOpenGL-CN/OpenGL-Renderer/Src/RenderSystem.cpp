#include "RenderSystem.h"

namespace rd {

void RenderSystem::initialize()
{
    initModels();
    initOpenGLObjects();
    initTextures();
    initOpenGLStatus();
    initShaders();
}

void RenderSystem::run()
{
    auto sc = shader_constructors[0];
    sc.use();
    sc.setInt("gMat.DiffuseTexture", 0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    auto window = window_sys->getWindow();
    glm::vec4 clear_color = glm::vec4(0.1f, 0.5f, 0.2f, 1.0f);

    float deltaTime = 0.f, lastFrame = 0.f;
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto& camera = window_sys->camera;

        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);

        //
        // Shared: Uniform
        //
        auto sc = shader_constructors[0];
        glm::mat4 model = glm::mat4(1.0), view = glm::mat4(1.0), projection = glm::mat4(1.0);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)window_width / window_height, 0.1f, 100.f);
        sc.setMat4("view", glm::value_ptr(view));
        sc.setMat4("projection", glm::value_ptr(projection));

        glm::vec4 gAmbientLight = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
        sc.setVec4("gAmbientLight", glm::value_ptr(gAmbientLight));

        const int MaxLights = 16;
        Light lights[MaxLights];
        lights[0].Strength = glm::vec3(0.6f, 0.6f, 0.6f);
        lights[0].Direction = glm::vec3(0.57735f, -0.57735f, 0.57735f);
        lights[1].Strength = glm::vec3(0.3f, 0.3f, 0.3f);
        lights[1].Direction = glm::vec3(-0.57735f, -0.57735f, 0.57735f);
        lights[2].Strength = glm::vec3(0.15f, 0.15f, 0.15f);
        lights[2].Direction = glm::vec3(0.0f, -0.707f, -0.707f);
        sc.setVec3("gLights[0].Strength", glm::value_ptr(lights[0].Strength));
        sc.setVec3("gLights[0].Direction", glm::value_ptr(lights[0].Direction));
        sc.setVec3("gLights[1].Strength", glm::value_ptr(lights[1].Strength));
        sc.setVec3("gLights[1].Direction", glm::value_ptr(lights[1].Direction));
        sc.setVec3("gLights[2].Strength", glm::value_ptr(lights[2].Strength));
        sc.setVec3("gLights[2].Direction", glm::value_ptr(lights[2].Direction));

        sc.setVec3("gViewPos", glm::value_ptr(camera.Position));


        //
        // Skull
        //
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.f, 0.f, 0.f));
        model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        model = glm::scale(model, glm::vec3(1.f));
        sc.setMat4("model", glm::value_ptr(model));

        Material mat;
        mat.FresnelR0 = glm::vec3(0.05f);
        mat.Roughness = 0.3f;
        mat.DiffuseAlbedo = glm::vec4(0.69f, 0.77f, 0.87f, 1.0f);
        sc.setVec4("gMat.DiffuseAlbedo", glm::value_ptr(mat.DiffuseAlbedo));
        sc.setVec3("gMat.FresnelR0", glm::value_ptr(mat.FresnelR0));
        sc.setFloat("gMat.Roughness", mat.Roughness);

        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, int(models[0].indices.size()), GL_UNSIGNED_INT, 0);
        //        glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());

        glfwSwapBuffers(window);
    }
}

void RenderSystem::shutdown()
{
    
}

void RenderSystem::initModels()
{
    Model skull("./Models/skull.txt");
    models.push_back(skull);
}

void RenderSystem::initOpenGLObjects()
{
    int count = (int)models.size();
    glGenVertexArrays(count, VAOs.data());
    glGenBuffers(count, VBOs.data());
    glGenBuffers(count, EBOs.data());
    
    for(int i = 0; i < count; i++)
    {
        glBindVertexArray(VAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        
        glBufferData(GL_ARRAY_BUFFER, models[i].vertices_vbo.size() * sizeof(float), models[i].vertices_vbo.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, models[i].indices.size() * sizeof(unsigned int), models[i].indices.data(), GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void RenderSystem::initTextures()
{
//    unsigned int texture;
//    texture = loadTexture("./Textures/container2.png");
//    textures.push_back(texture);
}

void RenderSystem::initOpenGLStatus()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void RenderSystem::initShaders()
{
    ShaderConstructor sc("./Shaders/VS.vert", "./Shaders/FS.frag");
    shader_constructors.push_back(sc);
}

}
