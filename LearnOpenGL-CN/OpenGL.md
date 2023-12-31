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

💡如果想要用vector作为顶点数据的存储容器，依旧可以，只需对`glBufferData`稍作改动：

```cpp
glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
```

⚠️我们可以绑定多种数据到顶点缓冲上，而且在shader中可以选择使用（比如：我绑定了顶点位置、纹理、颜色，而shader中layout只写了顶点位置的输入）

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

#### （6）GLSL使用结构体struct

如果想要自定义一个struct类型，并作为uniform对象传入：

```cpp
// shader
struct Light{
    vec3 position;
    vec3 color;
    float intensity;
};
uniform Light light;

// main.cpp
sc.setVec3("light.position", glm::value_ptr(light.position));
sc.setVec3("light.color", glm::value_ptr(light.color));
sc.setFloat("light.intensity", light.intensity);
```

也就是通过类似`light.color`的成员，来查location值，而且这些成员的location是挨着的，相差1，但是通过查找`light`就会失败，返回-1

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

相关函数：

- 正交投影：`glm::ortho()`
- 透射投影：`glm::perspective()`

### 7.深度缓冲（Z-buffer）

```cpp
glEnable(GL_DEPTH_TEST);

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

深度测试默认是关闭的，通过`glEnable()`开启；每次渲染迭代中需要清除深度缓冲，和颜色缓冲一样，不然就会保存上一帧的数据

### 8.camera

#### （1）Look At

```cpp
glm::vec3 cameraPos = glm::vec3(0.f, 0.f, 3.f);
glm::vec3 cameraTarget = glm::vec3(0.f, 0.f, 0.f);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
glm::vec3 cameraRight = glm::cross(up, cameraDirection);
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
```

摄像机坐标系中观察方向（-z轴）是确定的，竖直向上方向大致确定（在yoz平面内），因此根据基向量的正交性，能够得到x方向的方向垂直于yoz平面内，可以由叉乘得到

```cpp
glm::lookAt(glm::vec3 eye, glm::vec3 target, glm::vec3 up);
```

`glm::LookAt()`函数能简化上述步骤：只需要提供摄像机位置、观察位置和向上坐标

#### （2）欧拉角

欧拉角：俯仰角（Pitch）、偏航角（Yaw）、滚转角（Roll）

Pitch（y轴）一般不能接近90度，原因：欧拉角万向锁死问题

欧拉角直观的表达了视角移动的角度，我们通过记录欧拉角，最后手动转换为view坐标系和view矩阵

### 9.光照

#### （0）光照模型

常见的光照模型：

- Phong
- BlinnPhong

BlinnPhong相比于Phong模型来说，共同点是：都是ambient、diffuse和specular；区别是：blinnphong引入半程向量计算高光项、引入菲涅尔反射

#### （1）Phong光照模型

- ambient

  - ```
    vec3 ambient = lightCol * ambientStren * albedo;
    ```

- diffuse 

- specular

#### （2）Blinn Phong光照模型

公式：
$$
LitColor=c_a+c_d+c_s
\\ = A_L\otimes m_d+max(L\cdot n,0)\cdot B_L\otimes(m_d + R_F(\alpha_h)\frac{m+8}{8}(n\cdot h)^m)
$$
m<sub>d</sub>：**漫反射反射率**；首先漫反射是指来自光源的光进入物体内部，经过多次反射、折射、散射以及吸收后返回样品表面的光；漫反射反射率就是指：反射出样品表面的光的比例 => ambient和diffuse都采用diffuse albedo这个参数，是因为他们其实都是表示“漫反射”的结果，只是diffuse是由直接光照产生的，而ambient是由间接光照产生的【这里说的不准确，只是便于理解，因为BlinnPhong模型本身就是一种抽象假设，并不具有完全正确的物理意义】

max(L⋅n, 0)：**朗伯余弦定理**，是指光线达到平面时，平面接受到的能量和夹角的关系 => 这里ambient不用是因为环境光来自四面八方，而diffuse和specular来自直接光源，且是单向光源

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308182010426.png" alt="截屏2023-08-18 20.09.59" style="zoom: 50%;" />：**菲涅尔效应**&**半程向量**

- 菲涅尔效应：光线从一个介质进入另一个介质时，在界面处会发生反射和折射的现象，R<sub>F</sub>：反射光量，1-R<sub>F</sub>：折射光量，一般不会将完整的菲涅尔方程用于实时渲染，而是采用**石里克近似法**：

$$
R_F(\theta_i)=R_F(0^{\circ} )+(1-R_F(0^{\circ} ))(1-cos\theta_i)^5
$$

- R<sub>F</sub>(0<sup>o</sup>)是指入射角为0度，也就是观察方向垂直于平面时，所以对于一般介质来说，比如说水、玻璃、塑料来说，这个值相当小，水的值是0.02，但对于金属介质来说，因为基本上光会全反射回来，所以值很大，接近1

- 半程向量：引入半程向量之前，要理解**微表面模型**，一个微观平面往往由多个既微小又光滑的微平面所构成（如图👇）

  - 光向量L，观察向量v，平面(宏观)法线向量n，半程向量就是v和L的中间位置，有多少比例的微平面的法线是h，那么该平面就能反射多少比例的光亮，

  - α<sub>h</sub>：半程向量与光向量的夹角，所以将α<sub>h</sub>（而不是光向量与宏观法线向量）代入石里克近似方程，是因为我们将反射平面考虑为这些法线朝向为h的微平面

  - 那么还需要求取有多少比例的微平面，它的法线朝向是指向h的，有一个分布函数cosθ<sup>m</sup>就符合这个要求，再对其进行归一化处理，以保持光能守恒

  - $$
    max(L\cdot n, 0)\cdot B_L\otimes(R_F(\alpha_h)\frac{m+8}{8}(n\cdot h)^m)
    $$

    

  - <img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308191046800.png" alt="截屏2023-08-19 10.46.41" style="zoom:50%;" />

  - <img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308191057455.png" alt="截屏2023-08-19 10.57.16" style="zoom:50%;" />

  - 如上图👆：m这里代表光泽度，也就是`(1.0 - roughness) * 255.0 + 1.0`，roughness是0～1的浮点小数，一般会乘上256.f，得到这里的m


BlinnPhong需要注意的点：

- 朗伯余弦定理：朗伯余弦定理需要除以归一化常数`pi`，使光亮守恒，因为反射的光应该均匀的分到所有可能的方向，保证：发射的能量 = 吸收的能量 + 反射的能量
  - 如果不除，整体光亮会过量
- m：这里m的计算是`m = (1.0 - roughness * 255.0 + 1.0)`，是保证了m的范围[1, ∞]，m严格来说跟光泽度有关系，m叫做“粗糙度指数”，m决定高光的分布范围，m越大，高光越亮越集中

#### （2）特殊：法向量的转换

<font color='red'>**法向量之所以特殊，是因为它是从属于顶点位置的**</font>，也就是说，先有顶点位置，才有法向量；特殊地，当我们对顶点数据进行了**不等比缩放**时，对法向量进行同等变换，会让法向量不再垂直于顶点平面

我们输入的是局部空间的normal，如果想要计算出世界空间的normal，需要为该normal施加特别的model矩阵

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308181301498.png" alt="basic_lighting_normal_transformation" style="zoom: 100%;" />

所以说我们需要对法向量定制一个特殊的模型矩阵(model) - 法线矩阵：
$$
M_{normal}=(M^{-1})^T
$$
ps：矩阵求逆在shader中是很大的开销，最好在cpu上算出法线矩阵，并传进来

🤔️**推理**：切线和顶点是对应关系，也就是说，对顶点的变换也就是对切线的变换，公式如下：
$$
t = p_1-p_2
\\ T = M p_1-M p_2 =M t
$$
又因为切线t和法线n是始终垂直的，所以：
$$
n\cdot t=0
\\ N\cdot T=0
\\ \therefore (M_{n} n)\cdot(M t)=0
\\ (M_nn)^T(Mt)=0
\\ n^T(M_n^TM)t=0
$$
那这里就有一个trick了，因为这个公式并不能推出M<sub>n</sub><sup>T</sup>M为单位矩阵；但是，这个式子为单位矩阵时，确实让等式成立了，所以说这个做法是可取的，我们构造了一个法线矩阵：
$$
M_n = (M^{-1})^T
$$

#### （3）平行光、电光源、手电筒

##### 平行光 - directional light

- 光的描述：direction，不是position，光线不衰减

##### 点光源 - point light

- 光的描述：position，光线要衰减

- 衰减方程：

  - 线性：falloffStart ～ falloffEnd

  - 非线性：
    $$
    F_{att}=\frac{1.0}{K_c+K_l*d+K_q*d^2}
    $$
    K<sub>c</sub>一般保持1，让分母始终大于1，另外两个分别是一次项和二次项，实现非线性的光线衰减

    命名：constant、linear、quadratic

    <img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308220950503.png" alt="截屏2023-08-22 09.50.11" style="zoom: 50%;" />

  - 

##### 聚光灯 - spot light

距离聚光灯中心方向越近，光亮衰减越少；聚光灯边缘应该缓慢过渡到0，设计一个符合要求的衰减函数

### 10.模型导入

#### （1）Assimp库

Assimp：Open Asset Import Library - 开放的资产到入库；Assimp能够导入很多种不同的模型文件格式

> assimp3.31 mac编译(避坑) - 参考文章：https://cloud.tencent.com/developer/article/1649512
>

TODO：编译

## S3 高级OpenGL渲染

### 0.渲染管线



### 1.深度测试

```cpp
glEnable(GL_DEPTH_TEST);

// mainLoop
glClear(GL_DEPTH_BUFFER_BIT);
```

深度缓冲是由窗口系统所创建的，有16、24或32位**float**的形式存储深度值，**在大部分操作系统中，深度缓冲的精度都是24位**（说白了由float的位数决定，一般操作系统sizeof(float)=4字节=32位）

进行深度测试时，只读取深度值，但不写入深度值 - 关闭**深度掩码**：

```cpp
glDepthMask(GL_FALSE);
```

🤔️开启掩码：意味着数据能被正常写入；关闭掩码：意味着数据不允许写入；我的理解是：程序通过掩码的形式控制哪部分数据能写入，哪部分数据不能写入，故开启掩码意味着打开了数据写入的通道；同时可以从模版缓冲掩码验证我所说的

深度测试函数：

```cpp
glDepthFunc(GL_LESS); // 默认
```

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308231623618.png" alt="截屏2023-08-23 16.23.37" style="zoom:50%;" />

### 2.模版测试

```cpp
glEnable(GL_STENCIL_TEST);
glClear(GL_STENCIL_BUFFER_BIT);
glStencilMask(0xFF); // 默认:AND操作,默认所有掩码位都是1 - 1代表允许写入
glStencilFunc(GL_EQUAL, 1, 0xFF); // 模版测试函数
glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); 
```

模版缓冲中，通常每个模版值是**8位**的（二进制的8位能创造出256种组合，8位就是char的大小），同样由窗口系统所管理

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308231644824.png" alt="截屏2023-08-23 16.44.10" style="zoom:50%;" />

```cpp
glStencilFunc(GLenum func, GLint ref, GLuint mask)
```

ref是参考值，也就是说只有满足了模版比较函数`func`，说明该像素通过了模版测试，则不丢弃

```cpp
glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)
```

sfail：模版测试失败采取的行为；dpfail：模版测试通过，但深度测试失败时采取的行为；dppass：模版测试和深度测试都通过采取的行为

![截屏2023-08-23 16.53.19](https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308231653387.png)

总结一下模版缓冲的流程：

- 开启模版缓冲
- 开启模版缓冲的写入
- 渲染物体时，更新模版缓冲的内容
- 禁用模版缓冲的写入
- 再渲染其他物体，根据模版缓冲的内容丢弃特定的片段	

🤔️模版缓冲区能配置的和深度缓冲区一样吗？首先不可磨灭的不同之处就是，模版缓冲区存储的数据只有8位，而深度缓冲区能存储24位；深度缓冲区比较的是深度值信息，通过gl_Position.z获取，但模版缓冲区不能自定义模版值输入进去，模版值的更新只能通过glClear统一变为0，而且OpenGL模版缓冲提供的比较函数，不能实现保留最小模版值的操作

⚠️注意：容易混淆：`glStencilFunc`和`glStencilMask`都提供缓冲掩码的参数，它们作用并不同；`glStencilFunc`：掩码是对参考值ref进行处理，它会与ref进行AND操作，以对参考值进行修改；而`glStencilMask`：掩码是指模版缓冲区哪些位允许被写入

### 3.混合（Blend）

#### （1）alpha通道 

一些图片会带有alpha通道值，用于告诉我们哪些部分是透明的

#### （2）discard

如果不开启混合，alpha值可能没被运用起来，我们可以通过判断alpha是否几乎为0，以此来判断是否丢弃该像素 => 像素着色器提供内建函数`discard`

```glsl
if(texColor.a < 0.1)
	discard;
```

这种处理方式是很trick的，但是注意，纹理的环绕方式可能会影响该操作：也就是说，纹理边缘会进行插值，可能会导致alpha通道也进行插值，而导致错误显示

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308231807077.png" alt="截屏2023-08-23 18.07.12" style="zoom:50%;" />

#### （3）混合

```cpp
glEnable(GL_BLEND);
// 很多种方式控制blend方程
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
glBlendEquation(GL_FUNC_ADD); // 一般也不会修改这个"+"
```

混合运算采用👇方程的形式：
$$
C_{result}=C_{source}*F_{source}+C_{destination}*F_{destination}
$$
源颜色和目标颜色都各自有一个系数，这个系数是我们可以设置的，来控制混合方程

**源颜色**：当前像素着色器输出的颜色，或者叫当前渲染像素的颜色；**目标颜色**：颜色缓冲区里存储的颜色

```cpp
glBlendFunc(GLenum sfactor, GLenum dfactor)
glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
```

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308231812234.png" alt="截屏2023-08-23 18.12.18" style="zoom:50%;" />

Blend方程中的运算符：

```cpp
glBlendEquation(GLenum mode)
```

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308232014253.png" alt="截屏2023-08-23 20.14.09" style="zoom:50%;" />

### 4.特殊case：渲染镜子

特殊case：渲染镜子

- 先渲染镜子外的所有物体（不包括镜子）
  - 开启深度缓冲`glEnable(GL_DEPTH_TEST)`
  - 渲染
- 再渲染镜面的模版缓冲区
  - 清理模版缓冲区
  - 开启模版缓冲`glEnable(GL_STENCIL_TEST)`，和深度缓冲
  - 开启模版缓冲的写入`glStencilMask(0xFF);`，且设置模版函数为`glStencilFunc(GL_ALWAYS, 1, 0xFF)`，设置模版操作为`glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE)`
  - 但要关闭深度缓冲的写入
  - 渲染镜子，以更新模版缓冲 => 此时镜面可见部分为1，其余部分为0
- 然后渲染镜子之内（反射的）物体（虚像）
  - 关闭模版缓冲	

- 最后渲染镜子
  - 采用混合技术

### 5.面剔除

#### （1）环绕顺序

<font color='red'>**默认情况下：逆时针绕序会被视为正向的三角形**</font>

#### （2）面剔除

```cpp
glEnable(GL_CULL_FACE);
glCullFace(GL_BACK); // 剔除背面(也是默认)
```

有三种选项：`GL_BACK`、`GL_FRONT`、`GL_FRONT_AND_BACK` => 想要正背面都不剔除，就不要开启面剔除

修改默认绕序：

```cpp
glFrontFace(GL_CW);; // clockwise -> 修改成顺时针
```

`GL_CW`：顺时针 - clockwise；`GL_CCW`：逆时针（默认）- counter-clockwise

### 🌟6.帧缓冲 - 渲染到纹理

#### （1）帧缓冲

```cpp
unsigned int fbo;
glGenFrameBuffers(1, &fbo);
glBindFrameBuffer(GL_FRAMEBUFFER, fbo);

glDeleteFrameBuffers(1, &fbo);
```

帧缓冲（Frame Buffer）：之前做的所有操作都作用于**默认帧缓冲**上，默认帧缓冲是我们在创建窗口时自动生成的（GLFW帮我们完成）

创建和绑定已经很熟悉了，当我们绑定到`GL_FRAMEBUFFER`目标之后，所有的读取和写入帧缓冲的操作都会影响当前绑定的帧缓冲；当然，读取和写入目标可以分别绑定：`GL_READ_FRAMEBUFFER`、`GL_DRAW_FRAMEBUFFER`

```cpp
glBindFrameBuffer(GL_FRAMEBUFFER, 0); // 重新激活默认帧缓冲
```

“解绑”帧缓冲操作，就相当于重新激活默认帧缓冲，也就是主屏幕窗口的帧缓冲

⚠️注意：要使用帧缓冲，必须保证它是“完整”的，也就是说要满足以下条件：（1）附加至少一个缓冲(颜色、深度或模版)（2）至少有一个<font color='red'>**颜色附件(Attachment)**</font>（3）所有的附件必须是完整的(保留了内存)（4）每个缓冲都应该有相同的样本数

```c++
if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
  // 帧缓冲符合要求
```

#### （2）附件

附件：是一个内存位置，可以作为帧缓冲的一个缓冲，可以将它想象为一张图像

附件有两种类型：**纹理附件** & **渲染缓冲对象附件**

##### 纹理附件

我们可以将创建的纹理对象(texture object)作为附件绑定到帧缓冲上，他的好处是：因为它是一张纹理，所以他可以很方便的传递到shader中（也就是具有texture object的一切特性）

```cpp
unsigned int texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); // 给data参数传NULL
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

作为纹理附件的纹理不关心环绕方式和mipmap，因为我们大多情况下不会用到他们

注意：纹理的宽和高一般设置的和屏幕宽高一样，但如果想要将屏幕渲染到一个更大或更小的纹理上，注意需要在渲染前**重新设置glViewport**，将纹理的新维度作为参数，否则只有一小部分会被渲染到纹理上

将纹理附加到帧缓冲上：

```cpp
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
```

最后一个参数是mipmap级别，我们保留为0

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308251633152.png" alt="截屏2023-08-25 16.33.36" style="zoom:50%;" />

##### (?)渲染缓冲对象附件

渲染缓冲对象是后来引入OpenGL的，过去纹理是唯一的可用附件；渲染缓冲对象是一个真正的缓冲

（?）**使用渲染缓冲对象附件的好处：它会将数据存储为OpenGL原生的渲染格式，它是为离屏渲染到帧缓冲优化过的**；它直接将渲染数据存储到缓冲中，不会进行任何格式转换，变成一个“写”的更快的可存储介质；一般来说，渲染缓冲对象是**只写的**，一般不能读取它们（比如纹理访问）；但仍然可以通过`glReadPixels`来读取，但这种读取方式是从当前绑定的帧缓冲中返回像素，而不是附件本身

**好处总结：快速写入、快速交换缓冲、数据原生（格式不会转换）**

```cpp
unsigned int rbo;
glGenRenderbuffers(1, &rbo);
glBindRenderbuffers(GL_RENDERBUFFER, rbo);
```

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308251641933.png" alt="截屏2023-08-25 16.41.07" style="zoom:50%;" />

创建深度和模版缓冲对象：

```cpp
glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
```

##### 如何选择？

- 纹理附件：当需要采样（texture函数）时，则使用纹理附件
- 渲染缓冲对象附件：当不需要采样（texture函数）时，则使用渲染缓冲对象附件

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308251647838.png" alt="截屏2023-08-25 16.46.19" style="zoom:50%;" />

说白了：默认的一定足够通用，但性能一定不是最好，OpenGL开放这种定制途径，可以让我们更好的自定义我们需要的缓冲、对象等，可以提高性能

#### （3）渲染到纹理

我们需要创建一个shader处理**全屏四边形**（**quad**），这里要注意⚠️：OpenGL的NDC空间的定义中，x和y的范围是-1～1，而不是0～1

##### ⚠️⚠️⚠️注意：如果新创建了一个帧缓冲，因为帧缓冲中包含颜色附件和深度附件等，所以如果我们只附加了颜色附加，则该帧缓冲的深度测试失效！

![截屏2023-08-26 21.40.27](https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308262143430.png)

🤔️问题：当我在运行程序时，发现屏幕发现扭曲，实测发现：当增大一倍纹理分辨率后，该现象消失 => 因为mac是视网膜显示器，所以实际渲染大小需要是窗口大小的两倍

### 7.Cube Map

#### （1）cubemap使用

cubemap立方体贴图有六张纹理，通过**方向向量**来计算采样点，使用方法和普通纹理类似：

使用类型`GL_TEXTURE_CUBE_MAP`，我们需要通过`glTexImage2D`函数6次，依次对6个面设置纹理，纹理目标有6个，在shader中使用`samplerCube`：

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202309090833821.png" alt="截屏2023-09-09 08.33.25" style="zoom:50%;" />

```cpp
int width, height, nrChannels;
unsigned char *data;  
for(unsigned int i = 0; i < textures_faces.size(); i++)
{
    data = stbi_load(textures_faces[i].c_str(), &width, &height, &nrChannels, 0);
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
        0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
    );
}
```

设置环绕和过滤方式：

```cpp
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // 这里设置了R,相当于z坐标
// 此时的GL_CLAMP_TO_EDGE就是将坐标范围控制在-1~1之间,而不是0~1之间
```

渲染天空盒的要点：

- 天空盒box是一个长宽为2，中心在世界空间原点的立方体，这样每个方向就能直接得到对应的方向向量，并采样

- 天空盒应该能旋转，所以我们还是需要保留**view矩阵**，因为我们渲染天空盒实际上是渲染六个面，所以**projection矩阵**也是必须的

  - view矩阵代表摄像机空间，我们只需要摄像机空间在原点

  - ```cpp
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // 只保留旋转部分
    ```

天空盒的渲染时机：

- 可以最先渲染天空盒，再渲染其他物体，那么像素会被覆盖

  - 我们给的天空盒mesh的z值并不是巨大无比，但是仍能奏效的原因是因为：如果先渲染天空盒，我们都**没必要进行深度测试**和深度写入。这样在渲染其他物体时，直接覆盖即可

- 可以先渲染物体，再渲染天空盒，这样利用提前深度测试，减轻gpu负担 - 那么在渲染天空盒时，深度测试函数需要变更为`GL_LEQUAL`；

  - 所以如果要后渲染天空盒，就要注意深度的问题，天空盒的深度值在ndc空间中都应该为1

  - 我们在shader中可以通过以下trick实现：

  - ```cpp
    void main()
    {
        TexCoords = aPos;
        vec4 pos = projection * view * vec4(aPos, 1.0);
        gl_Position = pos.xyww;
      	// 因为shader结束后会进行透视触发,也就是各位置/w
        // 所以我们令z=w,那么透视除法后z=1
    }
    ```

#### （2）环境映射 - environment mapping

环境映射主要包含两方面：反射、折射

- 反射：这里的反射是指完全反射，通过观察向量与法向量，计算出反射向量，并用反射向量去采样cubemap

  - 反射函数：`reflect(被反射的向量, 法线)`

  - ```glsl
    vec3 view = normalize(positionW - cameraPosW);
    vec3 R = reflect(view, normalize(normalW));
    FragColor = texture(cubemap, R);
    ```

- 折射：光线穿过物体到达眼睛，这里我们只考虑一次折射（严格意义来说，进出物体都要发生折射），从视觉效果上来看够了

  - 折射函数：`refract(被折射的向量, 法线, 折射系数)`

  - ```glsl
    float ratio = 1.00 / 1.52;
    vec3 view = normalize(positionW - cameraPosW);
    vec3 R = refract(view, normalize(normalW), ratio);
    FragColor = texture(cubemap, R);
    ```

反射贴图：指定该位置反射的比例，并不是百分之百反射

#### （3）动态环境映射

之间我们使用的是静态的cubemap纹理，我们实际上可以将场景渲染到cubemap的六个面上，我们就用了动态物体的信息，但是比较费

### 8.高级数据

#### （1）缓冲区

```cpp
void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
// target: GL_TEXTURE_2D
// size: 占总字节数
// data: 数据数组指针
// usage: GL_STATIC_DRAW ...
```

如果我们将`glBufferData`的`data`参数设置为`NULL`，则只会分配空间而不会填充数据。但`glBufferData`只能一次性填充整个缓冲区的数据，我们可以通过`glBufferSubData`来填充缓冲区的特定区域

```cpp
void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
// 可以从偏移量offset处开始填充size字节的数据
```

另外，我们可以通过获取缓冲区指针的方式，直接通过缓冲区指针来给缓冲区注入数据，而不用通过`glBufferData`

```cpp
glBindBuffer(GL_ARRAY_BUFFER, buffer);
void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY); // 获取缓冲区指针
memcpy(ptr, data, sizeof(data));
glUnMapBuffer(GL_ARRAY_BUFFER); // 告诉OpenGL不在使用指针
```

#### （2）分批顶点属性

默认的顶点数据摆放方式：连续并排穿插循环摆放，也就是说，如果有三个顶点属性(1\2\3)，在数据中摆放方式：123123123 => 我们可以通过**分批**（batched）的方式改变成111222333（batch的意思是打包，我们将每种数据打包在一起，再合成一个大的区块）

🤔️平常我们从模型文件中获取顶点属性时，我们常常得到的是几个数组，比如位置数组、法线数组、纹理坐标数组，这些数组本身是分开的，我们之前为了好写入VBO而专门把它们转换为了一个大的交错数据数组，这很费力气。

我们通过`glBufferSubData()`实现数据的分批：

```cpp
float positions[] = { ... };
float normals[] = { ... };
float tex[] = { ... };
// 填充缓冲
glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), &positions);
glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(normals), &normals);
glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions) + sizeof(normals), sizeof(tex), &tex);

// 更新VAO
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);  
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(sizeof(positions)));  
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)(sizeof(positions)+sizeof(normals)));
// 修改步长和偏移量:
// 倒数第2个参数: 布长,因为现在同属性的数据紧密相连,所以步长就为该属性的大小
// 倒数第1个参数: 偏移量,数据从哪处为起点
```

分批的好处：不用单独整理一个交叉相错的顶点属性数组，数据看起来更整洁，其他无明显优点

#### （3）复制缓冲区数据

可能有时候希望将缓冲区数据复制到另一个缓冲区中，可以通过`glCopyBufferSubData`实现：

```cpp
void glCopyBufferSubData(GLenum readtarget, GLenum writetarget, GLintptr readoffset,
                         GLintptr writeoffset, GLsizeiptr size);
```

但是，会有一种情况：我们知道OpenGL一个缓冲类型只能绑定一个缓冲区在上面，所以如果我们希望将VBO的数据复制到另一个VBO上，就需要借助另外两个缓冲目标：`GL_COPY_READ_BUFFER`、`GL_COPY_WRITE_BUFFER`

也就是说，我们可以把VBO绑定在这两个可选的缓冲对象上，实现复制：

```cpp
glBindBuffer(GL_COPY_READ_BUFFER, vbo1);
glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2); // 当然也可以使用GL_ARRAY_BUFFER
glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(vertexData));
```

### 9.高级GLSL

#### （1）GLSL内建变量

常见内建变量：

- `gl_Position`：vs的输出向量（裁剪空间的位置）
- `gl_FragCoord`：

其他内建变量：

- 顶点着色器

  - `gl_PointSize`：（输出变量，类型：float）当我们以`GL_POINTS`的图元来渲染时，我们可以在vertex shader中输出当前渲染的点的大小 => 注意：默认是不能在vertex shader中修改点的大小，需要启用状态：`glEnable(GL_PROGRAM_POINT_SIZE)` => 在粒子生成之类的技术中有用

  - `gl_VertexID`：（vs输入变量，类型：int）当前渲染顶点的索引（glDrawElements则为对应当前索引，glDrawArrays则为对应顶点数量）=> 可以用来索引数组，比如我们有一个数组存储顶点的属性，我们通过gl_VertexID来找到对应下标

- 像素/片段着色器

  - `gl_FragCoord`：（输入变量，类型：vec4）x/y分量表示在屏幕上的坐标(以左下角为顶点) ，z分量为当前片元的深度值，w通常为1=> **与shadertoy相通**

  - `gl_FrontFacing`：（输入变量，类型：bool）告诉我们当前像素/片段属于正向面还是背向面，true：正向面 => 可以通过这个输入为三角形正背面独立渲染，比如正面使用A纹理，背面使用B纹理 => 前提：没开启背面剔除，开启再用这个属性就没啥意义了

  - 🌟<font color='purple'>**`gl_FragDepth`**</font>：（输出变量，类型：float）<font color='green'>**`gl_FragCoord`中我们可以读取像素的深度值，但它是只可读不可写，我们可以通过`gl_fragDepth`来在片段着色器中修改z值**</font> => <font color='red'>**>= OpenGL 4.2 版本**</font>

    - => 但我们一旦通过`gl_FragDepth`写入深度值，OpenGL就会自动**禁止所有的提前深度测试**

    - 我们可以在片段着色器的顶部使用**深度条件**重新声明gl_FragDepth变量

      ```glsl
      layout(depth_<condition>) out float gl_FragDepth;
      // depth_any depth_greater depth_less depth_unchanged
      ```

      <img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202309101811449.png" alt="截屏2023-09-10 18.10.50" style="zoom:50%;" />

      可以通过设置深度条件，告诉OpenGL我们会对深度值进行修改的方向，比如greater和less能够辅助OpenGL进行部分的提前深度测试
  
  - gl_FragData：TODO

#### （2）接口块

现在顶点着色器和像素着色器之间传递数据，都是通过定义一个一个对应变量实现，但这么写起来麻烦。OpenGL为了帮助我们管理这些变量，提供了**接口块**（Interface Block），类似于struct

```cpp
// vertex shader
...
out VS_OUT{
	vec2 TexCoords; // 注意:块内就不用写in/out了
} vs_out;
...
void main()
{
	vs_out.TexCoords = ...; // ⚠️使用时,要用块名作为前缀
}
  
// fragment shader
...
in VS_OUT{
	vec2 TexCoords;
} fs_in;
```

上述代码中，`VS_OUT`是<font color='red'>**块名**</font>，**必须一致**；`vs_out`和`fs_in`是**实例名**，可以随意 => <font color='purple'>**只要两个接口块的名字（块名）一样，输入和输出就能匹配起来**</font>

#### （3）Uniform缓冲对象

当有多个着色器时，我们会发现大部分uniform变量都是相同的，但我们却要反复设置 => 为解决这个问题，OpenGL提供**Uniform缓冲对象**（Uniform Buffer Object）

Uniform缓冲对象是一个缓冲对象，所以通过`glGenBuffers`创建，缓冲类型是：`GL_UNIFORM_BUFFER`

比如，我们想要将projection和view存入Uniform块中：<font color='red'>**⚠️注意:uniform块中的变量可以直接访问,不需要将块名作为前缀**</font>

```glsl
// vertex shader
...
layout(std140) uniform Matrics
{
	mat4 projection;
	mat4 view;
};
  
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0); // ⚠️注意:uniform块中的变量可以直接访问,不需要将块名作为前缀
}
```

#### （4）Uniform块布局

Uniform块中的变量并不一定都是紧密相邻排布的，这会导致我们使用`glBufferData`或`glBufferSubData`出现错误，所以我们需要了解Uniform块布局以及布局规则

有几种常见的内存布局方式：

- 共享布局（shared布局）：<font color='purple'>【**默认的**】</font>共享是指由硬件决定偏移量，并根据硬件定义的偏移量堆GLSL进行调整，以保证多个程序中保持一致 => 需要通过`glGetUniformIndices`来查询偏移信息，比较麻烦 【TODO：以后再来研究】

- **`std140`布局**：std140布局规则显示规定了每个变量的偏移量，显示地定义了规则意味着我们可以根据这些规则明确知道内存布局方式 => **显示的规定虽然不一定是最高效的，但一定能保证在各个程序中是一致的**

  - 布局规则：【内存对齐】

    - 每个变量有一个**基准对齐量**，表示该变量在Uniform块中所占据的空间（包括填充量（Padding）），比如int、bool等标量类型，都是4字节，而向量vecn可能是2\*4字节，可能是4\*4字节（特别的：vec3就是4*4字节，也就是要填充4字节的空白），而对于矩阵类型，相当于存储为多个vec4
    - 再计算它的**对齐偏移量**，一个变量的对齐偏移量必须等于**基准对齐量的倍数** => 也就是如果基准对齐量为4*4字节，那么该变量的起始地址必须存放在4\*4\*n的地址上，这是为了**方便寻址**

    <img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202309101851642.png" alt="截屏2023-09-10 18.51.39" style="zoom:50%;" />

  - 对齐偏移量例子：

    ```cpp
    layout (std140) uniform ExampleBlock
    {
                         // 基准对齐量       // 对齐偏移量
        float value;     // 4               // 0 
        vec3 vector;     // 16              // 16  (必须是16的倍数，所以 4->16)
        mat4 matrix;     // 16              // 32  (列 0)
                         // 16              // 48  (列 1)
                         // 16              // 64  (列 2)
                         // 16              // 80  (列 3)
        float values[3]; // 16              // 96  (values[0])
                         // 16              // 112 (values[1])
                         // 16              // 128 (values[2])
        bool boolean;    // 4               // 144
        int integer;     // 4               // 148
    }; 
    ```

- `std430`布局

- 紧凑布局（packed布局）

TODO：研究内存布局

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202309102152074.png" alt="截屏2023-09-10 21.51.32 1" style="zoom:40%;" />

#### （5）使用Uniform缓冲

```cpp
// 跟其他缓冲一样,
unsigned int uboExampleBlock;
glGenBuffers(1, &uboExampleBlock);
glBindBuffer(GL_UNIFORM_BUFFER, uboExampleBlock);
glBufferData(GL_UNIFORM_BUFFER, 152, NULL, GL_STATIC_DRAW); // 分配152字节的内存
glBindBuffer(GL_UNIFORM_BUFFER, 0);
```

如何将Uniform缓冲和shader中对应Uniform块对应起来，涉及两个概念：

- Block indices（块索引）：在每个着色器程序中，每个Uniform块对应一个块索引
- Binding points（绑定点）：绑定点类似于`GL_TEXTURE0`一样，也是预先挖好的坑，我们会将Uniform缓冲数据绑定在绑定点上；绑定点有0、1、2 ...

使用时，我们需要将块索引和绑定点链接起来，即可正确使用Uniform缓冲

```
unsigned int lights_index = glGetUniformBlockIndex(shaderA.ID, "Lights");
glUniformBlockBinding(shaderA.ID, lights_index, 2);
```

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202309111225467.png" alt="advanced_glsl_binding_points" style="zoom:100%;" />

PS：在OpenGL4.2版本起，可以在shader中显示指出Uniform缓冲块对应哪个绑定点，不再需要在外部调用`glUniformBlockBinding`

```glsl
layout(std140, binding = 2) uniform Lights { ... };
```

不仅shader中Uniform块要绑定在Binding points上，Uniform块缓冲区也要绑定到对应Binding Points上：

```cpp
glBindBufferBase(GL_UNIFORM_BUFFER, 2, uboExampleBlock); // 2:绑定点2 uboExampleBlock:缓冲区
// 或者
glBindBufferRange(GL_UNIFORM_BUFFER, 2, uboExampleBlock, 0, 152); // 0:附加偏移量 152:大小参数，可以将缓冲区的一部分绑定在点上 => 这样可以将多个缓冲区的多个部分合并在一起绑定在绑定区上
```

最后向缓冲对象输入数据：

```cpp
glBindBuffer(GL_UNIFORM_BUFFER, uboExampleBlock);
int b = true; // GLSL中的bool是4字节的，所以我们将它存为一个integer
glBufferSubData(GL_UNIFORM_BUFFER, 144, 4, &b); 
glBindBuffer(GL_UNIFORM_BUFFER, 0);
```

总结一下：创建Uniform缓冲区 => 绑定到Binding Point上`glBindBufferBase/glBindBufferRange` => 为缓冲区注入数据 => 将Shader中Uniform块绑定到Binding Point上(通过Block index来过渡) 「注意：不再需要通过glUniform...函数传递数据」

### 10.几何着色器

#### （1）一个简单的Geometry Shader：

```cpp
#version 330 core
layout (points) in; // 输入的图元类型
layout (line_strip, max_vertices = 2) out;

void main()
{
	gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0); 
  EmitVertex();

  gl_Position = gl_in[0].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0);
  EmitVertex();

  EndPrimitive();
}

/*
该几何着色器的作用：输入一个点，输出一条直线
*/
```

包括：

- `layout (points) in;` - 输入的图元类型

  - <img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202309122102692.png" alt="截屏2023-09-12 21.02.08" style="zoom:40%;" />
  - 图中：括号内的数字表示：一个图元所包含的最小顶点数

- `layout (line_strip, max_vertices = 2) out;` - 输出的图元类型、最大能够输出的顶点数量

  - 输出的图元类型包括`points`、`line_strip`、`triangle_strip`

- `gl_in` - 内建变量：是一个接口块（类似具有in/out的struct结构）数组，存储顶点着色器的相关输出信息

  - ```cpp
    // gl_in的底层实现:
    in gl_Vertex
    {
        vec4  gl_Position;
        float gl_PointSize;
        float gl_ClipDistance[];
    } gl_in[];
    ```

- `EmitVertex` - 将`gl_Position`中的向量添加到图元中来
- `EndPrimitive` - 所有发送出（EmitVertex）的顶点会被合成为指定的输出渲染图元 => **可以多次调用**，打包成多个图元

几何着色器的使用方式类似于vs和fs，也需要编译和链接，只是着色器类型为`GL_GEOMETRY_SHADER`

⚠️注意：因为几何着色器现在插足与顶点着色器和片段着色器之间，所以vs到fs的数据传递会变动，所以我们需要**在几何着色器中完成数据的传递**。比较不一样的是，几何着色器输入是多个顶点，<font color='red'>**所以数据都是以数组形式**</font>：

```glsl
/// vertex shader
out VS_OUT{
	vec3 color;
} vs_out;
out vec2 texCoord;


/// geometry shader
// 处理输入:
in VS_OUT{
	vec3 color;
} gs_in[]; // ⚠️数组
in vec2 texCoord[];
// 处理输出
out vec3 fColor; // 这里没写成数组的形式，它的工作原理是：当EmitVertex时，会发送fTexCoord中存储的值（所以如果所有顶点TexCoord一样，只需要在shader中设置一次）
out vec2 fTexCoord; 

void main()
{
  fTexCoord = gs_in[0].color;
  ...
  EmitVertex();
  ...
  EmitVertex();
}
```

几何着色器能实现以下**有趣的功能**：

- billboard：**公告牌技术**
- **可视化法线**

使用几何着色器的一些技巧：（1）一般我们不在vertex shader中计算gl_Position到齐次裁剪空间，因为这没办法在几何着色器中进行平移等操作，我们一般先处理到view空间，再把projection矩阵传进GS；（2）我们可以让GS什么多不做，传进什么再传出什么，可以方便查找shader错误

### 11.实例化

#### （1）实例化

**实例化的作用是减少draw call**：为进行一次draw call，OpenGL会做很多准备工作（比如：告诉GPU在哪个缓冲读取数据、哪里去寻找顶点属性等），CPU与GPU之间通信耗费性能，所以我们希望尽可能减少draw call。

实例化操作时，绘制函数从`glDrawArrays()`、`glDrawElements()`替换为**`glDrawArraysInstanced()`、`glDrawElementsInstanced()`** => 实例化函数在此基础上额外添加参数**“实例数量”** => 相当于我们可以共享顶点数据，而一次性绘制多个相似的物体

通过GLSL内建变量：`gl_InstanceID`来获取当前实例索引（从0开始）

一般的操作：<font color='purple'>**我们将Uniform数据写成数组的形式，通过gl_InstanceID获取对应的属性，来进行实例化渲染**</font>

#### （2）实例化数组

实例化有一个问题，因为我们能设置的Uniform是有限的，所以通过`gl_InstanceID`来索引Uniform数组的方式，在绘制大量相同object是有问题的 

=> OpenGL引入**实例化数组**，实例化数组是一个顶点属性，仅在绘制新的实例时才会更新

区分顶点属性&实例化数组：

- 常规的顶点属性：vertex shader每次运行，对应的顶点属性就会更新
- 实例化数组：只有vertex shader绘制某个实例时，对应的实例化数组才会更新

配置实例化数组和顶点属性大致一致，都要创建VBO，配置顶点属性，只是要额外调用函数`glVertexAttribDivisor`，用于指示OpenGL在什么时候更新顶点属性：

```cpp
// layout(location = 2) in vec2 offset;
glm::vec2 offsets[100];

unsigned int vbo;
glGenBuffers(1, &vbo);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &offsets[0], GL_STATIC_DRAW);

glEnableVertexAttribArray(2);
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
glBindBuffer(GL_ARRAY_BUFFER, 0);
glVertexAttribDivsor(2, 1);
```

`glVertexAttribDivsor(GLuint index, GLuint divisor)`：

- 参数1：指定顶点属性位置
- 参数2：属性除数（默认值为0），它是指OpenGL每绘制n个实例时，就更新顶点属性；比如n=1就是每次绘制新实例就更新「**当属性除数不等于0时，相当于告诉OpenGL我们在使用实例化数组**」

从上面可以看出，我们为实例化数组单独创建了VBO？TODO

### 12.抗锯齿

#### （0）抗锯齿介绍

> 参考博客：请问FXAA、FSAA与MSAA有什么区别？效果和性能上哪个好？ - 文刀秋二的回答 - 知乎 https://www.zhihu.com/question/20236638/answer/44821615

常见抗锯齿分为：硬件层面（MSAA）、后处理（PPAA：Post Processing）

##### 走样（Aliasing）：

- **几何走样**：几何覆盖函数采样不足，也就是俗称的边缘锯齿
- **着色走样**：渲染方程的采样不足（因为渲染方程也是连续函数，可能会产生噪点等）

#### （1）SSAA

SSAA：super sample anti-aliasing - 超采样抗锯齿：使用更高的分辨率渲染，再**下采样**到正常分辨率

OpenGL没有直接的API，需要手动实现

#### （2）MSAA

> 参考博客：移动端高性能图形开发 - 详解MSAA - 拳四郎的文章 - 知乎 https://zhuanlan.zhihu.com/p/382063141

MSAA：multisampling anti-aliasing - 多重采样：采用多采样点的形式，最后在光栅化时判断有多少采样点在三角形内，这个比例乘以对应颜色，即是结果。=> **MSAA只能处理边缘的走样问题，也就是几何走样**，无法处理内部走样问题

注意的是：对于X倍的MSAA，也就是有X个采样点，那么就需要额外的存储空间来存储这些采样点的计算结果

GLFW窗口系统为我们提供了多重采样缓冲，以代替默认的颜色缓冲：扩大颜色缓冲的同时深度缓冲也会扩大

```cpp
glfwWindowHint(GLFW_SAMPLES, 4); // 4xMSAA
glEnable(GL_MULTISAMPLE); // 大多数驱动是默认启动MSAA的
```

#### （3）离屏MSAA

离屏MSAA需要一个**支持存储多个采样点的纹理**，我们使用`glTexImage2DMultisample`来代替`glTexImage2D`，对应的纹理对象是`GL_TEXTURE_2D_MULTISAMPLE`

```cpp
glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
// samplers:样本个数
// 最后一个参数:图像将会对每个纹素使用相同的样本位置，以及相同数量的子采样点个数
glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

// 多重采样纹理附加到帧缓冲上
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex, 0);
```

`glRenderbufferStorage`替换为`glRenderbufferStorageMultisample`

```cpp
glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
```

因为多重采样的图像包含比普通图像更多的信息，不能直接采样，所以我们需要进行缩小/还原图像（降采样），使用**`glBlitFramebuffer`**来完成 => 主要作用是将多重采样帧缓冲区的内容复制到普通帧缓冲区

```cpp
glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampledFBO);
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 0: 默认帧缓冲
glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
// 借助GL_READ_FRAMEBUFFER和GL_WRITE_FRAMEBUFFER实现复制
```

这里渲染到默认缓冲区，也可以选择渲染到没有使用多重采样纹理的fbo上

#### （4）自定义抗锯齿算法

其实可以将多重采样的纹理图像直接传入shader，GLSL提供了这样的选项，让我们能对每个子样本进行采样

```cpp
uniform sampler2DMS screenTextureMS; // sampler2DMS
vec4 col = texelFetch(screenTextureMS, TexCoords, 3); // 第4个子样本(0/1/2/3)
```

#### （5）回顾SSAA

根据MSAA的启发，我们可以手动实现SSAA，具体步骤如下：

- 更高分辨率的渲染：调整视口、创建帧缓冲
- 渲染到纹理
- 手动将采样，通过shader来渲染到常规大小的缓冲区【也可以使用`glBlitFramebuffer`】
- 渲染

#### （6）常见抗锯齿

- SSAA：超级采样抗锯齿

- MSAA：多重采样抗锯齿【不与“延迟渲染”兼容】

- TAA：Temporal AA：（后处理）在时间维度上，综合历史帧的数据来实现抗锯齿

  - > https://zhuanlan.zhihu.com/p/425233743

- **FXAA**：Fast Approximate AA：（后处理）形变抗锯齿，检测出图像边缘的区域，通过两侧像素混合来实现抗锯齿 => 性能便宜，但会变糊

  - > https://zhuanlan.zhihu.com/p/431384101

- MLAA：同样是边缘检测+像素混合，是FXAA的改进版本

- **SMAA**：（MLAA的加强版）

## S4 高级光照

### 1.Bloom - 泛光/光晕

#### （1）bloom概述

因为监视器亮度范围有限，所以一种策略就是将光源的光“泄漏”，看起来像是强光区一样

步骤：我们确定一个颜色亮度阈值，超过这个阈值的光我们提取出来，进行模糊处理（一般是高斯模糊），那么这个模糊的纹理贴在原渲染图上，就产生了bloom效果

#### （2）MRT（Multiple Render Targets）多渲染目标

我们一个像素着色器其实不仅可以有一个输出，我们可以输出到多个颜色缓冲中

```glsl
// 输出到两个缓冲
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
```

要实现像素着色器的多个输出，需要：**有多个颜色缓冲附加到了当前绑定的帧缓冲对象上**，要通过`glFramebufferTexture2D`绑定多个`GL_COLOR_ATTACHMENT...`，并显示告诉OpenGL正在绘制到多个颜色缓冲：

```cpp
GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
glDrawBuffers(2, attachments);
```



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

  - > 参考博客：https://cloud.tencent.com/developer/article/2071212?areaSource=102001.1&traceId=P4orT5916tzcyP5_HS6_2 

  - 根据开发者所说，当前Render Doc在Mac的支持是有限的，只能保证编译通过

- Xcode 和 Instruments

  - Instruments作为Xcode内置的性能分析工具，能够分析CPU、内存、图形性能等
  - 对于图形调试，你可以使用 Instruments 中的 "OpenGL ES Analyzer"（针对 OpenGL ES）或者 "Metal System Trace"（针对 Metal）工具来分析图形性能和资源使用情况

- ~~OpenGL Profiler~~（已被Mac弃用）
  - macOS内置的图形分析工具，用于OpenGL应用程序的性能分析
  - 也是内置在Xcode-Instruments中

## C2 imgui

### 1.简单使用流程

#### （1）头文件

```cpp
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
```

#### （2）初始化并配置imgui

```cpp
IMGUI_CHECKVERSION();
```

宏，用于检测使用的ImGui版本与代码是否兼容

```cpp
ImGui::CreateContext();
```

创建ImGui上下文，上下文一般用于管理状态和配置信息（同OpenGL上下文一样）

```cpp
ImGuiIO& io = ImGui::GetIO(); (void)io;
```

获取ImGui输入输出的结构体，包含了用户界面的各种配置；

<font color='darkpink'>**`(void)io;`是一种trick写法，没有实际作用，是为了避免编译器报告未使用的变量警告**</font>

```cpp
io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
```

Imgui开启键盘控制

```cpp
ImGui::StyleColorsDark();
```

设置ImGui的样式主题，这里是设置为暗色主题

```cpp
ImGui_ImplGlfw_InitForOpenGL(window, true);
```

这个函数的参数：传入一个glfw窗口的指针，和是否为ImGui注册事件回调函数，该函数让ImGui创建一个适合于在GLFW窗口中绘制的上下文，并起诶创建一个可交互的用户界面

```cpp
ImGui_ImplOpenGL3_Init("#version 330");
```

该函数需要传入一个GLSL版本号的常量字符串，用于指示Imgui在创建shader时应该使用哪个glsl版本的shader，让用户可以在渲染循环中使用Imgui相关函数来**渲染用户界面**

#### （3）渲染主循环

```cpp
ImGui_ImplOpenGL3_NewFrame();
ImGui_ImplGlfw_NewFrame();
ImGui::NewFrame();
```

确保ImGui能正确协同OpenGL和GLFW进行渲染，`NewFrame()`用于开启新一帧的渲染，会清理上一针的绘制命令 => **需要在渲染循环迭代开头调用这三个函数**

```cpp
ImGui::Render();
```

在调用ImGui渲染指令后调用该函数，会真正实行这些渲染指令并绘制UI到后台缓冲区中 => **需要在渲染循环结束时调用这个函数**

<font color='purple'>**渲染指令**</font>

```cpp
ImGui::Begin("window name");
... // 各种控件,比如SliderFloat,CheckBox...
ImGui::End();
```

`Begin()`和`End()`之间是一个窗口组件，窗口中可以设置各种控件，这个就不一一介绍了，这些渲染指令调用需要在上述👆几个函数之间使用

> 🌟常用控件参考博客：https://zhuanlan.zhihu.com/p/485376285

**DemoWindow**

```cpp
ImGui::ShowDemoWindow();
```

ImGui提供一个已经创建好的demo-window，我们可以通过这一个函数来创建它辅助我们使用Imgui

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308151247073.png" alt="截屏2023-08-15 12.47.03" style="zoom:50%;" />

#### （4）Imgui常见问题

- 如果Imgui窗口没有正确显示，请查看主循环中每帧的`glClear`函数，是否在Imgui之后渲染，如果是，则窗口的显示会被glClear所覆盖

## C3 GLSL常见问题

### 1.uniform location

GLSL对数组和自定义结构的支持很奇怪：

- **struct**：

  - 当在shader中定义一个struct，并将其设为uniform时，`glGetUniformLocation()`函数很奇怪

  - ```cpp
    // shader
    struct Light{
    	vec3 strength;
    	vec3 position;
    };
    uniform Light light;
    
    // cpp
    glGetUniformLocation(ID, "light"); // -1
    glGetUniformLocation(ID, "light.strength"); // 0
    glGetUniformLocation(ID, "light.position"); // 1
    ```

    也就是说，当uniform涉及到struct时，无法通过struct变量名获取地址，必须准确到成员名`light.position`

- **普通数组**：

  - 单纯的数组是支持通过数组名获取location的，但是数组不能是struct数组

  - ```cpp
    // shader
    uniform vec3 colors[10];
    
    // cpp
    glGetUniformLocation(ID, "colors"); // 0
    glGetUniformLocation(ID, "colors[0]"); // 0
    glGetUniformLocation(ID, "colors[1]"); // 1
    ```

- **struct数组**：

  - struct数组就很麻烦了，必须定位到数组的第几个元素的哪个成员

  - ```cpp
    // shader
    // shader
    struct Light{
    	vec3 strength;
    	vec3 position;
    };
    const int N = 10;
    uniform Light lights[N];
    
    // cpp
    glGetUniformLocation(ID, "lights"); // -1
    glGetUniformLocation(ID, "lights[0]"); // -1
    glGetUniformLocation(ID, "lights[0].strength"); // 0
    glGetUniformLocation(ID, "lights[0].position"); // 1
    glGetUniformLocation(ID, "lights[1].strength"); // 2
    
    glUniform1fv(glGetUniformLocation(ID, "lights[0].strength"), N * sizeof(Light) / sizeof(float), reinterpret_cast<float*>(lights)); // 这种trick方法也不行,TODO:查清楚为什么不行,有可能是内存对齐问题,或者是什么其他问题?
    ```

🤔️为什么会是👆这种情况不知道，有可能是OpenGL/GLSL版本比较低，不支持，通过实际操作总结出以上规律；ChatGPT给我的解释是：

<img src="https://cdn.jsdelivr.net/gh/shuaigougou5545/blog-image/img/202308221713907.png" alt="截屏2023-08-22 17.12.31" style="zoom:50%;" />

### 2.内存对齐

GLSL内存对齐有两种方式：`std140`布局 & `std430`布局

- `std140`：要求数据按照4字节对齐，每个数据会在每4字节的边界上对齐，也就是说，如果不够四字节，则剩余空间将被填充
- `std430`：要求数据按照4字节对齐，但数据更紧凑，会紧凑排布数据，没有额外的填充

`uniform` 是一种特殊的全局 buffer，只可读，默认 std140 布局且无法修改

> 参考：https://blog.csdn.net/qq_62464995/article/details/128440953

TODO：补充～

### 3.优化

shader中uniform变量如果没有用到，则会被优化删除掉，所以会导致glGetUniformLocation找不到位置值，只是一个很严重的bug

## C4  OpenGL导出图片

#### 1.stb_image库

或者就用我们之前加载纹理时的库，这里要去下载`stb_image_write`库，和stb_image使用原理类似

```cpp
void WindowSystem::renderToPicture(const char* file_name, GLint fbo)
{
    // PS: 每次循环中,窗口处理在渲染处理之后,所以这里能获取到当前帧的渲染数据
    // 输出fbo帧缓冲,fbo默认值为0,也就是默认帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    int channels = 4;
    unsigned char* data = (unsigned char*)malloc(channels * m_width * m_height);
    glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // 上下翻转
    unsigned char* flipped_data = (unsigned char*)malloc(channels * m_width * m_height);
    for (int y = 0; y < m_height; y++) {
        memcpy(flipped_data + (m_height - 1 - y) * channels * m_width, data + y * channels * m_width, channels * m_width);
    }
    
    enum Type { bmp };
    Type type = bmp;
    switch (type) {
        case bmp:
            // bmp与OpenGL坐标系不同,bmp原点在左上角,opengl在左下角,需要对图像数据进行翻转
            stbi_write_bmp(file_name, m_width, m_height, channels, flipped_data);
            break;
            
        default:
            break;
    }
    
    
#ifdef __APPLE__
    // 在Mac OS下打开文件
    std::string command = "open " + std::string(file_name);
    int result = system(command.c_str());
#endif
    
    free(data);
    free(flipped_data);
}
```

## Q1 多线程

#### （1）渲染&窗口处理

当窗口事件处理和渲染在同一主线程中执行时，拖动窗口会导致主线程被阻塞，这样就会出现：拖动窗口时，渲染停止运行，直到拖动操作结束

解决方法：将窗口事件处理和渲染分离到不同的线程中 -- 比如，创建一个线程专门用于渲染，窗口事件就放在主线程中【要求：UI事件（GLFW窗口事件）必须放在主线程中】

TODO：将窗口事件处理和渲染分离到不同的线程中

## Q2 视网膜显示器 - MacOS

对于视网膜显示器，像素密度通常更高。我们一般需要：

- 提高帧缓冲区的分辨率
  - 一般将帧缓冲的分辨率设置为实际物理屏幕分辨率的两倍，以匹配高分辨率屏幕的像素密度

TODO：后续在继续研究这个问题，目前我将viewport设置为实际窗口大小的两倍

## Q3 常见贴图类型

> 参考文章：https://zhuanlan.zhihu.com/p/260973533

## Q4 模型加载

#### （1）assimp



## M1 常见问题

OpenGL常常会出现问题，因为不好debug，如果当glGetError()都失效的话，就很难发现问题 => 所以以下总结了常见的错误原因（包括手误，或者流程设计出错）

手误/流程设计出错：

- `glClear()`的调用放在了Draw Call后面
- ⚠️所有的`gl...`状态设置函数，会持续保持，直到改变，所以在渲染循环中，上一次循环结束时设置的一些状态，如果没有在新一帧中重置，则会造成很严重的影响，比如`gl...Mask`遮罩函数
- 同上：矩阵运算也是，尤其是model，每次都需要恢复成单位矩阵

一些错误：

- 当摄像机的方向与物体的面几乎保持平行时，会导致深度值出现错误，也就是精度错误

TODO：厚度为0.1f的box有问题，检查一下

#### （2）画面闪动

一般是跟opengl状态有关，一般是深度测试的原因，请检查`glEnable(GL_DEPTH_TEST);`这个代码是否在主循环中调用（或者你只在初始化时调用了，然后又被其他修改了）

#### （3）渲染到纹理（帧缓冲）报错

若framebuffer为不完整的，可能原因：

- 检查width和height是否错误，可能为0

  ```cpp
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  ```

#### （4）Mac渲染到纹理扭曲

视网膜显示器导致的，我们需要将framebuffer长宽各扩大
