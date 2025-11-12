# Task Completion Checklist

## Before Committing Code

### 1. Code Quality
- [ ] Code follows project style conventions (2-space indent, camelCase, semicolons)
- [ ] Copyright header added to new files
- [ ] ES6 module syntax used correctly (import/export)
- [ ] No console.log statements left in production code (unless intentional)

### 2. Testing
⚠️ **No automated test suite exists**
- [ ] Manually test in browser at `https://localhost:8080`
- [ ] Test with both `?gs=` and `?gvrm=` URL parameters if relevant
- [ ] Check browser console for errors
- [ ] Verify error.js catches and displays errors properly

### 3. Functionality Testing (if applicable)

#### For Preprocessing Changes:
- [ ] Test with sample PLY file
- [ ] Try with `?gpu` and without (CPU mode)
- [ ] Verify pose detection works
- [ ] Check GVRM file creation
- [ ] Test different `?stage=` values if debugging

#### For Runtime Changes:
- [ ] Test animation playback (Space key)
- [ ] Test VRM mesh visibility (X key)
- [ ] Test PMC visualization (V key)
- [ ] Test color cycling (C key)
- [ ] Verify FPS performance

#### For VR Changes:
- [ ] Test with `?vr` parameter
- [ ] Verify VR controllers work

### 4. Cross-browser Testing (if major changes)
- [ ] Chrome/Edge (primary target)
- [ ] Firefox
- [ ] Safari (important for iOS)

### 5. Documentation
- [ ] Update CLAUDE.md if architecture changes
- [ ] Update README.md if user-facing features change
- [ ] Add comments for complex algorithms
- [ ] Document any new URL parameters

### 6. File Organization
- [ ] Files in correct directory (gvrm-format/ vs apps/)
- [ ] No unnecessary files added
- [ ] Assets properly placed in assets/ directory

## Git Workflow

### Before Commit:
```bash
git status                    # Review changed files
git diff                      # Review changes
```

### Committing:
```bash
git add <files>              # Stage specific files
git commit -m "message"      # Commit with descriptive message
```

### After Development Session:
```bash
git push                     # Push to remote (if working with remote)
```

## No Automated Checks
This project does NOT have:
- Pre-commit hooks
- CI/CD pipeline
- Automated linting
- Automated testing
- Build verification

**Manual verification is essential!**

## Common Issues to Check

### Server Issues:
- [ ] SSL certificates exist in `.server/` directory
- [ ] Express.js is installed
- [ ] Port 8080 is not in use

### Loading Issues:
- [ ] File paths are correct (relative to server root)
- [ ] HTTPS is used (required for WebRTC and some features)
- [ ] CORS not blocking resources

### Performance Issues:
- [ ] Large PLY files may need `?gpu` parameter
- [ ] Background removal with `?nobg` if needed
- [ ] Check FPS counter (F key or fps.js)

### Preprocessing Issues:
- [ ] Input PLY must be in A-pose (arms spread, legs straight)
- [ ] VRM model must have standard humanoid bones
- [ ] Check preprocessing error logs if validation fails
