# README

For OpenGL Learning

`./LearnOpenGL-CN/OpenGL-Render`：<font color='red'>**OpenGL tiny renderer**</font>

`./LearnOpenGL-CN/OpenGL-Project-Template`：**OpenGL开发模版，用于快速创建工程**

-------

#### 创建OpenGL-Xcode工程步骤：

1. **链接** - `Library Search Paths` => 引入`.lib/.a`文件【**lib文件夹**】

   - ```shell
     $(SRCROOT)/../../3rd_party/GLFW/lib
     ```

     non-recursive

2. **头文件** - `Header Search Paths` => 引入头文件，之后可以通过`#include <...>`引入

   - ```shell
     $(SRCROOT)/../../3rd_party/GLAD/include
     $(SRCROOT)/../../3rd_party/GLFW/include
     $(SRCROOT)/../../3rd_party/GLM/include
     ```
     
     non-recursive

3. **源文件** - glad没有.lib或.a文件，所以需要自行将.c文件引入工程共同编译

   - `add files to '...'` => `Create folder references`，并勾选`Add to targets`

4. **其他"动态库"**

   - <img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308061034203.png" alt="工程配置" style="zoom:40%;" />

<font color='red'>**必须用到的框架**</font>：

- **IOKit.framework**：用于与硬件设备交互和管理硬件资源，允许开发者在应用程序中访问和控制设备硬件，从而实现与外部设备的通信、配置和监控 => OpenGL程序涉及OpenGL上下文，图形硬件，显示器等，必须利用此框架
- **Cocoa.framework**：用于创建图形界面、处理用户输入、管理窗口、控制应用生命周期等功能，提供丰富的图形用户界面和交互体验 => OpenGL程序涉及窗口管理，OpenGL上下文，事件管理，必须利用此框架
- **libglfw3.a**：【**这个需要手动拖入**】

这些框架需要手动配置，Mac这么设置是为了程序的灵活性和更大的控制权

另外两个框架【**可不加**】：

- OpenGL.framework：因为我们用了glad库，和这个框架作用一样，所以可以不加这个框架
- GLUT.framework：GLUT也是一个管理窗口，便于OpenGL程序开发的库，跟GLFW库作用相同，因为我们引入了GLFW库，所以可以不加这个框架

##### <font color='green'>Imgui</font>：

- 需要用到的文件有：

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308151141496.png" alt="截屏2023-08-15 11.40.59" style="zoom:50%;" />

- 去imgui官网下载文件夹，其中根目录下的文件是必须的，因为我们是OpenGL3+glfw环境，所以还需要backends文件夹下5个文件：`imgui_impl_glfw.cpp/.h`、`imgui_impl_opengl3.cpp/.h`、`imgui_impl_opengl3_loader.h`，整理好放在一个文件夹下后，通过`Create group`加入到xcode工程中即可

- imgui相关用法参考`examples`文件夹下`...opengl3...`的`main.cpp`文件

- 头文件包含：

  ```cpp
  #include "imgui.h"
  #include "imgui_impl_glfw.h"
  #include "imgui_impl_opengl3.h"
  ```

---

#### 🚌快速创建工程：

为了方便，提供了模版工程`OpenGL-Project-Template`，复制模版工程的文件夹，先修改文件夹名，打开工程，进入工程设置，点击PROJECT，在右侧修改Name（会一并修改该工程中所有相关名字）

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308111926101.png" alt="截屏2023-08-11 19.26.02" style="zoom:30%;" />
