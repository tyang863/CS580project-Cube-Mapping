# CS580 Project: Cube Mapping

This is my final project of USC CSCI 580 3D Graphics and Rendering. 

The program contains 4 modules: illumination rendering based on Gouraud shading / Phong shading, and environment reflection / refraction simulating based on cube mapping. Change the value of the macro MODE in gz.h to specify which module to use. Click Run-Run Render during debugging in Visual Studio 2017 to see the results.

My work includes the implementation of all functions in rend.cpp, half of Application5.cpp, part of gz.h and rend.h. Other parts were implemented by course TAs. The rend.cpp performs the whole rendering process, including parsing triangles based on shading rules or cube mapping rules, space transformation, outputting the result to frame buffer and a ppm file, etc.

Gouraud shading:

![](https://raw.githubusercontent.com/OrderAngel/Picture/main/blog_files/img/Pic-Github/OpenGLconfig/20210515015728.png)

Phong shading:

![](https://raw.githubusercontent.com/OrderAngel/Picture/main/blog_files/img/Pic-Github/OpenGLconfig/20210515015816.png)

Cube mapping – skybox:

![](https://raw.githubusercontent.com/OrderAngel/Picture/main/blog_files/img/Pic-Github/OpenGLconfig/20210515015934.png)

Cube mapping – reflection:

![](https://raw.githubusercontent.com/OrderAngel/Picture/main/blog_files/img/Pic-Github/OpenGLconfig/20210515020035.png)

Cube mapping – refraction:

![](https://raw.githubusercontent.com/OrderAngel/Picture/main/blog_files/img/Pic-Github/OpenGLconfig/20210515020111.png)
