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
  - 视口（viewport）
- 注册GLFW回调函数
  - 检测窗口变动，以修改视口

- OpenGL渲染主循环
  - 渲染指令
    - 清理颜色缓冲

  - GLFW轮询事件查询
  - 交换颜色缓冲区

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

`GLFW_CONTEXT_VERSION_MAJOR`和`GLFW_CONTEXT_VERSION_MINOR`：设置OpenGL上下文 => ~~这里不要误写成`GLFW_VERSION_MAJOR`，以免GLFW初始化失败，随之会导致窗口创建失败~~

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

Proc：是procedure，过程、函数的缩写

🤔️<font color='red'>**这一步容易忘记，如果漏写，OpenGL函数就会报错！！**</font>

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

#### （8）清理缓冲区

```cpp
glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT);
```

有三个缓冲区：颜色缓冲、深度缓冲、模版缓冲 - `GL_COLOR_BUFFER_BIT`,`GL_DEPTH_BUFFER_BIT`,`GL_STENCIL_BUFFER_BIT`

首先通过`glClearColor`,`glClearDepth`,`glClearStencil`设置清空缓冲区对应的颜色值，再调用`glClear`选择要清理的缓冲区

`glClearColor`就是一个状态设置函数，`glClear`就是一个状态使用函数

### 2.VAO & VBO & EBO(IBO)

#### （0）区分三个对象

- VAO：Vertex Array Object - 顶点数组对象
- VBO：Vertex Buffer Object - 顶点缓冲对象
- EBO/IBO：Element/Index Buffer Object - 索引缓冲对象

#### （1）VBO

```cpp
unsigned int VBO;
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // vertices: float[]顶点数据
```

VBO的作用是将顶点数据一次性发送到显存（GPU内存）上

通过`glGenBuffers`创建缓冲对象后，需要绑定到对应对象上去，VBO的缓冲类型是`GL_ARRAY_BUFFER`，后续我们使用关于`GL_ARRAY_BUFFER`的缓冲调用，都是使用绑定在其上的VBO

`glBufferData`将用户数据复制到对应缓冲区中，第四个参数指定我们希望显卡如何管理给定的数据，主要意图就是便于GPU管理显存中存放的数据：

- `GL_STATIC_DRAW`：数据几乎不会改变 - 主要存储一次性的数据，比如模型的顶点数据
- `GL_DYNAMIC_DRAW`：数据周期性地改变，但频率不高
- `GL_STREAM_DRAW`：数据每次绘制都会改变

#### （2）EBO/IBO

EBO与VBO的函数调用基本一致，因为都是BO（Buffer Object）缓冲对象，唯一不同点是缓冲类型是`GL_ELEMENT_ARRAY_BUFFER`

#### （3）VAO

##### 顶点属性

```cpp
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
```

`glVertexAttribPointer`：解析顶点数据，这里用的`xxxPointer`是因为最后一个参数

- 参数1：对应Location值
- 参数2：顶点属性大小，比如vec3，则是3
- 参数3：数据类型 - GL_FLOAT对应vecn
- 参数4：是否希望数据被标准化
- 参数5：步长，每组之间的字节长度
- 参数6：偏移量，类型是`void*`

`glEnableAttribPointer`：参数是Location值

🤔️这里有多种函数，具有不同的后缀，比如`xxxPointer`,`xxx1f`。`xxxPointer`的作用是传递一组顶点属性数据，而`xxx1f`的作用是传递单个值；**为什么`xxxPointer`以pointer为后缀，我觉得原因可能是它的最后一个参数需要一个指向顶点属性数据的指针，这个指针用来指定数据存储的位置** --<font color='bblue'> **但是这里我们传递的是`(void*)0`，这是因为这是一个<u>特殊情况</u>，它会让OpenGL从当前绑定的顶点数组`GL_ARRAY_BUFFER`中获取顶点属性数据**</font> => 这里完全可以传入一个浮点数数组的指针

##### VAO

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308111741222.png" alt="截屏2023-08-11 17.41.36" style="zoom:50%;" />

VAO能够存储**顶点属性**相关的调用：

- `glEnableVertexAttribArray`和`glDisableVertexAttribArray`的调用
- 通过`glVertexAttribPointer`设置的顶点属性配置
- 通过`glVertexAttribPointer`调用与顶点属性关联的顶点缓冲对象（VBO），也会存储对应（EBO）

```cpp
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);
... // 存储相关操作:VBO的绑定,数据复制到VBO,顶点属性调用
glBindVertexArray(0);

// 渲染指令之前绑定对应VAO
glBindVertexArray(VAO);
```

#### （4）shader - VS & PS/FS

##### VS

```cpp
unsigned int VS;
VS = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(VS, 1, &vertexShaderSource, NULL);
glCompileShader(VS);
```

`glShaderSource`：第四个参数为字符串的长度，但若传入字符串是C风格字符串，则可以为"NULL"

🤔️这里`glCreateShader`是通过返回值传递ID的，之前`glGenBuffers`是通过传入引用来传递ID的，这么设计的原因可能是：GenBuffers可以一次性创建多个buffer，所以不选择通过返回值（不然就得返回数组）来得到ID，而CreateShader一次只能创建一个Shader，所以通过返回值来传递ID

`glShaderSource`将数据传递到VS对象上，这里第二个参数指定字符串的数量

shader编译失败，报错：检查shader语法错误

```cpp
int success;
char infoLog[512];
glGetShaderiv(VS, GL_COMPILE_STATUS, &success);
if(!success)
{
    glGetShaderInfoLog(VS, 512, NULL, infoLog);
    std::cout << "VS COMPILATION FAILED: \n" << infoLog << std::endl;
}
```

这里'iv'是int vector整数数组的缩写，因为很多状态位都是用整数来存储，所以这里查询的就是`GL_COMPILE_STATUS`的结果

##### PS

像素着色器同理，唯一不同是创建shader时是`GL_FRAGMENT_SHADER`

##### 着色器程序 shader program

```cpp
unsigned int SP;
SP = glCreateProgram();
glAttachShader(SP, VS);
glAttachShader(SP, PS);
glLinkProgram(SP);
```

着色器程序用于合并多个着色器，比如将VS和PS**链接**在一起，并检查是否合理

先将shader依次附给着色器程序，然后进行链接

着色器程序链接失败，报错：检查VS和PS输入输出是否匹配

```cpp
glGetProgramiv(SP, GL_LINK_STATUS, &success);
if(!success)
{
    glGetProgramInfoLog(SP, 512, NULL, infoLog);
    std::cout << "SHADER PROGRAM LINK FAILED: \n" << infoLog << std::endl;
}
```

链接成功后我们可以直接使用该程序对象：

```cpp
glUseProgram(SP);
```

链接到程序对象后，着色器对象就不再使用了，应该删除释放资源：

```cpp
glDeleteShader(VS);
glDeleteShader(PS);
```

#### （5）渲染指令

两个主要的渲染指令：`glDrawArrays`和`glDrawElements`

```cpp
glDrawArrays(GL_TRIANGLES, 0, 3);
```

参数依次是：图元拓扑，顶点数组的起始索引，绘制的顶点数

```cpp
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
```

参数依次是：图元拓扑，绘制的顶点数（索引个数，重复的顶点也会算进去），索引的类型，EBO中的偏移量【这里如果索引使用的`GL_ELEMENT_ARRAY_BUFFER`，则直接填偏移量即可，否则得填指针】

🤔️如何记忆`...Arrays`,`...Elements`：VBO对应的缓冲类型是`GL_ARRAY_BUFFER`，所以不用索引则是draw arrays；EBO对应的缓冲类型是`GL_ELEMENT_ARRAY_BUFFER`，所以用索引则是draw elements

#### （6）特殊case

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308111834238.png" alt="截屏2023-08-11 18.19.45" style="zoom:50%;" />

VAO存储顶点属性相关指令时，有一个特殊case，参考👆图：VAO不会存储VBO解绑操作，但会存储EBO解绑操作

🤔️产生这种case的可能原因是：VAO会记录最后一个被绑定到`GL_ELEMENT_ARRAY_BUFFER`的EBO，所以如果在此期间解绑了EBO但没绑定新的EBO，那么VAO当然不会具有正确的EBO配置；但VBO则不同，即使在VAO启用期间解绑VBO，VAO仍能知道这个VBO是与顶点属性所关联的，所以可以在VAO启用期间安全解绑VBO

🤨那我猜测：EBO和VBO的这种差异，可能是因为VBO往往只会设置一次，而EBO会设置多次，因为索引可能根据不同情况进行改变

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308111834710.png" alt="截屏2023-08-11 18.34.14" style="zoom:50%;" />

#### （7）其他渲染设置

##### 多边形渲染模式：线框模式、默认模式等

- **线框模式**

```cpp
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
```

- **默认模式**

```
glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
```

- 点模式

```cpp
glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
```

第一个参数用于指定要设置的面，可以是正面，背面，或者正背面；第二个参数指定渲染模式，包括绘制线（LINE），填充（FILL），绘制点（POINT）

### 3.GLSL

```cpp
#version 330 core
in type in_variable_name
in type in_variable_name
  
out type out_variable_name

uniform type uniform_variable_name
  
void main()
{
	// ...
}
```

#### （1）in & out

🤔️Godot-shader中使用`varying`关键词，用于在顶点着色器和像素着色器之间传递数据的关键词，与`out`关键词作用一致；`varying`和`out`在VS和PS之间传递数据时，都会进行**插值** 
=> `varying`是在较低的OpenGL版本中使用，`out`是在后来的的OpenGL版本（OpenGL 3.3及之后）中使用，<font color="red">**更推荐使用`in`,`out`关键词控制数据传递方式**</font>

每个着色器使用关键字`in`、`out`设定输入和输出，发送方着色器定义一个输出，则接收方着色器必须对应定义一个输入（**名称一样，类型一样**）

=> 特殊的是：顶点着色器的输入和片段着色器的输出

- 顶点着色器：是从顶点数据`layout(location = x)`中获取输入`in`
- 像素着色器：要求输出一个`vec4`的颜色值（变量名自定义）

我们能声明的顶点属性(Vertex Attribute)是有上限的，一般由硬件决定，但**至少有16个包含4分量的顶点属性可用**；可以通过`glGetIntergerv()`函数获取上限

#### （2）uniform关键词

```cpp
int ourColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
glUniform4f(ourColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
// 矩阵:【参数分别是:location、矩阵个数、矩阵是否需要转置(默认:列主序)、float数组指针】
glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value); 
```

CPU向GPU着色器发送数据有两种方式：顶点属性、uniform

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308122317893.png" alt="截屏2023-08-12 23.17.42" style="zoom:40%;" />

uniform变量使用方法：

1. shader中定义uniform变量
2. 查询：程序中利用函数`glGetUniformLocation`获取uniform变量的位置值
3. 设置：利用函数`glUniform...`设置对应位置处uniform变量的值 
   - uniform变量会一直保存它的数据，直到被重置或更新
   - 函数后缀基本只有i和f，所以bool值的话也需要用整数i

⚠️注意：设置uniform变量值时，必须调用`glUseProgram`，因为**设置的uniform变量是针对当前激活的着色器程序的**；查询位置值时，不需要`glUseProgram`，因为查询函数有两个参数，一个是着色器程序，一个是uniform变量名

#### （3）GLSL基础

基本数据类型：

- `int`,`float`,`double`,`uint`,`bool` - `uint`：unsigned int
- 向量
  - `vecn`
  - `bvecn`,`ivecn`,`uvecn`,`dvecn`

分量可以任意重组，并可以用作初始化：

```glsl
vec4 v2 = v1.xxzy;
vec4 v3 = vec4(v2.xyz, 1.0);
```

### 4.纹理

#### （0）纹理使用基本流程

```cpp
//
// texture
//
unsigned int texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);

// 纹理环绕方式和过滤方式,都是设置到当前绑定对象上
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

int width, height, nrChannels;
unsigned char *data = stbi_load("./Textures/wall.jpg", &width, &height, &nrChannels, 0);
if(data)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
else
{
    std::cout << "ERROR [texture]: failed to load texture" << std::endl;
}
stbi_image_free(data);
```

#### （1）纹理环绕方式 - wrap

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
```

处理纹理坐标范围(0,0)~(1,1)之外的情况，环绕方式有：

- `GL_REPEAT`：重复
- `GL_MIRRORED_REPEAT`：镜像重复
- `GL_CLAMP_TO_EDGE`：clamp
- `GL_CLAMP_TO_BORDER`：超出范围以指定颜色

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308131225122.png" alt="截屏2023-08-13 12.25.28" style="zoom:40%;" />

纹理坐标：s、t、r（3D坐标）=> **OpenGL能为纹理的各个坐标轴分别设置环绕方式**

`glTexParameteri`：用于设置整数相关的纹理参数，`...f`：用于设置浮点数相关的纹理参数，比如各向异性度参数；`...fv`：用于设置浮点数数组相关的纹理参数，比如设置`GL_CLAMP_TO_BORDER`相关颜色

#### （2）纹理过滤 - filter

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

纹理过滤：如何将纹理坐标映射到对应纹理像素

- `GL_NEAREST`：邻近过滤
- `GL_LINEAR`：线性过滤

考虑两种情况：**放大`Magnify`、缩小`Minify`** => 比如，放大是指物体很大，但纹理分辨率很小（也就是人走近物体）=> **OpenGL能分别为纹理的放大和缩小两种情况分别设置纹理过滤方式**

🤔️纹理过滤往往处理的是放大的情况，放大后需要线性过滤，不然像素过渡不平滑情况被放大了

#### （3）多级渐远纹理（Mipmap）

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

⚠️**注意：mipmap只针对“缩小”`min`操作才有用，<font color='red'>对放大操作没效果</font>，且会报错👆**

```cpp
glGenerateMipmaps
```

`glGenerateMipmaps`：OpenGL提供函数来对图像生成mipmap

若使用Mipmap，则过滤方式将会改变，因为原先只在一张纹理上采样，现在有多张纹理可供采样，所以OpenGL提供如下，mipmap纹理过滤方式：

- `GL_NEAREST_MIPMAP_NEAREST`
- `GL_LINEAR_MIPMAP_NEAREST`
- `GL_NEAREST_MIPMAP_LINEAR`
- `GL_LINEAR_MIPMAP_LINEAR`

`GL_方式1_MIPMAP_方式2`：方式1为纹理采样方式，方式2为如何选择mipmap级别

mipmap提供多分辨率的纹理，主要作用于缩小的情况，因为对于缩小的情况还用原先大分辨率的贴图，好处：一是浪费GPU缓存，减少纹理采样、过滤、插值等计算成本；二是不够准确，因为可能轻轻移动采样到纹理上截然不同的纹素点

🤔️性能分析：mipmap具体流程：当我们生成mipmap时，会将多张不同分辨率的纹理一同加载进GPU显存中，加载是一次性的，虽然加载会比普通纹理加载慢，但是mipmap能在渲染过程中**显著提高性能**，尤其是对于**远处渲染**而言：低分辨率纹理会带来：较少的纹理采样计算，更少的插值操作（因为我们会选择临近过滤），减少**纹理缓存**开销（纹理采样时会从GPU显存转移到GPU缓存中）

#### （4）加载纹理 - <font color='green'>stb_image.h</font>

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308131606117.png" alt="截屏2023-08-13 16.06.06" style="zoom:50%;" />

```cpp
int width, height, nrChannels;
unsigned char *data = stbi_load("./Textures/wall.jpg", &width, &height, &nrChannels, 0);
...
stbi_image_free(data);
```

`stbi_load()`：会将图像的宽度、高度、颜色通道的个数；`stbi_image_free()`释放图像内存

🤔️`stbi_load()`函数返回值是`unsigned char`数组，因为：**unsigned char是无符号字符范围是0~255(1字节8位)，刚好能表示颜色值**，所以这里用的是`unsigned char`

#### （5）使用纹理

```cpp
unsigned int texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);
... // 过滤方式、环绕方式
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
glGenerateMipmap(GL_TEXTURE_2D);

...

```

用法与缓冲区对象类似，都是先`glGen...`函数创建，然后绑定`glBind...`到对应类型，然后传递数据`glTexImage2D`

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308131655129.png" alt="截屏2023-08-13 16.55.18" style="zoom:50%;" />

第二个参数也就是`level`，就是其所处在mipmap中的级别，我们可以依次设置当前纹理的不同级别图像，我们也可以只设置最基本级别`0`，然后通过`glGenerateMipmap()`函数为我们自动创建所需要的mipmap

如何在shader中使用纹理，并使用多张纹理：如前文所述，我们使用`glBindTexture(GL_TEXTURE_2D, texture)`来将纹理绑定到`GL_TEXTURE_2D`上，这么看似只能绑定一张纹理，实则不然：纹理还有一个概念，叫做**纹理单元（Texture Unit）**，比如：

```cpp
glActiveTexture(GL_TEXTURE0); // 在绑定纹理之前先激活纹理单元
glBindTexture(GL_TEXTURE_2D, texture);
glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); 
```

也就是说，我们实际是将纹理绑定到**当前激活的纹理单元上**，`GL_TEXTURE0`是默认激活的，所以只用一张纹理时不用`glActiveTexture`操作；同时，需要通过`glGetUniformLocation()`找到采样器位置，然后通过`glUniform1i()`设置int值，该int值就是对应的纹理单元

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308131722286.png" alt="截屏2023-08-13 17.21.57" style="zoom:50%;" />

总结一下：shader中使用纹理

- 采样器sampler：`sampler2D`...
  - 采样器会存储采样的相关配置（比如：环绕方式、过滤方式）
- 纹理单元：`GL_TEXTURE0`...
  - 纹理单元是指GPU上存储纹理数据的一块资源
  - 一个纹理单元上可以绑定一个采样器，这样该纹理单元就能被用于渲染管线中的采样纹理操作
- 纹理目标/类型：`GL_TEXTURE_2D`
  - 纹理目标用来指示纹理数据存储格式和用途

### 5.数学库 GLM

GLM：OpenGL Mathematics，只有头文件的库，不需要链接和编译，它的变量命名方式与OpenGL基本对齐，方便使用

```cpp
glm::vec4 v(1.0f, 0.0f, 0.0f, 1.0f);
glm::mat4 trans = glm::mat4(1.0f); // 单位矩阵
```

提供`scale`、`rotate`、`translate`三个函数用于生成SRT矩阵，这里函数的作用是在trans的基础上右乘一个变化矩阵（也就是说它和常规相反）

```cpp
// SRT
trans = glm::translate(trans, glm::vec3(1.0f, 0.0f, 0.0f));
trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
```

矩阵与向量乘法：向量一般视为列向量，所以需要左乘变化矩阵

```cpp
v = trans * v;
```

### 6.坐标系统

###### 🥱略～之前已经研究很透彻，就不在这里做笔记了 => 这里更关注：所使用的函数，以及相关规范，以及与其他api不同之处

#### （1）投影矩阵

- 正交投影：`glm::ortho()`
- 透射投影：`glm::perspective()`



## C1 调试

### 1.glGetError()

```cpp
GLenum glGetError();
```

`glGetError`只能返回最近发生的<u>一个</u>错误标志，而不能一次性返回所有可能的错误，并且会在调用后将该错误标志从OpenGL状态中清除 => 所以常规做法是：在可能发生错误的一系列OpenGL操作之间多次调用`glGetError`，以便获取多个错误

输出调试信息时，可以使用两个实用的宏`__FILE__`、`__LINE__`，分别指对应的文件与行号

### 2.调试输出

暂时可以忽略，因为只在OpenGL4.3以及更高的版本中才成为OpenGL的核心特性，且低版本部分操作系统可能不支持；调试输出能够更详细地检测OpenGL执行期间的问题和错误

### 3.调试着色器输出

着色器没办法打断点，想要调试着色器，最好的方法是将变量传递到像素着色器，然后通过颜色值输出到屏幕，通过观察颜色来推导是否出错

### 4.OpenGL GLSL参考编译器

**Glslang**：GLSL语言校验器（GLSL Lang Validator）：能够检测着色器是否符合GLSL规范（虽然不能完全保证没有Bug）

> 网址：https://www.khronos.org/opengles/sdk/tools/Reference-Compiler/
>
> Release资源包下载：https://github.com/KhronosGroup/glslang/releases

可以通过homebrew安装glslang：`brew install glslang`

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308121732141.png" alt="截屏2023-08-12 17.32.26" style="zoom: 50%;" />

```shell
glslangValidator xxx.vert
```

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308121755628.png" alt="截屏2023-08-12 17.55.21" style="zoom:50%;" />

没问题则不会返回任何信息，有报错则会输出详细信息如上👆

### 5.帧缓冲输出

就是将信息输出到帧缓冲中，思想就是把一些需要检验的信息通过像素颜色的形式输出到帧缓冲中，我们可以在屏幕右上角划分一片小区域输出这个帧缓冲，来进行相关调试

比如可以将法线信息、纹理信息、阴影深度图等信息输出出来，看是否出现问题，以便程序员调试发现错误

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308121802320.png" alt="debugging_fbo_output" style="zoom: 100%;" />

### 6.外部调试软件

###### gDebugger、RenderDoc、CodeXL、NVIDIA Nsight

外部调试软件：通常将它们自己注入到OpenGL驱动中，以拦截各种OpenGL调用，来提供大量的数据

注意：不同调试软件受平台以及显卡的限制，只能在特定平台和显卡上发挥其功效

**Mac平台调试图形应用程序**：【ChatGPT推荐】

- <font color='purple'>**Render Doc**</font>
- Xcode 和 Instruments
  - Instruments作为Xcode内置的性能分析工具，能够分析CPU、内存、图形性能等
  - 对于图形调试，你可以使用 Instruments 中的 "OpenGL ES Analyzer"（针对 OpenGL ES）或者 "Metal System Trace"（针对 Metal）工具来分析图形性能和资源使用情况
- OpenGL Profiler
  - macOS内置的图形分析工具，用于OpenGL应用程序的性能分析
  - 也是内置在Xcode-Instruments中
