# Demo-Template

###### 🤔️用尽可能少的依赖以及函数封装，实现图形学算法，去还原API本身，便于熟悉以及记忆OpenGL-API本身

只依赖：1）glad；2）glm数学库；3）stb_image纹理读取

#### 1.代码梳理

**初始化**：

- glfw初始化
- 创建窗口，并绑定上下文
- glad初始化【在绑定上下文之后，调用OpenGL函数之前】
- 视口设置（绑定窗口变换）
- 数据准备：
  - VBO、EBO、VAO
  - shader、shader program
  - 纹理

**主循环**：

- 事件轮询
- 渲染
  - 清理缓冲区
- 交换缓冲区

**结束**：

- glfw终止
- 释放OpenGL对象