# README

For OpenGL Learning

-------

#### 创建OpenGL-Xcode工程步骤：

1. **链接** - `Library Search Paths` => 引入`.lib/.a`文件【**lib文件夹**】

   - ```shell
     $(PROJECT_DIR)/../../3rd_party/GLFW/lib
     ```

     non-recursive

2. **头文件** - `Header Search Paths` => 引入头文件，之后可以通过`#include <...>`引入

   - ```shell
     $(PROJECT_DIR)/../../3rd_party/GLAD/include
     $(PROJECT_DIR)/../../3rd_party/GLFW/include
     ```

     non-recursive

3. **源文件** - glad没有.lib或.a文件，所以需要自行将.c文件引入工程共同编译

   - `add files to '...'` => `Create folder references`，并勾选`Add to targets`

4. **其他"动态库"**

   - <img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308061034203.png" alt="工程配置" style="zoom:40%;" />

