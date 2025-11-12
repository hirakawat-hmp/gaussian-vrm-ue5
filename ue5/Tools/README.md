# GVRM UE5 Conversion Tools

Python tools for converting GVRM files to Unreal Engine 5 format.

## Setup

```bash
# Install uv if not already installed
curl -LsSf https://astral.sh/uv/install.sh | sh

# Run the tool (uv handles everything automatically)
uv run gvrm_to_ue5.py ../../assets/author.gvrm -o ./output
```

## Usage

### Quick Start

```bash
# Convert a GVRM file
uv run gvrm_to_ue5.py ../../assets/author.gvrm -o ./output

# Or install as a command
uv pip install -e .
gvrm-to-ue5 ../../assets/author.gvrm -o ./output
```

### Batch Conversion

```bash
# Convert all GVRM files
for file in ../../assets/*.gvrm; do
    uv run gvrm_to_ue5.py "$file" -o "./output/$(basename $file .gvrm)"
done
```

## Output

- `model.vrm` - VRM character model
- `model.ply` - Gaussian splat point cloud
- `splat_binding.csv` - Splat-to-vertex binding data
- `metadata.json` - Additional metadata
- `IMPORT_INSTRUCTIONS.md` - UE5 import guide

## Requirements

- Python 3.8+
- No external dependencies (uses standard library only)

## License

MIT License
