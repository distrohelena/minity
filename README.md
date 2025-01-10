Idea: Develop an Unity Exporter that can support older consoles. 

Features:
- 3D Models
- Camera
- Lights
- Tesselation: Big triangles on the PS2 get easily culled, so we tesselate to make them smaller (checkbox).
- Lightmaps: We extract the lightmaps, and convert from HDR to SDR with a specific exposure.
- Code Conversion: C# to C++ code conversion (very rudimentary, but does work). Requires manually building the output code inside the ps2engine project.
- DirectX Windows version: Debug on Windows, the C++ version of the engine is cross-platform, and just loads different renderers based on the platform.

How it works:
- The exporter generates 2 binary files:
* assets.ps2: Contains all assets loaded on the scene
* scene.ps2: Contaisn the scene itself.
Right now there's no support for multiple scenes.

Installation
Windows: Just build and run the project
PS2:
- Install Tyra Engine: https://github.com/h4570/tyra/tree/master/docs/install
- Copy source code from project to tyra/demo/minity
- Copy demo_game.hpp and demo_game.cpp to demo tyra/demo folder

![14](https://github.com/user-attachments/assets/3f368225-4ad8-46ab-b918-f0b1f8627411)
![9](https://github.com/user-attachments/assets/0b2aed8e-ef66-4077-a143-86301e8321dc)
![10](https://github.com/user-attachments/assets/a97bf04e-d398-4738-b92b-6354acc0d16d)
![image](https://github.com/user-attachments/assets/2fcd2942-ee24-4806-a9e0-62896d8c7d98)
