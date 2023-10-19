# TinyRenderer-IMGUI
一个基于DX11的渲染引擎（目前仅有渲染部分的内容）


# 进度情况

2023-10-19  
引擎编辑器界面构建；  
引入了`RenderGraph`框架：支持多`Pass`扩展。  
   （目前包含了`Shadow Pass`-> `Shading Pass`-> `Skybox Pass`-> `PostProcess Pass`）  
引入了`RenderGraph`的`Resource`概念：支持多Pass共享资源;  
阴影实现了基于硬件的`PCF`滤波  
![image](images/20231019_Process.png)

2023-08-25  
引入了IMGUI库；  
引入了类unity材质机制；  
![image](images/20230825_Process.png)

2023-06-23  
一阶段  
![image](images/20230623_Process.png)