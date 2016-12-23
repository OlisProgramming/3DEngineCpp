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

## TEMAP syntax

You should not need to worry about TEMAP syntax unless you wish to help with the development of this project, so if you just want to use the MapCompiler you can skip over this section.

Each line of a TEMAP file contains a command for the interpreter. It then passes a series of whitespace-separated arguments to the command if necessary. There will be no 'default' arguments, because it needs to be as fast as possible (and so every argument 'slot' must be filled). When you write a MapCompiler XML file, you are permitted to omit many arguments (the ones marked with `o` above). These are then automatically filled when the TEMAP file is written to.

### List of commands

The command's arguments are denoted by the smaller bullet points after the command. The arguments must be written in the exact order shown here, otherwise the map will either not look as desired, or will crash.

* `E` - defines an entity. All commands after this point must be components of the entity.
  - `posX posY posZ` - position of the entity.
  - `rotX rotY rotZ rotAng` - rotation of the entity.
  - `scl` - scale of the entity.
* `e` - finalises an entity and adds it to the scene. This command must be run after an `E` command.
* `M` - defines a material.
  - `name` - name of the material.
  - `diffuse normal displacement` - the textures used for diffuse, normal and displacement maps.
  - `specIntensity specPower dispScale dispOffset` - parameters for the specular and displacement attributes of the material.
* `MR` - defines a `MeshRenderer` component.
  - `meshStr` - the name of the Wavefront (OBJ) file containing the mesh.
  - `materialStr` - the name of the material to use when rendering the mesh, as defined above in `M::name`.
* `C` - defines a camera.
  - `fov near far` - the Field Of View and the near and far Z planes.
* `FL` - defines a `FreeLook` camera component.
* `FM` - defines a `FreeMove` camera component.

## Example

The following is an example of a MapCompiler XML file.

```xml
<Map name="Test Map">
  
  <!-- Materials must be created before the meshes they are used in! -->
  <Material name="default"
            specular="0.3"/>
    
  <Entity pos="0 -1 5" scl="32">
    <Mesh name="terrain02.obj" mat="default" />
  </Entity>

  <Entity>
    <Mesh name="monkey3.obj" mat="default" />
  </Entity>

  <Entity rot="1 0 0 -45">
    <DirectionalLight />
  </Entity>

  <Entity pos="0 2 0" rot="0 1 0 23">
    <FreeCamera />
  </Entity>

</Map>
```

This XML file would compile to this TEMAP file.

```
M default defaultTexture.png default_normal.jpg default_disp.png 0.3 1 0 0
E 0 -1 5 1 0 0 0 32
MR terrain02.obj default
e
E 0 0 0 1 0 0 0 1
MR monkey3.obj default
e
E 0 0 0 1 0 0 -45 1
LD 1 1 1 0.4 10 80 1
e
E 0 2 0 0 1 0 23 1
C 70 0.1 1000
FL
FM
e
```
