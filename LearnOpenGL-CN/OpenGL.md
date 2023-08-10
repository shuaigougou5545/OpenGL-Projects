# OpenGL 3.3

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308071545046.png" alt="截屏2023-08-07 15.45.14" style="zoom:50%;" />

## S1 前言

### 1.GLFW & GLAD

#### （1）GLFW

GLFW：Graphics Library Framework，用于**创建窗口和处理用户输入**，特别用于OpenGL图形库（创建和管理OpenGL上下文），旨在提供跨平台图形编程

为图形编程者忽略不同操作系统相关的底层逻辑

#### （2）GLAD

GLAD：OpenGL Loader Generator，用于自动生成OpenGL函数指针加载代码，帮助开发者便携地加载和管理OpenGL函数

**OpenGL动态查询机制**：

- OpenGL驱动版本众多，**大多数函数的位置无法在编译时确定下来**，需要在运行时查询 => 开发人员在运行时通过查询某函数是否支持，并获取函数的地址，以确保应用程序的兼容性
- GLAD旨在简化这个过程

### 2.立即渲染模式 & 核心模式

#### （1）立即渲染模式

采用“固定渲染管线”

优点：使用一系列函数调用完成图形的绘制，简单，快速实现；

缺点：在复杂场景下效率低，因为每次绘制涉及大量的函数调用和状态的更改

#### （2）核心模式

引入着色器和VBO（顶点缓冲对象）等概念

### 3.OpenGL上下文 & 状态机

#### （1）上下文

OpenGL上下文（Context）：充当**操作系统与图形硬件的桥梁**，负责**管理OpenGL的状态和资源**，处理图形渲染和绘制指令

使用步骤：创建上下文 -> 设置并绑定上下文 -> 执行OpenGL指令 -> 销毁上下文

OpenGL中可以同时出现多个上下文，但每个线程或进程中只能有一个当前上下文；绑定上下文：其实就是将上下文绑定到正确的线程或进程上

#### （2）状态机

- 状态设置函数

- 状态使用函数

### 4.对象

OpenGL对象是指一些选项的集合，类似于一个struct结构体

使用对象：创建对象,同时保存对象的id(引用) -> 绑定对象到上下文 -> 用函数修改对象的选项 -> 解绑对象

## S2 OpenGL渲染

### 1.基本流程

#### （0）流程梳理

- GLFW
  - glfw初始化
  - 创建glfw窗口，绑定opengl上下文
- GLAD
  - glad初始化
- OpenGL渲染设置
  - 视口（viewport），注册回调函数检测窗口变动而修改视口
- OpenGL渲染主循环
  - 交换颜色缓冲区
  - glfw轮询事件

- GLFW
  - glfw析构（释放资源）


#### （1）GLFW初始化

```cpp
glfwInit();
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
```

`GLFW_OPENGL_FORWARD_COMPAT`：前向兼容 - 在此是指不再支持老版本所丢弃的特性

前向兼容: forward  - 在某一平台的较低版本环境中编写的程序可以在较高版本的环境中运行【向未来兼容】
后向兼容: backward - 【向过去兼容】

#### （2）创建窗口&绑定上下文

```cpp
GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL-01", NULL, NULL);
if(!window){
    glfwTerminate();
    return -1;
}
glfwMakeContextCurrent(window);
```

#### （3）GLAD初始化

```cpp
if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    return -1;
}
```

**gladLoadGLLoader**函数需要传入一个函数，这里传入的glfw库的**glfwGetProcAddress**函数，该函数可以根据名字查找符合当前操作系统的对应OpenGL函数

#### （4）视口

视口：OpenGL渲染窗口的大小，渲染窗口应该比GLFW创建的系统窗口维度要小

```cpp
glViewport(0, 0, 800, 600);
```

形参类似Rect2，前两参数以左下角为起点，后两参数为宽高

注意：glfw创建的系统窗口可以通过鼠标进行大小变动，但视口不会随之变动，所以需要手动设置，我们可以通过glfw库的**glfwSetFramebufferSizeCallback**函数，为glfw窗口变动时绑定回调函数，改动视口

```cpp
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
```

=> 这里使用的是`...Framebuffer...`是指帧缓冲大小，是指存储图像渲染结果的尺寸大小，帧缓冲在前，随后才会呈现在屏幕上 -- 这里没用`...Window...`，可能原因是：“默认情况下，GLFW会尝试匹配窗口的大小和帧缓冲的大小，以保持它们的一致性”，所以这里帧缓冲和窗口基本无差异；而且帧缓冲与视口应该才是对应变动关系

⚠️这里还必须注册该回调函数以更新视口，不然MAC平台运行报错，可能原因是视口没随窗口大小更新产生问题

#### （5）交换颜色缓冲区

```cpp
glfwSwapBuffers(window);
```

双缓冲机制，颜色缓冲是一个存储着GLFW窗口每一个像素颜色的缓冲区，最终也会呈现在GLFW窗口上，所以这里是glfw函数

#### （6）glfw轮询事件

```cpp
glfwPollEvents();
```

查看是否有glfw触发的事件

#### （7）GLFW释放资源

```cpp
glfwTerminate();
```

需要初始化，也就应当要释放资源
