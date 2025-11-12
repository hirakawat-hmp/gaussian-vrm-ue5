# Code Style and Conventions

## File Headers
All source files include MIT License copyright notice:
```javascript
// Copyright (c) 2025 naruya
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
```

## Module System
- **ES6 Modules**: Use `import`/`export` syntax
- **Named Exports**: Prefer named exports for classes and utilities
- **Default Exports**: Not commonly used

## Naming Conventions
- **Classes**: PascalCase (e.g., `GVRM`, `VRMCharacter`, `PLYParser`, `GaussianSplatting`)
- **Variables/Functions**: camelCase (e.g., `currentVrm`, `loadingPromise`, `updateByBones`)
- **Constants**: camelCase (not UPPER_CASE)
- **Private Members**: No special prefix (no underscore convention)

## Formatting
- **Indentation**: 2 spaces
- **Semicolons**: Always used
- **Line Length**: No strict limit, but generally reasonable
- **Quotes**: Single quotes for strings (not strict)

## Code Organization
- **Class-based**: Core functionality organized in ES6 classes
- **Static Methods**: Used for factory/initialization methods
- **Async/Await**: Preferred over raw Promises for asynchronous operations

## Comments
- **Inline Comments**: Used sparingly, mostly for complex algorithms
- **JSDoc**: Not consistently used (no formal documentation comments)
- **Error IDs**: Special comments like `[ErrorID N]` used in preprocessing for debugging

## Error Handling
- **Global Error Handler**: `error.js` provides comprehensive error logging
- **Console Errors**: Intercepted and displayed to user
- **Fetch Errors**: Wrapped to show HTTP errors visually
- **Promise Rejections**: Caught and displayed

## WebGL/Three.js Conventions
- **Scene Graph**: Standard Three.js hierarchy
- **Update Loop**: `update()` methods called in animation loop
- **Material Customization**: Shader injection for custom rendering

## File Organization
- **Modular**: Each major component in separate file
- **Flat Structure**: No deep nesting in core libraries
- **Apps Separation**: Application-specific code in `apps/` directory
