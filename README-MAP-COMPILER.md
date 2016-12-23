# 3DEngineCpp MapCompiler Documentation

The map compiler converts a `.xml` file into a file type called `.temap` which I designed to be as fast to load as possible,
while maintaining some amount of readability. This is used so that any change in the map does not necessitate a recompile of the engine
because maps are loaded at runtime.

The compiled `.temap` should reside in your `/res/maps/` folder. To load a map into your game, simply call
`LoadMap(const std::string& name, const Window& window)` in your `Game::Init` function. No other code is then required in `Init` unless
you need non-map-specific code.

## Running MapCompiler

First, compile the file `3DEngineCpp/MapCompiler/main.cpp` with your choice of C++ compiler, and run it with two commandline parameters,
the name of the directory containing `/res/` and the name (without a file extension) of your XML file.

## XML syntax

I assume if you are using this that you are already familiar with the XML file type, so I'll get to the MapCompiler-specific syntax.

### Tags and Attributes

In this segment I will abbreviate what the compiler expects when parsing eeach attribute, for example `1s` is one string value,
and `3n` is three numeric values. `c` is short for compulsory, `o` is short for optional.

* `Map` - the root tag of the XML file. Valid subtags: `Material`, `Entity`.
  - `name 1s c` - the name of the map. Does not do anything yet, apart from throw up a nice message during compilation.
 
* `Material` - belongs in `Map` - defines a material to use in rendering meshes later. Materials MUST be defined BEFORE the mesh(es) they are used in! No valid subtags.
  - `name 1s c` - the name of the material. This is the name used later in the file when specifying which material do use in a mesh.
  - `diffuse 1s o` - the file name of the texture to use for diffuse shading on the material. Defaults to `defaultTexture.png`.
  - `normal 1s o` - the file name of the texture to use for normal maps on the material. Defaults to `default_normal.jpg`.
  - `displacement 1s o` - the file name of the texture to use for displacement maps on the material. Defaults to `default_disp.png`.
  - `specular 1n o` - the specular intensity. Defaults to 0.
  - `specularPower 1n o` - the specular power. Defaults to 1.
  - `displacementScale 1n o` - the scale of the displacement map. Defaults to 0.
  - `displacementOffset 1n o` - the offset on the displacement map. Defaults to 0.

* `Entity` - belongs in `Map` - creates an entity in the world. Valid subtags: all Entity Components (every tag defined below this point)
  - `pos 3n o` - the position of the entity. Defaults to the origin.
  - `rot 4n o` - the rotation of the entity. Defaults to a rotation of zero degrees around the X axis
  - `scl 1n o` - the scale of the entity. Defaults to 1.

* `Mesh` - belongs in `Entity` - renders a mesh.
  - `name 1s c` - the name of the Wavefront (OBJ) file containing the mesh.
  - `mat 1s c` - the material to use when rendering the mesh. Defined above with the `Material` tag.

* `DirectionalLight` - belongs in `Entity` - creates a directional light to use in the world (light radiating not from a point, but from everywhere pointing in the direction of the rotation of the containing entity
  - `color/colour 3n o` - (can be spelt both ways for the benefit of US users)
  - `intensity 1n o` - the intensity of the light. Defaults to 0.4.
  - `shadowMapSize 1n o` - the size of the shadow map. Defaults to 10.
  - `shadowArea 1n o` - the area of shadows. Defaults to 80.
  - `shadowSoft 1n o` - the softness of the shadows' borders. Defaults to 1.

* `FreeCamera` - belongs in `Entity` - creates a camera. This camera moves around the scene freely, controlled by the W, A, S and D keys, and the mouse.
  - `fov 1n o` - the Field Of View of the camera. Defaults to 70.
  - `near 1n o` - the near Z plane. Defaults to 0.1 world units.
  - `far 1n o` - the far Z plane. Defaults to 1000 world units.
