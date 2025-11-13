# GVRM Runtime - Niagara System Setup Guide

**Plugin:** GVRM Runtime
**UE Version:** 5.3+
**Purpose:** Configure Niagara to render animated Gaussian Splats

---

## Overview

The GVRM Niagara system renders Gaussian splats as particles, updating their positions every frame based on VRM skeletal animation. This guide walks through creating and configuring the Niagara system from scratch.

---

## Prerequisites

- ✅ GVRM Runtime plugin installed and compiled
- ✅ VRM model imported
- ✅ GVRM binding data created
- ✅ Basic understanding of Niagara

---

## Step 1: Create Niagara System

### 1.1 Create Empty System

1. **Content Browser** → Right-click → **FX → Niagara System**
2. Select **"New system from selected emitters"**
3. Click **"+"** → **"New emitter from template"**
4. Choose **"Empty"** template
5. Name: `NS_GVRM_Splats`

### 1.2 Initial Configuration

In the **Niagara System** editor:

**System Properties:**
- Warmup Time: `0`
- Fixed Bounds: `Enabled` (important for performance)
- Fixed Bounds Extent: `500, 500, 500` (adjust based on character size)

---

## Step 2: Configure Emitter

### 2.1 Emitter Properties

Select the emitter in the **System Overview**:

**Emitter Properties:**
- Sim Target: `GPUCompute Sim` ⚠️ **REQUIRED**
- Fixed Bounds: Use system bounds
- Calculate Bounds Mode: `Fixed`

### 2.2 Emitter Spawn

**Module: Spawn Burst Instantaneous**
- Spawn Count: `76337` (or your splat count from metadata.json)
- Spawn Time: `0.0`

**Settings:**
- Execution: `Emitter Spawn`
- Enabled: ✅

### 2.3 Particle Spawn

Add the following modules in **Particle Spawn** group:

#### Module: Initialize Particle

**Settings:**
- Lifetime Mode: `Random`
- Lifetime Min: `99999`
- Lifetime Max: `99999`

#### Module: Set Particles.SplatIndex

1. Click **"+"** → **"Set new or existing parameter directly"**
2. Parameter: Create new `Particles.SplatIndex` (int32)
3. Set Mode: `Index`

**Script:**
```hlsl
// This sets each particle's SplatIndex to its spawn index (0, 1, 2, ...)
Particles.SplatIndex = ExecIndex;
```

#### Module: Set Particles.VertexIndex (from Binding Data)

**Option A: Via Data Table (recommended)**

1. Import `splat_binding.csv` as Data Table
2. Create struct: `FSplatBindingRow`
   - `SplatIndex` (int32)
   - `VertexIndex` (int32)
   - `BoneIndex` (int32)
   - `RelativePosX` (float)
   - `RelativePosY` (float)
   - `RelativePosZ` (float)

3. Add module: **"Get Data Table Row"**
   - Data Table: `DT_SplatBindings`
   - Row Index: `Particles.SplatIndex`
   - Output: `Particles.VertexIndex`

**Option B: Via Custom HLSL (for testing)**

```hlsl
// Temporary: All splats use vertex 0
Particles.VertexIndex = 0;
Particles.RelativePosition = float3(0, 0, 0);
```

---

## Step 3: Add GVRM Data Interface

### 3.1 Add User Parameter

1. **User Parameters** panel (left side)
2. Click **"+"** → **"Make new"**
3. Type: **"Data Interface"**
4. Class: Search **"Niagara Data Interface GVRM"**
5. Name: `GVRM_NDI`

### 3.2 Configure NDI Properties

Select `GVRM_NDI` in User Parameters:

**Properties:**
- **Skeletal Mesh Component:** (set at runtime via Blueprint)
- **Max Bone Influences:** `4`

---

## Step 4: Particle Update - GVRM Skinning

### 4.1 Add Custom Module

1. **Particle Update** group → **"+"** → **"Module Script"**
2. Name: `GVRM Skinning`
3. **Script Type:** `Module`

### 4.2 Write Skinning Script

**Module Input Parameters:**
- `GVRM_NDI` (User.GVRM_NDI)
- `Particles.VertexIndex` (int)
- `Particles.RelativePosition` (vector3)

**Module Output Parameters:**
- `Particles.Position` (vector3)

**Custom HLSL:**

```hlsl
// GVRM Skinning Module
// Computes skinned position using Linear Blend Skinning (LBS)

// Inputs (from particle attributes)
int VertexIndex = Particles.VertexIndex;
float3 RelativePosition = Particles.RelativePosition;

// Get vertex data from NDI
float3 VertexPosition = GVRM_NDI.GetVertexPosition(VertexIndex);
int BoneIndex0, BoneIndex1, BoneIndex2, BoneIndex3;
GVRM_NDI.GetVertexBoneIndices(VertexIndex, BoneIndex0, BoneIndex1, BoneIndex2, BoneIndex3);
float4 BoneWeights = GVRM_NDI.GetVertexBoneWeights(VertexIndex);

// Get bone matrices
float4x4 BoneMat0 = GVRM_NDI.GetBoneTransform(BoneIndex0);
float4x4 BoneMat1 = GVRM_NDI.GetBoneTransform(BoneIndex1);
float4x4 BoneMat2 = GVRM_NDI.GetBoneTransform(BoneIndex2);
float4x4 BoneMat3 = GVRM_NDI.GetBoneTransform(BoneIndex3);

// Linear Blend Skinning (LBS)
float4x4 SkinMatrix =
    BoneWeights.x * BoneMat0 +
    BoneWeights.y * BoneMat1 +
    BoneWeights.z * BoneMat2 +
    BoneWeights.w * BoneMat3;

// Apply skinning to vertex + relative position
float3 LocalPosition = VertexPosition + RelativePosition;
Particles.Position = mul(float4(LocalPosition, 1.0), SkinMatrix).xyz;
```

**Module Settings:**
- Execution: `Particle Update`
- Update Frequency: `Every Frame`

---

## Step 5: Particle Rendering

### 5.1 Add Sprite Renderer

1. **Render** group → **"+"** → **"Sprite Renderer"**

**Sprite Renderer Settings:**
- Material: `M_GaussianSplat` (create this material)
- Sprite Alignment: `Velocity Aligned` or `Camera Facing`
- Sprite Size Mode: `Uniform`
- Sprite Size: `5.0` (adjust based on visual preference)

### 5.2 Create Gaussian Splat Material

**Create Material: M_GaussianSplat**

1. Content Browser → **Material**
2. Name: `M_GaussianSplat`

**Material Settings:**
- Material Domain: `Surface`
- Blend Mode: `Translucent`
- Shading Model: `Unlit`

**Material Graph:**

```
[Particle Color] → Base Color
                 ↓
          [Custom (Gaussian)]
                 ↓
           [Opacity]

Custom Node (Gaussian Alpha):
Code:
  float2 uv = TexCoord[0] * 2.0 - 1.0;
  float dist = dot(uv, uv);
  return exp(-dist * 2.0);

Inputs:
  - TexCoord (float2)

Output:
  - Opacity (float)
```

---

## Step 6: Blueprint Integration

### 6.1 Reference Niagara System in GVRM Actor

**In BP_GVRMActor_Author:**

1. Open Blueprint
2. **Details Panel** → **Splat Niagara System Asset**
3. Select: `NS_GVRM_Splats`

### 6.2 Set NDI Reference at Runtime

**Event Graph:**

```cpp
Event BeginPlay
  ↓
[Get Splat Niagara System]
  ↓
[Set Niagara Variable (Object)]
  - Name: "GVRM_NDI"
  - Value: [Get VRM Skeletal Mesh]
```

**Or use C++ (already implemented in AGVRMActor::SetupNiagaraDataInterface)**

---

## Step 7: Testing

### 7.1 Place Actor in Level

1. Drag `BP_GVRMActor_Author` into level
2. Press **Play** (PIE)

### 7.2 Verify Rendering

**Expected behavior:**
- ✅ Niagara system activates
- ✅ 76,337 particles spawn
- ✅ Particles positioned at VRM mesh vertices
- ✅ Particles follow animation

### 7.3 Debug Visualization

**Console commands:**
```cpp
// Show Niagara stats
stat Niagara

// Show particle count
fx.Niagara.Debug.DrawSystemBounds 1

// Show GPU performance
stat GPU
```

---

## Advanced Configuration

### LOD (Level of Detail)

**Dynamic Splat Count:**

```hlsl
// In Emitter Spawn
float LODDistance = length(Engine.Owner.CameraPosition - System.LocalToWorld.Position);
int SplatCount = lerp(76337, 7633, saturate(LODDistance / 2000.0));
SpawnBurstInstantaneous.SpawnCount = SplatCount;
```

### Performance Optimization

**Culling:**

```hlsl
// In Particle Update (before skinning)
float3 CameraVector = Particles.Position - Engine.Owner.CameraPosition;
float Distance = length(CameraVector);

// Kill particles beyond certain distance
if (Distance > 5000.0)
{
    Particles.Lifetime = 0;
}
```

**Fixed Delta Time:**

In Niagara System properties:
- Use Fixed Delta Time: `Enabled`
- Fixed Delta Time: `0.0333` (30 FPS update)

### Color Modes

**Add color variation:**

```hlsl
// In Particle Spawn
// Debug: Color by bone
float Hue = float(Particles.BoneIndex) / 100.0;
Particles.Color = HSVtoRGB(float3(Hue, 1.0, 1.0));

// Production: Use original splat colors
// (requires importing PLY color data)
```

---

## Troubleshooting

### Particles Not Appearing

**Check:**
1. Spawn count > 0
2. Particle lifetime > 0
3. Renderer enabled
4. Material assigned

**Console:**
```cpp
fx.Niagara.Debug.DrawSystemBounds 1
fx.Niagara.Debug.DrawEmitters 1
```

### Particles in Wrong Position

**Common issues:**
1. NDI not bound → Set in Blueprint Event Graph
2. Wrong coordinate space → Check SkinMatrix multiplication
3. Relative position wrong → Verify CSV data

**Test with simple position:**
```hlsl
// Bypass skinning (test)
Particles.Position = float3(0, 0, 100);
```

### Low FPS

**Optimizations:**
1. Reduce spawn count
2. Enable GPU Compute Sim
3. Use fixed delta time
4. Implement distance culling

**Target Performance:**
- 60 FPS with 50K splats (RTX 3080)
- 30 FPS with 100K splats (RTX 3060)

---

## Example Configurations

### Minimal Test System

**For quick testing:**
- Spawn: 1000 particles
- Update: Simple offset (no skinning)
- Render: Basic sprite

```hlsl
// Minimal particle update
Particles.Position = float3(
    Particles.SplatIndex % 10 * 10,
    (Particles.SplatIndex / 10) % 10 * 10,
    100
);
```

### Production System

**For final rendering:**
- Spawn: Full splat count
- Update: GVRM skinning
- Render: Gaussian material with LOD
- Culling: Distance-based

---

## Next Steps

1. ✅ Niagara system created
2. ✅ GVRM skinning configured
3. ⏳ **Optimize performance** (LOD, culling)
4. ⏳ **Import custom animations**
5. ⏳ **Create material variations**

---

## Resources

- **Niagara Documentation:** https://docs.unrealengine.com/5.4/niagara-overview/
- **Custom HLSL in Niagara:** https://docs.unrealengine.com/5.4/hlsl-in-niagara/
- **GPU Compute Sim:** https://docs.unrealengine.com/5.4/gpu-particles-in-niagara/

---

**Last Updated:** 2025-11-14
**Plugin Version:** 0.1.0
**Tested on:** UE 5.4
