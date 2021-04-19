# mesh-viewer

OPenGL applications for viewing meshes

## How to build

*Windows*

Open git bash to the directory containing this repository.

```
mesh-viewer $ mkdir build
mesh-viewer $ cd build
mesh-viewer/build $ cmake -G "Visual Studio 16 2019" ..
mesh-viewer/build $ start CS312-MeshViewer.sln
```

Your solution file should contain multiple projects, such as `mesh-viewer`.
To run from the git bash command shell, 

```
mesh-viewer/build $ ../bin/Debug/mesh-viewer
```

*macOS*

Open terminal to the directory containing this repository.

```
mesh-viewer $ mkdir build
mesh-viewer $ cd build
mesh-viewer/build $ cmake ..
mesh-viewer/build $ make
```

To run each program from build, you would type

```
mesh-viewer/build $ ../bin/mesh-viewer
```

## Mesh demo

Default Shader (Phong per vertex shader):

<img width="493" alt="Screen Shot 2021-04-19 at 2 37 36 PM" src="https://user-images.githubusercontent.com/48161551/115289551-b3a6a380-a120-11eb-993b-92c047cc6147.png">

Phong per fragment shader:

<img width="493" alt="Screen Shot 2021-04-19 at 2 39 43 PM" src="https://user-images.githubusercontent.com/48161551/115289552-b43f3a00-a120-11eb-9374-28648decf69a.png">

Toon shader:

<img width="494" alt="Screen Shot 2021-04-19 at 2 40 19 PM" src="https://user-images.githubusercontent.com/48161551/115289554-b43f3a00-a120-11eb-9940-89898fb1dc86.png">

User can change colors of model:

Use R,G,B keys to increase red, green, and blue of diffuse color

<img width="492" alt="Screen Shot 2021-04-19 at 2 44 58 PM" src="https://user-images.githubusercontent.com/48161551/115289673-d769e980-a120-11eb-8c88-f32941b06c81.png">

Use C key to reset color to dark gray

<img width="497" alt="Screen Shot 2021-04-19 at 2 45 05 PM" src="https://user-images.githubusercontent.com/48161551/115289675-d8028000-a120-11eb-855b-862886a4d712.png">


Elephant model from https://www.cgtrader.com/free-3d-models/animals/mammal/elephant-59f6d447-7685-4692-94bb-8a3333dfe0e7



