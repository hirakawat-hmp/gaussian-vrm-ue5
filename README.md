# Gaussian-VRM for Unreal Engine 5

**Bringing Photorealistic Animated Gaussian Splatting Avatars to Unreal Engine**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![UE5](https://img.shields.io/badge/Unreal%20Engine-5.3%2B-blue)](https://www.unrealengine.com/)
[![Status](https://img.shields.io/badge/Status-Prototype-orange)](https://github.com/hirakawat-hmp/gaussian-vrm-ue5)

This repository extends the original [gaussian-vrm](https://github.com/naruya/gaussian-vrm) web implementation to Unreal Engine 5, enabling real-time animation of Gaussian Splatting avatars using VRM skeletal meshes in game engines.

---

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Quick Start](#quick-start)
- [Project Structure](#project-structure)
- [Development Environment Setup](#development-environment-setup)
  - [Windows Setup](#windows-setup-primary-development)
  - [macOS Setup](#macos-setup-design--preparation)
- [Implementation Roadmap](#implementation-roadmap)
- [Architecture](#architecture)
- [Development Workflow](#development-workflow)
- [Contributing](#contributing)
- [License](#license)

---

## 🎯 Overview

**Gaussian-VRM (GVRM)** combines:
- **Gaussian Splatting** - Photorealistic 3D representation from neural radiance fields
- **VRM** - Standard format for 3D humanoid avatars
- **Unreal Engine 5** - Industry-leading game engine

This enables:
- ✅ Real-time animation of photorealistic scanned avatars
- ✅ 60+ FPS performance on modern hardware
- ✅ Standard VRM animation compatibility (Mixamo, etc.)
- ✅ Full UE5 integration (Niagara, Blueprint, C++)

### Original Implementation

This project is based on the **Three.js implementation** of Gaussian-VRM:
- **Web Version:** [naruya/gaussian-vrm](https://github.com/naruya/gaussian-vrm)
- **Research Paper:** [Instant Skinned Gaussian Avatars](https://arxiv.org/abs/2510.13978) (SUI 2025)

---

## ✨ Features

### Current (Phase 1 - Complete)
- [x] Python conversion tool (GVRM → UE5 format)
- [x] 76K+ splat processing tested
- [x] UE5 plugin structure
- [x] Comprehensive documentation

### In Progress (Phase 2-3)
- [ ] Niagara Data Interface for skeletal mesh skinning
- [ ] Custom HLSL shaders (Linear Blend Skinning)
- [ ] GVRM Actor component
- [ ] Runtime animation support

### Planned (Phase 4-5)
- [ ] LOD system (distance-based)
- [ ] VR support
- [ ] Blueprint integration
- [ ] Performance optimizations

---

## 🚀 Quick Start

### Prerequisites

**For All Developers:**
- Python 3.8+ (conversion tools)
- Git

**For Windows Developers (Runtime Development):**
- Unreal Engine 5.3+
- Visual Studio 2022 (with C++ workload)
- Required UE5 Plugins:
  - [VRM4U](https://github.com/ruyo/VRM4U)
  - [XVERSE 3D-GS](https://github.com/xverse-engine/XScene-UEPlugin)

**For macOS Developers (Design/Preparation):**
- Xcode 14+ (for header file editing)
- Optional: UE5 macOS version (for testing)

### Installation

```bash
# Clone repository
git clone git@github.com:hirakawat-hmp/gaussian-vrm-ue5.git
cd gaussian-vrm-ue5

# Setup Python environment
cd ue5/Tools
curl -LsSf https://astral.sh/uv/install.sh | sh  # Install uv
uv venv
source .venv/bin/activate  # Windows: .venv\Scripts\activate

# Test conversion tool
uv run gvrm_to_ue5.py ../../assets/author.gvrm -o ./output
```

### Sample Output

The conversion tool generates:
- `model.vrm` - VRM character model
- `model.ply` - Gaussian splat point cloud
- `splat_binding.csv` - 76,337 splat-to-vertex bindings
- `metadata.json` - Scale and bone operations
- `IMPORT_INSTRUCTIONS.md` - UE5 import guide

---

## 📁 Project Structure

```
gaussian-vrm-ue5/
├── gvrm-format/              # Original Three.js implementation
├── apps/                     # Web applications
├── assets/                   # Sample GVRM files (author.gvrm, samples)
├── ue5/                      # 🎮 Unreal Engine 5 Implementation
│   ├── Plugins/
│   │   └── GVRMRuntime/      # Main UE5 plugin (C++)
│   │       ├── Source/
│   │       │   └── GVRMRuntime/
│   │       │       ├── Public/   # Header files (.h)
│   │       │       └── Private/  # Implementation (.cpp)
│   │       ├── Content/      # Blueprint, materials, etc.
│   │       ├── Shaders/      # HLSL shaders (.usf)
│   │       ├── GVRMRuntime.uplugin
│   │       └── Resources/
│   ├── Tools/                # Python conversion tools
│   │   ├── gvrm_to_ue5.py   # Main converter
│   │   ├── pyproject.toml   # uv configuration
│   │   └── README.md
│   ├── EditorScripts/        # UE5 editor automation (optional)
│   ├── Samples/              # Sample UE5 projects (future)
│   └── README.md
└── .serena/                  # Development documentation
    └── memories/
        ├── ue5_gvrm_implementation_plan.md
        ├── ue5_gvrm_technical_details.md
        └── ue5_gvrm_code_examples.md
```

---

## 🛠️ Development Environment Setup

### Windows Setup (Primary Development)

**Required for:**
- C++ plugin compilation
- UE5 runtime testing
- Final performance validation

#### Step 1: Install Visual Studio 2022

```bash
# Download Visual Studio 2022 Community
https://visualstudio.microsoft.com/downloads/

# Required Workloads:
- Desktop development with C++
- Game development with C++
```

#### Step 2: Install Unreal Engine 5

```bash
# Method 1: Epic Games Launcher (Recommended)
1. Download Epic Games Launcher
2. Library → Unreal Engine → Install Engine (5.3+)

# Method 2: GitHub Source (Advanced)
git clone https://github.com/EpicGames/UnrealEngine.git
```

#### Step 3: Install Required Plugins

**VRM4U Plugin:**
```bash
# Clone VRM4U
git clone https://github.com/ruyo/VRM4U.git

# Copy to your UE5 project
cp -r VRM4U/Plugins/VRM4U <YourProject>/Plugins/
```

**XVERSE 3D-GS Plugin:**
```bash
# Clone XVERSE plugin
git clone https://github.com/xverse-engine/XScene-UEPlugin.git

# Copy to your UE5 project
cp -r XScene-UEPlugin/UEPlugin/Plugin/XV3DGS <YourProject>/Plugins/
```

#### Step 4: Setup GVRM Plugin

```bash
# Open UE5 project
# File → New Project → Games → Third Person (C++)

# Copy GVRM plugin
cd <YourProject>/Plugins
mklink /D GVRMRuntime "<repo>/ue5/Plugins/GVRMRuntime"

# Or copy directly
xcopy /E /I "<repo>\ue5\Plugins\GVRMRuntime" "Plugins\GVRMRuntime\"

# Generate project files
# Right-click .uproject → Generate Visual Studio project files

# Open in Visual Studio and compile
```

#### Step 5: Python Tools Setup (Windows)

```bash
# Install uv (Python package manager)
powershell -c "irm https://astral.sh/uv/install.ps1 | iex"

# Setup Python environment
cd ue5/Tools
uv venv
.venv\Scripts\activate

# Test conversion
uv run gvrm_to_ue5.py ..\..\assets\author.gvrm -o .\output
```

---

### macOS Setup (Design & Preparation)

**Use Case:**
- Header file design and editing
- Shader code preparation
- Documentation and planning
- Python tool development

**Note:** macOS development is **optional** for UE5 plugin development. Final compilation and testing require Windows environment.

#### Step 1: Install Xcode

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Or install full Xcode from App Store
```

#### Step 2: Python Tools Setup (macOS)

```bash
# Install uv
curl -LsSf https://astral.sh/uv/install.sh | sh

# Setup environment
cd ue5/Tools
uv venv
source .venv/bin/activate

# Test conversion
uv run gvrm_to_ue5.py ../../assets/author.gvrm -o ./output
```

#### Step 3: Header File Editing

```bash
# Open with your favorite editor
code ue5/Plugins/GVRMRuntime/Source/GVRMRuntime/Public/
```

**What you CAN do on macOS:**
- ✅ Edit C++ header files (.h)
- ✅ Write HLSL shaders (.usf)
- ✅ Design plugin structure
- ✅ Develop Python tools
- ✅ Write documentation

**What you CANNOT do on macOS (requires Windows):**
- ❌ Compile the plugin
- ❌ Test in UE5 runtime
- ❌ Use Windows-specific plugins (if any)

---

## 🗺️ Implementation Roadmap

### Phase 1: Setup & Conversion Tools ✅ **COMPLETE**

**Duration:** 2 weeks (Completed)

**Deliverables:**
- [x] Python conversion tool (`gvrm_to_ue5.py`)
- [x] UE5 plugin directory structure
- [x] Plugin configuration files (`.uplugin`, `Build.cs`)
- [x] 76K+ splat conversion tested

**Platform:** Any (Python only)

---

### Phase 2: Data Pipeline 🚧 **IN PROGRESS**

**Duration:** 1 week

**Tasks:**
1. Create UE5 Data Asset types
   - `UGVRMBindingData` class
   - CSV → Data Table importer
2. Implement data validation
3. Create sample import workflow

**Required Environment:** Windows + UE5

**Current Status:** Design phase (macOS)

---

### Phase 3: Niagara Data Interface (NDI) 🔜 **NEXT**

**Duration:** 2-3 weeks

**Key Files:**
```cpp
// ue5/Plugins/GVRMRuntime/Source/GVRMRuntime/Public/
NiagaraDataInterfaceGVRM.h          // NDI header
GVRMSkinningData.h                  // Data structures

// ue5/Plugins/GVRMRuntime/Source/GVRMRuntime/Private/
NiagaraDataInterfaceGVRM.cpp        // NDI implementation
```

**Implementation:**
1. **GPU Buffer Management**
   - Bone matrices buffer
   - Vertex positions buffer
   - Skinning weights buffer

2. **NDI Functions** (exposed to Niagara)
   ```cpp
   GetBoneMatrix(int BoneIndex) → FMatrix
   GetVertexPosition(int VertexIndex) → FVector
   ComputeSkinnedPosition(int VertexIndex, FVector RelativePos) → FVector
   ```

3. **HLSL Integration**
   - Custom shader module for LBS (Linear Blend Skinning)
   - Based on `gvrm-format/gvrm.js:622-650` (original GLSL)

**Required Environment:** Windows + UE5

**Preparation on macOS:**
- ✅ Design NDI class structure
- ✅ Write header files
- ✅ Prepare HLSL shader code
- ❌ Compilation/testing (requires Windows)

---

### Phase 4: Runtime Integration

**Duration:** 1 week

**Components:**
1. **GVRM Actor Component**
   ```cpp
   AGVRMActor
   ├─ USkeletalMeshComponent (VRM)
   ├─ UNiagaraComponent (Gaussian Splats)
   └─ UGVRMBindingData (Data Asset)
   ```

2. **Animation Update Loop**
   ```cpp
   void AGVRMActor::Tick(float DeltaTime)
   {
       // 1. Update VRM animation
       // 2. Update NDI GPU buffers
       // 3. Niagara updates splat positions
   }
   ```

3. **Blueprint Integration**
   - Expose properties to Blueprint
   - Create sample Blueprint actors

**Required Environment:** Windows + UE5

---

### Phase 5: Optimization & Polish

**Duration:** 2 weeks

**Features:**
1. **LOD System**
   - Distance-based splat density reduction
   - 3-tier LOD (High/Medium/Low)

2. **Performance**
   - Compute shader optimization
   - Frustum culling
   - Occlusion culling

3. **Documentation**
   - API reference
   - Integration tutorial
   - Performance best practices

**Required Environment:** Windows + UE5

---

## 🏗️ Architecture

### Data Flow

```
┌─────────────────────────────────────────┐
│      Development Time (Any OS)          │
├─────────────────────────────────────────┤
│  GVRM File (author.gvrm)                │
│         ↓                               │
│  Python: gvrm_to_ue5.py                 │
│         ↓                               │
│  CSV + JSON + VRM + PLY                 │
└─────────────────────────────────────────┘
         ↓ Import to UE5 (Windows)
┌─────────────────────────────────────────┐
│      UE5 Editor (Windows)               │
├─────────────────────────────────────────┤
│  UE5 Data Assets (.uasset)              │
│  ├─ Data Table (CSV → struct)           │
│  ├─ GVRM Binding Data                   │
│  ├─ VRM Skeletal Mesh (via VRM4U)      │
│  └─ Gaussian Splat (via XVERSE)        │
└─────────────────────────────────────────┘
         ↓ Runtime
┌─────────────────────────────────────────┐
│      UE5 Runtime (Game)                 │
├─────────────────────────────────────────┤
│  AGVRMActor                             │
│  ├─ Update Animation (VRM skeleton)    │
│  ├─ Update NDI GPU Buffers             │
│  │   ├─ Bone matrices                  │
│  │   ├─ Vertex positions               │
│  │   └─ Skinning weights               │
│  └─ Niagara System                     │
│      ├─ Custom HLSL: Compute LBS       │
│      ├─ Update Splat Positions         │
│      └─ Render Gaussian Splats         │
└─────────────────────────────────────────┘
```

### Core Algorithm: Linear Blend Skinning (LBS)

```hlsl
// GVRM Skinning Algorithm (HLSL)
// Ported from gvrm.js:622-650 (GLSL)

float3 ComputeSplatPosition(int vertexIndex, float3 relativePos)
{
    // 1. Get skinning data for this vertex
    float4 skinIndices = GetVertexSkinIndices(vertexIndex);
    float4 skinWeights = GetVertexSkinWeights(vertexIndex);

    // 2. Compute skin matrix (Linear Blend Skinning)
    float4x4 skinMatrix =
        skinWeights.x * GetBoneMatrix(skinIndices.x) +
        skinWeights.y * GetBoneMatrix(skinIndices.y) +
        skinWeights.z * GetBoneMatrix(skinIndices.z) +
        skinWeights.w * GetBoneMatrix(skinIndices.w);

    // 3. Transform splat position
    float3 vertexPos = GetVertexPosition(vertexIndex);
    float3 localPos = vertexPos + relativePos;
    return mul(skinMatrix, float4(localPos, 1.0)).xyz;
}
```

**This is the SAME algorithm as the Three.js version** - ensuring consistency between web and game engine implementations.

---

## 💻 Development Workflow

### For Windows Developers (Full Development)

```bash
# 1. Setup environment (first time only)
# - Install Visual Studio 2022
# - Install UE5 5.3+
# - Install VRM4U and XVERSE plugins

# 2. Clone and setup
git clone git@github.com:hirakawat-hmp/gaussian-vrm-ue5.git
cd gaussian-vrm-ue5

# 3. Create/Open UE5 project
# - Open UE5 Editor
# - Create new C++ Third Person project
# - Link GVRMRuntime plugin

# 4. Compile plugin
# - Generate VS project files
# - Open in Visual Studio
# - Build Development Editor configuration

# 5. Test in editor
# - Convert GVRM file with Python tool
# - Import assets to UE5
# - Test runtime functionality

# 6. Iterate
# - Edit C++ code in Visual Studio
# - Hot reload in UE5 Editor
# - Commit changes
```

### For macOS Developers (Design & Preparation)

```bash
# 1. Clone repository
git clone git@github.com:hirakawat-hmp/gaussian-vrm-ue5.git
cd gaussian-vrm-ue5

# 2. Setup Python tools
cd ue5/Tools
uv venv && source .venv/bin/activate
uv run gvrm_to_ue5.py ../../assets/author.gvrm -o ./output

# 3. Design phase work
# - Edit header files (.h) in ue5/Plugins/GVRMRuntime/Source/
# - Write HLSL shaders (.usf)
# - Update documentation
# - Develop Python tools

# 4. Commit design work
git add ue5/Plugins/GVRMRuntime/Source/
git commit -m "Add NDI header files"
git push

# 5. Handoff to Windows for compilation/testing
# - Windows developer pulls changes
# - Compiles and tests in UE5
# - Reports back results
```

### Cross-Platform Collaboration

**Workflow:**
1. **macOS:** Design headers, write shaders, plan architecture
2. **Commit to GitHub**
3. **Windows:** Pull, compile, test, iterate
4. **Report results back**
5. **Repeat**

---

## 📚 Documentation

### Technical References

All detailed technical documentation is stored in `.serena/memories/`:

- **Implementation Plan:** `ue5_gvrm_implementation_plan.md`
  - Phase-by-phase development plan
  - Milestones and deliverables
  - Risk assessment

- **Technical Details:** `ue5_gvrm_technical_details.md`
  - Architecture deep dive
  - Data flow diagrams
  - LBS algorithm details
  - GPU buffer management

- **Code Examples:** `ue5_gvrm_code_examples.md`
  - Complete NDI implementation
  - HLSL shader code
  - Python conversion tools
  - Blueprint setup examples

- **Python Usage:** `ue5_python_usage_guide.md`
  - Python tool architecture
  - Offline vs runtime usage
  - Development workflow

### Quick Links

- **Web Implementation Reference:** `gvrm-format/gvrm.js`
  - Original GLSL shaders: lines 533-776
  - Update loop: lines 306-356
- **Conversion Tool:** `ue5/Tools/gvrm_to_ue5.py`
- **Plugin Config:** `ue5/Plugins/GVRMRuntime/GVRMRuntime.uplugin`

---

## 🤝 Contributing

### Getting Started

1. **Fork the repository**
2. **Choose your development path:**
   - **Windows:** Full C++ plugin development
   - **macOS:** Design, headers, shaders, Python tools
3. **Create feature branch:** `git checkout -b feature/amazing-feature`
4. **Commit changes:** `git commit -m 'Add amazing feature'`
5. **Push to branch:** `git push origin feature/amazing-feature`
6. **Open Pull Request**

### Contribution Areas

**All Platforms:**
- 📝 Documentation improvements
- 🐛 Bug reports and testing
- 💡 Feature suggestions
- 🐍 Python tool enhancements

**Windows:**
- 🔧 C++ plugin development
- 🎮 UE5 runtime testing
- ⚡ Performance optimization
- 📦 Packaging and distribution

**macOS:**
- 🎨 Header file design
- ⚙️ Shader development (HLSL)
- 📊 Architecture planning
- 🐍 Python tool development

---

## 📄 License

This project is licensed under the **MIT License** - see [LICENSE](LICENSE.txt) file for details.

### Component Licenses

- **Source Code:** MIT License
- **Assets** (`./assets/`): Research purposes only (see original repo)
- **Sample Avatars:** See [original repository](https://github.com/naruya/gaussian-vrm)

### Original Work

This project extends [gaussian-vrm](https://github.com/naruya/gaussian-vrm) by naruya.

**Research Paper:**
```bibtex
@misc{kondo2025instantskinnedgaussianavatars,
      title={Instant Skinned Gaussian Avatars for Web, Mobile and VR Applications},
      author={Naruya Kondo and Yuto Asano and Yoichi Ochiai},
      year={2025},
      eprint={2510.13978},
      archivePrefix={arXiv},
      primaryClass={cs.CG},
}
```

---

## 🙏 Acknowledgements

- **Original gaussian-vrm implementation:** [naruya](https://github.com/naruya)
- **Three.js VRM support:** [pixiv/three-vrm](https://github.com/pixiv/three-vrm)
- **Gaussian Splatting for UE5:** [XVERSE Technology](https://github.com/xverse-engine)
- **VRM for UE5:** [VRM4U](https://github.com/ruyo/VRM4U)
- **Research support:** Ochiai Pavilion at Osaka/Kansai Expo 2025, JSPS KAKENHI

---

## 📞 Support

- **Issues:** [GitHub Issues](https://github.com/hirakawat-hmp/gaussian-vrm-ue5/issues)
- **Discussions:** [GitHub Discussions](https://github.com/hirakawat-hmp/gaussian-vrm-ue5/discussions)
- **Original Project:** [gaussian-vrm.github.io](https://gaussian-vrm.github.io/)

---

**Status:** 🚧 Prototype Phase | **Version:** 0.1.0 | **Last Updated:** November 2025
