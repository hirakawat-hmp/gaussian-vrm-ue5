# Technology Stack

## Core Libraries
- **Three.js** (v0.170.0) - 3D rendering engine
- **Gaussian Splats 3D** - Custom library for rendering Gaussian splatting point clouds
  - Location: `lib/gaussian-splats-3d.module.js`
- **@pixiv/three-vrm** (v2.1.0) - VRM character model support
- **TensorFlow.js + Pose Detection** - Automated pose estimation during preprocessing
- **JSZip** (v3.10.1) - For GVRM file format (zipped archive)

## Runtime Environment
- **Browser-based**: Runs entirely in web browsers
- **HTTPS Required**: Uses HTTPS for WebRTC and certain browser features
- **WebGL**: GPU-accelerated rendering and preprocessing

## Module System
- **ES6 Modules**: Uses import/export syntax
- **No build system**: Direct JavaScript module loading
- **No package.json**: Dependencies loaded via CDN or local lib files

## Development Server
- **Express.js**: HTTPS development server
- **Node.js**: Server runtime
- **Self-signed SSL certificates**: For local HTTPS development

## CDN Resources
When using as a library, dependencies can be loaded from:
- Three.js: `https://cdn.jsdelivr.net/npm/three@0.170.0/`
- @pixiv/three-vrm: `https://cdn.jsdelivr.net/npm/@pixiv/three-vrm@2.1.0/`
- JSZip: `https://cdn.jsdelivr.net/npm/jszip@3.10.1/+esm`
- Gaussian Splats 3D: `https://naruya.github.io/gs-edit/lib/gaussian-splats-3d.module.js`
- GVRM Library: `https://naruya.github.io/gs-edit/lib/gaussian-vrm.min.js`
