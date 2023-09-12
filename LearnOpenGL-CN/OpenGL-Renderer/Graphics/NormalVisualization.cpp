#include "NormalVisualization.h"


NormalVisualization::NormalVisualization()
{
    initShaders();
}

NormalVisualization::~NormalVisualization()
{
      
}

void NormalVisualization::initShaders()
{
    const std::string vsPath = "./Shaders/NormalVisualization/normal_visualization_vs.vert";
    const std::string gsPath = "./Shaders/NormalVisualization/normal_visualization_gs.geom";
    const std::string fsPath = "./Shaders/NormalVisualization/normal_visualization_fs.frag";
    
    sc_ptr = std::make_shared<ShaderConstructor>(vsPath, fsPath, "", "", gsPath);
}

void NormalVisualization::use_shader(float normal_len)
{
    sc_ptr->use();
    sc_ptr->setFloat("normal_length", normal_len);
}

