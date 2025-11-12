# Suggested Commands

## Development Server

### SSL Certificate Setup (Required Once)
Generate self-signed certificates for HTTPS:
```bash
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes -subj "/CN=localhost"
mkdir -p .server
mv *.pem .server/
```

### Install Dependencies
Express.js is required for the development server:
```bash
npm install -g express
# OR install locally
npm install express
```

### Start Development Server
```bash
node server.js
```
Server runs on: `https://localhost:8080`

### Access Applications
- Main editor: `https://localhost:8080/`
- Simple viewer: `https://localhost:8080/examples/simple-viewer.html`
- Avatar World demo: `https://localhost:8080/apps/avatarworld/`

## URL Parameters

### Main Application (`main.js`)
- `?gs=<path>` - Load PLY file for preprocessing
- `?gvrm=<path>` - Load existing GVRM file
- `?stage=<0|1|2|3>` - Skip preprocessing stages (debugging)
- `?vr` - Enable VR mode
- `?gpu` - Use GPU-accelerated preprocessing
- `?nobg` - Remove background splats
- `?nocheck` - Skip validation checks
- `?saveply` - Save processed PLY alongside GVRM
- `?customtype=<float>` - Custom shader material type
- `?size=<width>,<height>` - Canvas size override

Example:
```
https://localhost:8080/?gs=./path/to/scan.ply&gpu&nobg
```

## Runtime Controls (Keyboard)
- **Space** - Pause/play animation
- **V** - Update PMC (Points/Mesh/Capsules) visualization
- **C** - Cycle splat color modes (empty/assign/original)
- **X** - Toggle VRM mesh visibility
- **G** - Cycle through sample GVRM files
- **A** - Cycle through sample FBX animations
- **P** - Toggle point cloud rendering mode
- **Drag & Drop** - Load FBX animation files

## Git Commands (macOS/Darwin)
Standard git commands work as expected:
```bash
git status
git add <files>
git commit -m "message"
git push
git pull
```

## System Utilities (macOS/Darwin)
- `ls` - List directory contents
- `cd` - Change directory
- `grep` - Search text
- `find` - Find files
- `cat` - Display file contents
- `head` - Show first lines of file
- `tail` - Show last lines of file

## No Build/Test/Lint Commands
This project does NOT have:
- ❌ Build/compilation step (no webpack, vite, etc.)
- ❌ Test suite (no jest, mocha, etc.)
- ❌ Linting (no eslint)
- ❌ Formatting (no prettier)
- ❌ Package manager scripts (no package.json)

## Debugging
- Browser DevTools - Primary debugging method
- Console errors displayed via `error.js`
- Check preprocessing stages with `?stage=` parameter
- Visualize bone assignments with `V` and `C` keys
