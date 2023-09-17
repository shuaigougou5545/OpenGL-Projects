#include "RenderSystem.h"


RenderSystem::~RenderSystem()
{
    
}


void RenderSystem::initialize()
{
    initModels();
    initOpenGLObjects();
    initUniformBlocks();
    initTextures();
    initLogic();
    initSkybox();
    initNormalVisualization();
    initShaders(); // 暂时放这里,防止内存泄漏
}

void RenderSystem::tick(float delta_time)
{
    updateLogic();
    updateUniformBlocks();
    draw();
    drawNormalVisualization();
    drawSkybox();
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
        
        // pos normal
        size_t pos_size = models[i].pos_list.size() * sizeof(glm::vec3);
        size_t normal_size = models[i].normal_list.size() * sizeof(glm::vec3);
        size_t size = pos_size + normal_size;
        
        // batched data
        glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, pos_size, models[i].pos_list.data());
        glBufferSubData(GL_ARRAY_BUFFER, pos_size, normal_size, models[i].normal_list.data());
        
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, models[i].indices.size() * sizeof(unsigned int), models[i].indices.data(), GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)pos_size);
        glEnableVertexAttribArray(1);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void RenderSystem::initUniformBlocks()
{
    GLint binding_point = 0;
    
    // cbPerObject [binding_point: 0]
    cb_per_object_list.resize(models.size());
    
    glGenBuffers(1, &ubo_cbPerObject);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_cbPerObject);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(cbPerObject), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo_cbPerObject);
    
    // cbPass [binding_point: 1]
    binding_point++;
    glGenBuffers(1, &ubo_cbPass);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_cbPass);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(cbPass), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo_cbPass);
}

void RenderSystem::initTextures()
{
    unsigned int texture;
    texture = loadTexture("./Textures/container2.png");
    textures.push_back(texture);
}

void RenderSystem::initShaders()
{
    shader_constructor_ptr = std::make_shared<ShaderConstructor>("./Shaders/vs.vert", "./Shaders/fs.frag");
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

void RenderSystem::initNormalVisualization()
{
    normalvisualization_ptr = std::make_shared<NormalVisualization>();
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
    
    camera = &window_sys->camera;
}

void RenderSystem::updateUniformBlocks()
{
    // cbPass [binding_point: 1]
    cb_pass.view = camera->GetViewMatrix();
    cb_pass.projection = glm::perspective(glm::radians(camera->Zoom), (float)viewport_width / viewport_height, 0.1f, 100.f);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_cbPass);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(cbPass), &cb_pass);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RenderSystem::draw()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    auto& sc = shader_constructor_ptr;
    sc->use();
    sc->setUniformBlock("cbPerObject", 0);
    sc->setUniformBlock("cbPass", 1);
    sc->setInt("gMat.DiffuseTexture", 0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    
    glm::vec4 clear_color = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


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

    sc->setVec3("gViewPos", glm::value_ptr(camera->Position));


    //
    // Skull
    //
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0.f, 0.f, 0.f));
    model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
    model = glm::scale(model, glm::vec3(1.f));
    
    cb_per_object_list[0].model = model;
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_cbPerObject);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(cb_per_object_list[0]), &cb_per_object_list[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    

    Material mat;
    mat.FresnelR0 = glm::vec3(0.05f);
    mat.Roughness = 0.3f;
    mat.DiffuseAlbedo = glm::vec4(0.69f, 0.77f, 0.87f, 1.0f);
    sc->setVec4("gMat.DiffuseAlbedo", glm::value_ptr(mat.DiffuseAlbedo));
    sc->setVec3("gMat.FresnelR0", glm::value_ptr(mat.FresnelR0));
    sc->setFloat("gMat.Roughness", mat.Roughness);

    glBindVertexArray(VAOs[0]);
    glDrawElements(GL_TRIANGLES, int(models[0].indices.size()), GL_UNSIGNED_INT, 0);
}

void RenderSystem::drawNormalVisualization()
{
    if(!window_sys->normal_visualization)
        return;
    
    normalvisualization_ptr->use_shader(window_sys->normal_visualization_len);
    
    // TODO: 整理一下，有点乱
    normalvisualization_ptr->sc_ptr->setUniformBlock("cbPerObject", 0);
    normalvisualization_ptr->sc_ptr->setUniformBlock("cbPass", 1);

    glBindVertexArray(VAOs[0]);
    glDrawElements(GL_TRIANGLES, int(models[0].indices.size()), GL_UNSIGNED_INT, 0);
}

void RenderSystem::drawSkybox()
{
    glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)viewport_width / viewport_height, 0.1f, 100.f);
    
    skybox_ptr->drawSkybox(view, projection);
}


