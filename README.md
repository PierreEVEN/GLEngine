
# GLEngine

__This is and abandonned project | don't use it for game development__

GLEngine was my first C++ project and my first attempt to create my own 3D game engine using OpenGL. 

## Features

- Basic lighting
- Advanced physics using Bullet3D
- Separated render and game threads
- Multithreaded data loading
- Custom asset binary format
- Render targets
- Content browser with dynamic thumbnails
- Frustrum culling
- Max display screen size
- ImGui user interface
- Easy to use CPU profiler
- Procedural landscapes
- Generated project vs files with a custom tool (C#)
- Raycasting

## Dependencies
- assimp
- glfw
- glad
- stbi
- glm

## Videos

Async data loading

[![](https://github.com/PierreEVEN/GLEngine/blob/master/Showcase/GLEngine-AsyncLoading.gif?raw=true)](https://www.youtube.com/watch?v=n23T4ktsU0o)


Lighting

[![](https://github.com/PierreEVEN/GLEngine/blob/master/Showcase/Dynamiclights.gif?raw=true)](https://www.youtube.com/watch?v=9scu9FjyV1Q)

Last build review

[![](https://github.com/PierreEVEN/GLEngine/blob/master/Showcase/GLEngine-QuickReview.gif?raw=true)](https://www.youtube.com/watch?v=yvfDptMhhlw)


## Screenshots

Scene primitive bounding boxes (used for line traces and culling)
![](https://github.com/PierreEVEN/GLEngine/blob/master/Showcase/DynamicBoundingBoxes.png?raw=true)

Dynamically generated thumbnails + object preview
![](https://github.com/PierreEVEN/GLEngine/blob/master/Showcase/Dynamic_thumbnails.png?raw=true)

Large model loading (Imported from google maps data)
![](https://github.com/PierreEVEN/GLEngine/blob/master/Showcase/LargeModelLoading.png?raw=true)

Procedural infinite landscape (async mesh generation)
![](https://github.com/PierreEVEN/GLEngine/blob/master/Showcase/ProceduralLandscape.png?raw=true)

Generated foliages (Quick implementation of my landscape library, no instancing)
![](https://github.com/PierreEVEN/GLEngine/blob/master/Showcase/ProceduralLandscaleAndFoliage.png?raw=true)
