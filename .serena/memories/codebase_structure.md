# Codebase Structure

## Root Directory
```
gaussian-vrm/
├── index.html              # Main HTML page
├── main.js                 # Main application entry point
├── server.js               # HTTPS development server
├── error.js                # Error logging and reporting utilities
├── README.md               # Project documentation
├── CLAUDE.md               # Development instructions for Claude Code
├── LICENSE.txt             # MIT License
└── .mcp.json               # MCP server configuration
```

## Core Library (`gvrm-format/`)
Core algorithms and file format handlers:
- **gvrm.js** - Main GVRM class, load/save/update logic
- **vrm.js** - VRM character management
- **gs.js** - Gaussian splatting viewer wrapper
- **ply.js** - PLY file parser
- **utils.js** - Utilities (pose operations, visualization, PMC)

## Applications (`apps/`)
Application-specific modules:

### Preprocessing (`apps/preprocess/`)
Preprocessing pipeline for converting PLY + VRM to GVRM:
- **preprocess.js** - Main preprocessing orchestration
- **preprocess_gl.js** - GPU-accelerated splat assignment
- **pose.js** - Pose detection wrapper
- **check.js** - Validation and error checking
- **utils_gl.js** - WebGL utilities

### Avatar World Demo (`apps/avatarworld/`)
Multi-avatar demo application:
- **main.js** - Entry point for avatar world
- **walker.js** - Autonomous avatar walking behavior
- **scene.js** - Scene creation (sky, floor, houses)
- **index.html** - Avatar world HTML page

### Shared Utilities
- **fps.js** - FPS counter
- **vr.js** - VR mode support

## Third-party Libraries (`lib/`)
- TensorFlow.js and pose detection models
- Three.js modules
- `gaussian-splats-3d.module.js` - Gaussian splatting renderer
- `gaussian-vrm.min.js` - Minified GVRM library for distribution

## Assets (`assets/`)
- Sample GVRM files
- FBX animation files
- `default.json` - Default bone operations configuration
- VRM model (`sotai.vrm`)

## Examples (`examples/`)
- `simple-viewer.html` - Minimal GVRM viewer example
- Example usage and integration demonstrations

## Build Artifacts
- No build directory (no compilation step)
- `.server/` - SSL certificates (not in repo, generated locally)
- `.serena/` - Serena tool workspace
- `.claude/` - Claude Code configuration

## Key Entry Points
1. **Main Application**: `index.html` → `main.js`
2. **Simple Viewer**: `examples/simple-viewer.html`
3. **Avatar World**: `apps/avatarworld/index.html` → `apps/avatarworld/main.js`
4. **GVRM Library**: `lib/gaussian-vrm.min.js`

## Data Flow
1. User loads PLY file → `main.js` detects and starts preprocessing
2. `apps/preprocess/preprocess.js` orchestrates conversion
3. Pose detection → alignment → splat assignment → GVRM creation
4. GVRM file saved (ZIP containing VRM + PLY + metadata JSON)
5. Runtime: GVRM loaded → shader customization → animation loop
