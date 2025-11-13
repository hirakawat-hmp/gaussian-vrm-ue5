# GVRM Runtime - Windows Build Guide

**Target Platform:** Windows 10/11
**UE Version:** 5.3, 5.4, 5.6
**Build Tool:** Visual Studio 2022

---

## Prerequisites

### Required Software

1. **Unreal Engine 5.3+**
   - Download from Epic Games Launcher
   - Or install via GitHub source (advanced)

2. **Visual Studio 2022**
   - Community Edition (free) or Professional
   - Required workloads:
     - Desktop development with C++
     - Game development with C++
   - Download: https://visualstudio.microsoft.com/downloads/

3. **Git**
   - For cloning the repository
   - Download: https://git-scm.com/

### Required UE5 Plugins

1. **VRM4U** - VRM model support
   - GitHub: https://github.com/ruyo/VRM4U
   - Install to: `<YourProject>/Plugins/VRM4U`

2. **XVERSE XScene-UEPlugin** - Gaussian Splatting rendering
   - GitHub: https://github.com/xverse-engine/XScene-UEPlugin
   - Install to: `<YourProject>/Plugins/XV3DGS`

---

## Step-by-Step Build Instructions

### 1. Clone the Repository

```powershell
# Clone the repo
git clone https://github.com/your-username/gaussian-vrm-ue5.git
cd gaussian-vrm-ue5
```

### 2. Create UE5 Project

**Option A: New Project**

1. Open Epic Games Launcher
2. Create new project: **Third Person (C++)**
3. Project name: `GVRMTestProject`
4. Location: Any convenient location
5. Click **Create**

**Option B: Existing Project**

Use your existing UE5 C++ project.

### 3. Install Required Plugins

```powershell
# Navigate to your project's Plugins folder
cd C:\Path\To\GVRMTestProject\Plugins

# Clone VRM4U
git clone https://github.com/ruyo/VRM4U.git

# Clone XVERSE plugin
git clone https://github.com/xverse-engine/XScene-UEPlugin.git
# Copy only the plugin subfolder
xcopy /E /I "XScene-UEPlugin\UEPlugin\Plugin\XV3DGS" "XV3DGS\"
```

### 4. Install GVRM Runtime Plugin

**Method 1: Symlink (Recommended for Development)**

```powershell
# From Plugins folder
mklink /D GVRMRuntime "C:\Path\To\gaussian-vrm-ue5\ue5\Plugins\GVRMRuntime"
```

**Method 2: Copy**

```powershell
# Copy entire plugin folder
xcopy /E /I "C:\Path\To\gaussian-vrm-ue5\ue5\Plugins\GVRMRuntime" "GVRMRuntime\"
```

Your Plugins folder should now look like:
```
Plugins/
├── GVRMRuntime/     ← GVRM plugin
├── VRM4U/           ← VRM support
└── XV3DGS/          ← Gaussian Splatting
```

### 5. Generate Project Files

1. **Close Unreal Editor** if open
2. Right-click on `GVRMTestProject.uproject`
3. Select **"Generate Visual Studio project files"**
4. Wait for completion

### 6. Open in Visual Studio

1. Open `GVRMTestProject.sln` with Visual Studio 2022
2. Set build configuration:
   - **Configuration:** `Development Editor`
   - **Platform:** `Win64`

### 7. Build the Project

**First Build:**
```
Build → Build Solution (Ctrl+Shift+B)
```

**Expected build time:**
- First build: 5-15 minutes (depending on hardware)
- Incremental builds: 30 seconds - 2 minutes

**Common Build Errors & Fixes:**

| Error | Solution |
|-------|----------|
| `Cannot open include file: 'NiagaraDataInterface.h'` | Enable Niagara plugin in `.uproject` |
| `LNK2019: unresolved external symbol` | Rebuild entire solution |
| `MSB3073: exited with code 6` | Close UE Editor, rebuild |
| Missing `.gen.cpp` files | Re-generate project files |

### 8. Launch Unreal Editor

**Method 1: From Visual Studio**
```
Debug → Start Without Debugging (Ctrl+F5)
```

**Method 2: From Launcher**
```
Open Epic Games Launcher → Library → Launch
```

### 9. Verify Plugin Installation

In Unreal Editor:

1. **Edit → Plugins**
2. Search for **"GVRM"**
3. Verify **"GVRM Runtime"** is **enabled** ✅
4. Restart editor if prompted

### 10. Convert GVRM File

**Using Python Tool:**

```powershell
# Navigate to tools folder
cd gaussian-vrm-ue5\ue5\Tools

# Convert sample GVRM
python gvrm_to_ue5.py ..\..\assets\author.gvrm -o C:\Temp\GVRM_Output

# Verify output
dir C:\Temp\GVRM_Output
```

**Expected output:**
```
model.vrm              (VRM character)
model.ply              (Gaussian splats)
splat_binding.csv      (76,337 bindings)
metadata.json          (scale, bone ops)
IMPORT_INSTRUCTIONS.md (import guide)
```

### 11. Import Assets to UE5

**A. Import VRM Model:**

1. Content Browser → Right-click → **Import**
2. Select `model.vrm`
3. VRM4U import dialog appears
4. Click **Import**
5. Save to: `/Game/GVRM/Characters/`

**B. Import PLY (Gaussian Splats):**

1. Use XVERSE plugin import
2. Select `model.ply`
3. Save to: `/Game/GVRM/Splats/`

**C. Create Binding Data Asset:**

1. Content Browser → Right-click → **Miscellaneous → Data Asset**
2. Choose **`GVRMBindingData`**
3. Name it: `DA_AuthorGVRM`
4. Open the asset
5. **Manual data entry** (temporary):
   - For now, bindings must be entered manually or via custom import tool
   - Future: CSV auto-import tool

### 12. Create GVRM Actor

**Blueprint Method:**

1. Content Browser → Right-click → **Blueprint Class**
2. Parent Class: Search **"GVRMActor"**
3. Name: `BP_GVRMActor_Author`
4. Open Blueprint
5. **Details Panel:**
   - **VRM Skeletal Mesh:** Assign imported VRM
   - **Binding Data:** Assign `DA_AuthorGVRM`
   - **Splat Niagara System Asset:** Assign Niagara system (see Step 13)

### 13. Create Niagara System

See: **[NIAGARA_SETUP_GUIDE.md](./NIAGARA_SETUP_GUIDE.md)** (to be created)

Minimal setup:

1. Content Browser → **Niagara System**
2. Create **Empty System**
3. Add **GVRM Splat Emitter**
4. Configure Data Interface
5. Save as `NS_GVRM_Splats`

### 14. Test in Level

1. Drag `BP_GVRMActor_Author` into level
2. Press **Play**
3. You should see:
   - ✅ VRM character model
   - ✅ Gaussian splats rendering
   - ✅ Splats following character animation

---

## Performance Testing

### Expected Performance

| Hardware | FPS (76K splats) |
|----------|------------------|
| RTX 4090 | 120+ FPS |
| RTX 4070 | 90-100 FPS |
| RTX 3080 | 70-80 FPS |
| RTX 3060 | 50-60 FPS |

### Console Commands

```cpp
// Show debug info
GVRM.ShowDebug 1

// Force LOD level
GVRM.LODLevel 0

// Run benchmark
GVRM.Benchmark 300
```

### Stat Commands

```cpp
stat FPS
stat Unit
stat Niagara
stat GPU
```

---

## Troubleshooting

### Build Errors

**Error: "Plugin 'GVRMRuntime' failed to load"**

```powershell
# Solution 1: Rebuild from scratch
1. Close UE Editor
2. Delete: Binaries/, Intermediate/, Saved/
3. Right-click .uproject → Generate VS files
4. Rebuild in Visual Studio

# Solution 2: Check Build.cs dependencies
# Verify GVRMRuntime.Build.cs has:
# - "Niagara", "NiagaraCore", "NiagaraShader"
```

**Error: "GetSkinnedVertexPosition() not found"**

```cpp
// Alternative method (GVRMActor.cpp):
// Replace GetSkinnedVertexPosition() with:
FVector VertexPos = SkelComp->GetSocketLocation(FName(*FString::Printf(TEXT("Vertex_%d"), VertexIdx)));
```

### Runtime Errors

**Splats not appearing:**

1. Check Niagara system is **Active**
2. Verify **GVRM_NDI** is bound in Niagara
3. Check `bAutoActivateSplats = true`

**Splats in wrong position:**

1. Verify **Model Scale** matches metadata
2. Check **Bone Operations** applied correctly
3. Test with `bApplyBoneOperations = false`

**Low FPS:**

1. Reduce splat count (use LOD)
2. Check GPU usage (`stat GPU`)
3. Disable debug visualization

---

## Next Steps

1. ✅ Build successful
2. ✅ Assets imported
3. ⏳ **Create Niagara System** (see NIAGARA_SETUP_GUIDE.md)
4. ⏳ **Optimize performance** (LOD, culling)
5. ⏳ **Add custom animations**

---

## Additional Resources

- **UE5 Documentation:** https://docs.unrealengine.com/5.4/
- **Niagara Tutorial:** https://docs.unrealengine.com/5.4/niagara-overview/
- **VRM4U Wiki:** https://github.com/ruyo/VRM4U/wiki
- **XVERSE Docs:** https://github.com/xverse-engine/XScene-UEPlugin

---

## Support

- **Issues:** https://github.com/your-username/gaussian-vrm-ue5/issues
- **Discussions:** https://github.com/your-username/gaussian-vrm-ue5/discussions
- **Original GVRM:** https://gaussian-vrm.github.io/

---

**Build Date:** 2025-11-14
**Plugin Version:** 0.1.0
**UE Compatibility:** 5.3, 5.4, 5.6
