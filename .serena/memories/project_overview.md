# Project Overview

## Project Name
**Gaussian-VRM (GVRM)** / **Instant Skinned Gaussian Avatars**

## Purpose
A web-based 3D application that combines Gaussian Splatting (3DGS) point clouds with VRM (Virtual Reality Model) characters to create animated rigged avatars. The system processes PLY files containing Gaussian splats and binds them to VRM character skeletons, enabling real-time animation of photorealistic 3D scans.

## Key Features
- Convert PLY Gaussian splat scans + VRM models into GVRM format
- Real-time animation of Gaussian splat avatars
- Automated pose detection and alignment using TensorFlow.js
- VR mode support
- Web, mobile, and VR application support
- Three.js integration (avatars handled as standard Three.js objects)

## GVRM Format
A proprietary format (`.gvrm` files) that packages:
- `model.vrm`: VRM character model
- `model.ply`: Gaussian splat point cloud
- `data.json`: Binding metadata (splat-to-vertex/bone mappings, relative poses, bone operations)

## License
- **Source Code**: MIT License
- **Assets** (`./assets` and `./examples/assets/`): Research purposes only, not covered by MIT License
- Sample avatars available separately under MIT License

## Academic Context
This is the official repository for "Instant Skinned Gaussian Avatars for Web, Mobile and VR Applications" (SUI 2025 Demo Track)
- arXiv: 2510.13978
- Project page: https://gaussian-vrm.github.io/
- Demo: https://naruya.github.io/gaussian-vrm
