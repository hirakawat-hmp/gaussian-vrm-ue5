#!/usr/bin/env python3
# Copyright (c) 2025 naruya
# Licensed under the MIT License. See LICENSE file in the project root for full license information.

"""
GVRM to UE5 Converter

GVRMファイルをUnreal Engine 5用のアセットに変換します。
このツールはオフライン（UE5の外）で実行され、CSVとJSONファイルを生成します。

Usage:
    python gvrm_to_ue5.py <gvrm_file> -o <output_dir>

Example:
    python gvrm_to_ue5.py ../../assets/author.gvrm -o ./output
"""

import zipfile
import json
import csv
import os
import argparse
from pathlib import Path


class GVRMConverter:
    """GVRMファイルをUE5形式に変換するクラス"""

    def __init__(self, gvrm_path, output_dir):
        self.gvrm_path = Path(gvrm_path)
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(parents=True, exist_ok=True)

        if not self.gvrm_path.exists():
            raise FileNotFoundError(f"GVRM file not found: {self.gvrm_path}")

    def convert(self):
        """メイン変換処理"""
        print(f"Converting {self.gvrm_path}...")
        print(f"Output directory: {self.output_dir}")

        with zipfile.ZipFile(self.gvrm_path, 'r') as z:
            # 1. data.jsonを読み込み
            print("\n[1/5] Reading data.json...")
            data = json.loads(z.read('data.json'))
            self._print_data_summary(data)

            # 2. VRMとPLYを抽出
            print("\n[2/5] Extracting VRM and PLY files...")
            z.extract('model.vrm', self.output_dir)
            z.extract('model.ply', self.output_dir)
            print(f"  ✓ Extracted model.vrm")
            print(f"  ✓ Extracted model.ply")

            # 3. バインディングデータをCSV化
            print("\n[3/5] Exporting binding data...")
            self._export_binding_csv(data)

            # 4. メタデータをJSON出力
            print("\n[4/5] Exporting metadata...")
            self._export_metadata(data)

            # 5. UE5インポート用のREADME生成
            print("\n[5/5] Generating import instructions...")
            self._generate_readme()

        print(f"\n✓ Conversion complete!")
        print(f"\nOutput files:")
        print(f"  - {self.output_dir / 'model.vrm'}")
        print(f"  - {self.output_dir / 'model.ply'}")
        print(f"  - {self.output_dir / 'splat_binding.csv'}")
        print(f"  - {self.output_dir / 'metadata.json'}")
        print(f"  - {self.output_dir / 'IMPORT_INSTRUCTIONS.md'}")

    def _print_data_summary(self, data):
        """データの概要を表示"""
        num_splats = len(data.get('splatVertexIndices', []))
        model_scale = data.get('modelScale', 1.0)
        num_bone_ops = len(data.get('boneOperations', []))

        print(f"  Splat count: {num_splats:,}")
        print(f"  Model scale: {model_scale}")
        print(f"  Bone operations: {num_bone_ops}")

    def _export_binding_csv(self, data):
        """スプラットバインディング情報をCSV出力"""
        csv_path = self.output_dir / "splat_binding.csv"

        vertex_indices = data['splatVertexIndices']
        bone_indices = data.get('splatBoneIndices', [])
        relative_poses = data['splatRelativePoses']

        num_splats = len(vertex_indices)

        with open(csv_path, 'w', newline='') as f:
            writer = csv.writer(f)

            # ヘッダー
            writer.writerow([
                'SplatIndex',
                'VertexIndex',
                'BoneIndex',
                'RelativePosX',
                'RelativePosY',
                'RelativePosZ'
            ])

            # データ行
            for i in range(num_splats):
                vertex_idx = vertex_indices[i]
                bone_idx = bone_indices[i] if i < len(bone_indices) else -1
                rel_pos = relative_poses[i * 3:(i + 1) * 3]  # [x, y, z]

                writer.writerow([
                    i,
                    vertex_idx,
                    bone_idx,
                    rel_pos[0],  # x
                    rel_pos[1],  # y
                    rel_pos[2]   # z
                ])

        print(f"  ✓ Exported {num_splats:,} splat bindings to splat_binding.csv")

    def _export_metadata(self, data):
        """メタデータをJSON出力"""
        metadata = {
            'modelScale': data.get('modelScale', 1.0),
            'boneOperations': data.get('boneOperations', []),
            'splatCount': len(data['splatVertexIndices']),
            'version': '1.0',
            'source': str(self.gvrm_path.name)
        }

        json_path = self.output_dir / "metadata.json"
        with open(json_path, 'w') as f:
            json.dump(metadata, f, indent=2)

        print(f"  ✓ Exported metadata to metadata.json")

    def _generate_readme(self):
        """UE5インポート手順を生成"""
        readme_content = """# UE5 Import Instructions

## Overview
This directory contains converted GVRM data for Unreal Engine 5.

## Files
- `model.vrm` - VRM character model (requires VRM4U plugin)
- `model.ply` - Gaussian splatting point cloud (requires XVERSE plugin)
- `splat_binding.csv` - Splat-to-vertex binding data
- `metadata.json` - Additional metadata (scale, bone operations)

## Import Steps

### 1. Install Required Plugins
- VRM4U: https://github.com/ruyo/VRM4U
- XVERSE XScene-UEPlugin: https://github.com/xverse-engine/XScene-UEPlugin

### 2. Import VRM Model
1. Open Unreal Editor
2. Import `model.vrm` using VRM4U
3. Save to `/Game/GVRM/Characters/`

### 3. Import PLY File
1. Import `model.ply` using XVERSE plugin
2. Save to `/Game/GVRM/Splats/`

### 4. Create Data Table
1. Right-click in Content Browser → Miscellaneous → Data Table
2. Choose appropriate struct (create custom struct if needed)
3. Import `splat_binding.csv`
4. Save to `/Game/GVRM/Data/`

### 5. Create GVRM Binding Asset
1. Create a Data Asset of type `UGVRMBindingData`
2. Reference the Data Table created in step 4
3. Set metadata from `metadata.json`
4. Save to `/Game/GVRM/Data/`

### 6. Setup GVRM Actor
1. Create a Blueprint based on `AGVRMActor`
2. Assign the VRM skeletal mesh
3. Assign the GVRM binding data
4. Setup Niagara system

## Next Steps
See the main GVRM documentation for runtime setup and Niagara configuration.
"""

        readme_path = self.output_dir / "IMPORT_INSTRUCTIONS.md"
        with open(readme_path, 'w') as f:
            f.write(readme_content)

        print(f"  ✓ Generated IMPORT_INSTRUCTIONS.md")


def main():
    parser = argparse.ArgumentParser(
        description='Convert GVRM files to UE5 assets',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Convert a single file
  python gvrm_to_ue5.py ../../assets/author.gvrm -o ./output

  # Convert with custom output directory
  python gvrm_to_ue5.py path/to/avatar.gvrm -o /path/to/ue5/project/Import
        """
    )

    parser.add_argument('gvrm_file', help='Path to .gvrm file')
    parser.add_argument('-o', '--output', required=True, help='Output directory')

    args = parser.parse_args()

    try:
        converter = GVRMConverter(args.gvrm_file, args.output)
        converter.convert()
        return 0
    except Exception as e:
        print(f"\n❌ Error: {e}")
        import traceback
        traceback.print_exc()
        return 1


if __name__ == "__main__":
    exit(main())
