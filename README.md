GLEngine 
Project Root Directory:

<img width="995" height="715" alt="image" src="https://github.com/user-attachments/assets/f1930253-3ffa-4a42-b231-2255ff430d28" />

Terminal prints logger and layer debug messages:

<img width="2050" height="435" alt="image" src="https://github.com/user-attachments/assets/06395bd0-512b-4487-b55c-e9d3d07f6443" />

First render basic triangle using uniform to change color:

<img width="752" height="490" alt="Recording Basic Triangle" src="https://github.com/user-attachments/assets/4ba694a9-f100-4517-87b6-a4f50ab8a9c7" />

Second render basic Rectangle using uniform to change color 
and implemented indices to save VRAM improve rendering performance:

<img width="708" height="474" alt="Recording Basic Rectangle" src="https://github.com/user-attachments/assets/6205dd06-f071-46b3-b1b0-9bc566d0ce2c" />


Rectangle move using input Shader UniformMat4 transform:

<img width="3506" height="1986" alt="Recording Basic Rectangle move with input " src="https://github.com/user-attachments/assets/fcc2f544-fadb-4ca6-a45a-c01cc387f538" />


Camera system added Camera class wrapper and shader uniformMat4 ViewProjection * UniformMat4 transformation:

<img width="722" height="442" alt="Recording Square transform UniformMat4   ViewProjection UniformMat4 Camera movement" src="https://github.com/user-attachments/assets/88bcfea4-fe39-4167-a17b-9f1e7ce991af" />


Texture class Wrapper stb library loader , shader layout location 1 pass UVs TexCoord Basic Snow 2k texture:

<img width="690" height="642" alt="image" src="https://github.com/user-attachments/assets/3c39bacf-dc48-4167-a839-26802f18604f" />


Vertices and indices cout increase , glEnable(GL_DEPTH_TEST_); inside OnAttach function GameLayer.cpp
to help draw back facing sides or they will draw ontop of front facing sides turning the cube inside out. 
Also OnRender function glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); this clears old depth and color
data per frame:

<img width="582" height="470" alt="Recording Cube Snow Texture 2k " src="https://github.com/user-attachments/assets/6f4a9fb3-7135-4a12-a397-c3c668d17ea2" />


Basic Mesh Rigged Blender Model Human obj file from free3d website, file loaded using assimp library
using class wrapper ModelLoader, same snow text as cube: 

<img width="3790" height="1637" alt="image" src="https://github.com/user-attachments/assets/0b909e0c-ac9f-4e11-8201-1e4a95c0ef01" />


Light source rays created using god rays vert and frag, 2D image processing tricks to make
it look like its bleeding and stretching outwards from light source. Raymarching Screen Space Radial Blur
it walks a line towards the lights screen position, samples the colors along that path and blends them together
to create streaky glowing light trail:

<img width="706" height="524" alt="Recording Basic Mesh Rigged Human   Cube God rays " src="https://github.com/user-attachments/assets/04719b34-e407-404a-8cef-28fb8885f43b" />



