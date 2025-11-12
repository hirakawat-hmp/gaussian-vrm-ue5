# GVRM for Unreal Engine 5

This directory contains the Unreal Engine 5 implementation of GVRM (Gaussian-VRM).

## Overview

GVRM for UE5 enables real-time animation of Gaussian Splatting avatars using VRM skeletal meshes, bringing photorealistic animated characters to Unreal Engine projects.

## Directory Structure

```
ue5/
├── Plugins/
│   └── GVRMRuntime/          # UE5 plugin (C++ runtime)
│       ├── Source/
│       │   └── GVRMRuntime/
│       │       ├── Public/   # Header files
│       │       └── Private/  # Implementation files
│       └── Content/          # Plugin content
├── Tools/                     # Python conversion tools
│   └── gvrm_to_ue5.py        # GVRM to UE5 converter
├── EditorScripts/            # UE5 editor automation scripts
└── Samples/                  # Sample UE5 projects
```

## Quick Start

### Prerequisites

1. **Unreal Engine 5.3+**
2. **Python 3.8+** (for conversion tools)
3. **Required UE5 Plugins:**
   - VRM4U: https://github.com/ruyo/VRM4U
   - XVERSE XScene-UEPlugin: https://github.com/xverse-engine/XScene-UEPlugin

### Step 1: Convert GVRM File

```bash
cd ue5/Tools
python gvrm_to_ue5.py ../../assets/author.gvrm -o ./output
```

This generates:
- `model.vrm` - VRM character model
- `model.ply` - Gaussian splat point cloud
- `splat_binding.csv` - Binding data
- `metadata.json` - Metadata
- `IMPORT_INSTRUCTIONS.md` - Detailed import guide

### Step 2: Import to UE5

Follow the instructions in `output/IMPORT_INSTRUCTIONS.md` to import the converted assets into your UE5 project.

### Step 3: Setup Runtime

(Documentation in progress - see implementation plan in serena memory)

## Development Status

🚧 **Current Phase: Prototype Development**

- [x] Directory structure
- [x] Python conversion tool
- [ ] C++ Plugin implementation
  - [ ] Niagara Data Interface
  - [ ] GVRM Actor component
  - [ ] Custom HLSL modules
- [ ] Sample project
- [ ] Documentation

## Implementation Approach

This implementation follows the same core algorithm as the Three.js version:

**Linear Blend Skinning (LBS):**
```
SplatPosition = SkinMatrix * (VertexPosition + RelativePosition)

where SkinMatrix = Σ(weight_i * BoneMatrix_i)
```

See `../gvrm-format/gvrm.js:622-650` for the original GLSL implementation.

## Architecture

```
┌─────────────────────────────────────────┐
│         Development Time                │
├─────────────────────────────────────────┤
│  Python Tools (ue5/Tools/)              │
│  └─ gvrm_to_ue5.py                      │
│       ↓ converts                        │
│  CSV/JSON data                          │
│       ↓ imported to                     │
│  UE5 Data Assets (.uasset)              │
└─────────────────────────────────────────┘

┌─────────────────────────────────────────┐
│         Runtime (Game)                  │
├─────────────────────────────────────────┤
│  C++ Plugin (GVRMRuntime)               │
│  ├─ Niagara Data Interface              │
│  ├─ GVRM Actor                          │
│  └─ Custom HLSL Skinning Module         │
└─────────────────────────────────────────┘
```

## Related Documentation

- **Web Implementation:** `../gvrm-format/` (Three.js)
- **Implementation Plan:** See serena memory `ue5_gvrm_implementation_plan.md`
- **Technical Details:** See serena memory `ue5_gvrm_technical_details.md`
- **Code Examples:** See serena memory `ue5_gvrm_code_examples.md`

## Contributing

This is part of the gaussian-vrm research project. See main README for contribution guidelines.

## License

MIT License - See LICENSE file in project root.
