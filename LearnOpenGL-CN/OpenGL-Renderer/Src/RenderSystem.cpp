#include "RenderSystem.h"


RenderSystem::~RenderSystem()
{
    
}


void RenderSystem::initialize()
{
    initModels();
    initOpenGLObjects();
    initTextures();
    initLogic();
    initSkybox();
    initShaders(); // 暂时放这里,防止内存泄漏
}

void RenderSystem::tick(float delta_time)
{
    draw();
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
    
    VAOs.resize(count);
    VBOs.resize(count);
    EBOs.resize(count);
    
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
    unsigned int texture;
    texture = loadTexture("./Textures/container2.png");
    textures.push_back(texture);
}

void RenderSystem::initShaders()
{
    shader_constructor_ptr = std::make_shared<ShaderConstructor>("./Shaders/VS.vert", "./Shaders/FS.frag");
}

void RenderSystem::initLogic()
{
//    postprocess_ptr = std::make_shared<PostProcess>(viewport_width, viewport_height, window_sys->post_process_info);
}

void RenderSystem::initSkybox()
{
    std::vector<std::string> faces = {
        "./Textures/skybox/right.jpg",
        "./Textures/skybox/left.jpg",
        "./Textures/skybox/top.jpg",
        "./Textures/skybox/bottom.jpg",
        "./Textures/skybox/front.jpg",
        "./Textures/skybox/back.jpg",
    };
    
    skybox_ptr = std::make_shared<Skybox>(faces);
}

void RenderSystem::updateLogic()
{
    auto window = window_sys->getWindow();
    glfwGetWindowSize(window, &window_width, &window_height);
    
    viewport_width = window_width;
    viewport_height = window_height;
    
#ifdef __APPLE__
    viewport_width *= 2;
    viewport_height *= 2;
#endif
}

void RenderSystem::draw()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    auto& sc = shader_constructor_ptr;
    sc->use();
    sc->setInt("gMat.DiffuseTexture", 0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    
    glm::vec4 clear_color = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto& camera = window_sys->camera;

    //
    // Shared: Uniform
    //
    MVP mvp;
    mvp.view = camera.GetViewMatrix();
    mvp.projection = glm::perspective(glm::radians(camera.Zoom), (float)viewport_width / viewport_height, 0.1f, 100.f);
    sc->setMat4("view", glm::value_ptr(mvp.view));
    sc->setMat4("projection", glm::value_ptr(mvp.projection));

    glm::vec4 gAmbientLight = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
    sc->setVec4("gAmbientLight", glm::value_ptr(gAmbientLight));

    const int MaxLights = 16;
    Light lights[MaxLights];
    lights[0].Strength = glm::vec3(0.6f, 0.6f, 0.6f);
    lights[0].Direction = glm::vec3(0.57735f, -0.57735f, 0.57735f);
    lights[1].Strength = glm::vec3(0.3f, 0.3f, 0.3f);
    lights[1].Direction = glm::vec3(-0.57735f, -0.57735f, 0.57735f);
    lights[2].Strength = glm::vec3(0.15f, 0.15f, 0.15f);
    lights[2].Direction = glm::vec3(0.0f, -0.707f, -0.707f);
    sc->setVec3("gLights[0].Strength", glm::value_ptr(lights[0].Strength));
    sc->setVec3("gLights[0].Direction", glm::value_ptr(lights[0].Direction));
    sc->setVec3("gLights[1].Strength", glm::value_ptr(lights[1].Strength));
    sc->setVec3("gLights[1].Direction", glm::value_ptr(lights[1].Direction));
    sc->setVec3("gLights[2].Strength", glm::value_ptr(lights[2].Strength));
    sc->setVec3("gLights[2].Direction", glm::value_ptr(lights[2].Direction));

    sc->setVec3("gViewPos", glm::value_ptr(camera.Position));


    //
    // Skull
    //
    mvp.model = glm::mat4(1.0);
    mvp.model = glm::translate(mvp.model, glm::vec3(0.f, 0.f, 0.f));
    mvp.model = glm::rotate(mvp.model, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
    mvp.model = glm::scale(mvp.model, glm::vec3(1.f));
    sc->setMat4("model", glm::value_ptr(mvp.model));

    Material mat;
    mat.FresnelR0 = glm::vec3(0.05f);
    mat.Roughness = 0.3f;
    mat.DiffuseAlbedo = glm::vec4(0.69f, 0.77f, 0.87f, 1.0f);
    sc->setVec4("gMat.DiffuseAlbedo", glm::value_ptr(mat.DiffuseAlbedo));
    sc->setVec3("gMat.FresnelR0", glm::value_ptr(mat.FresnelR0));
    sc->setFloat("gMat.Roughness", mat.Roughness);

    glBindVertexArray(VAOs[0]);
    glDrawElements(GL_TRIANGLES, int(models[0].indices.size()), GL_UNSIGNED_INT, 0);
    
    drawSkybox();
}

void RenderSystem::drawSkybox()
{
    auto& camera = window_sys->camera;
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)viewport_width / viewport_height, 0.1f, 100.f);
    
    skybox_ptr->drawSkybox(view, projection);
}
