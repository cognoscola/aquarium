# Sea

Take a look at the scene here:
https://www.instagram.com/p/_7-SQGNgRh/

How to install
- copy the entire project to any directory\n
- Go to install directory in a terminal
- type without quotes: "cmake ." to generate a makefile
- type without quotes: "make" to generate an executable
- run executable.

Controls
- a,s,d,w  - move camera
- mouse    - orient camera
- b        - take a snapshop (will freeze for a few seconds)
- spacebar - shatter the screen
- p        - record 10 seconds worth of frames


These are the steps I did to make the scene, feel free to replicate yourself.

1. Skybox

The skybox is very simple. We simply create a texture object but with the GL_TEXTURE_CUBE_MAP property and assign it
data from our 6 image files. Align the cube such that the camera is in the center of it. I temporarily set
translate portion of the viewMatrix to 0 before passing it to the shader. This keeps the camera at the center of it.

2.  Water.

Get the source code for rendering water found here (https://github.com/cognoscola/water_reflection).
Render a quad with normal (0, 1, 0 ) to be the water. Make it large. I actually made four quads and
put them together to make a bigger quad. If you do choose to put multiple quads together you'll need to
perform some additional steps. First make sure the quads are the same size, height and that the corners
are aligned, otherwise you will see artifacts (seams) along the edges of the smaller quads. This happens because
the edges of the normal map and the distortion map images (which are made to be tiled and are used to give light
effects to the water) become unaligned. In the shader code, if you are tiling the maps, be sure to tile in multiples
of 4 if you have 4 quads. I like to set the value to 8 because it makes the water more turbulent and hence more
realistic.

Next we need to remove the outer edge of the water quad. We do this by enabling alpha blend and making it so that
points in the water that are further away from the camera become more transparent. To do this, you pass the camera's
coordinates to the shader, and inside the vertex shader calculate the alpha value based on the distance between  a
point on the water quad and the camera using a fog formula (https://msdn.microsoft.com/en-us/library/windows/desktop/bb173401(v=vs.85).aspx).
This gradual fade out makes the quad edge disappear and also blends the horizon well with the water.

Next I rendered the underwater portion of the scene. To do this, keep track of the water's height as well
as the camera's height. If the camera's height is below the water's height, rotate the water quad by 180 around the
X or Z axis. We also need to change the clipping planes such that everything above the water is clipped when rendering
the reflection texture and everything below the water is clipped when rendering the refraction texture.

3. Underwater environment (space between the sea floor and the water surface)

The under water environment consists of 2 parts: The Fog and the light rays that penetrate the water.
The fog is created with a very similar effect that that was used to create the gradual water edge fade effect.
Apply an exponential fog formula to all objects underwater and instead of changing the alpha value of the fragment
shader's output colour, we mix the output colour with our desired colour, in this case light-blue. Change the density
and gradient values of the formula so that the the skybox is completely blue but the ground stays visible for some
distance. This fog should only be applied while the camera's height is less than the water's height. To do this
we set the density and gradient values to 0 if this condition is false.

The light rays are generated using a modified version of this shader code (https://www.shadertoy.com/view/lljGDt).
The original version has a dark background and is more circular shape. In the modified version have edges fade out,
the light looks more rectangular in shape, the quads have transparency and fade in and out completely. I place many
of these quads randomly throughout the map. The quads are given a life span where they are completely transparent
when they are born and when they die, but are translucent in between. They are assigned an age randomly when the scene begins
and they change locations whenever they die to reappear at a new location. Lastly, I added a semi-billboard effect to the
quads (they constantly face the camera) allowing only yaw movements (rotate around y axis only).

4. The sea floor

This is very straight forward method. First load into memory a static .obj file which will act as a terrain. Also
load its original texture and then load caustic textures found on this website (https://www.opengl.org/archives/resources/code/samples/mjktips/caustics/)
using the loading code also found on the same website. When rendering the mesh,  pass the shader code
only two textures at any one time. The first is always the static default texture for the terrain, while the 2nd texture
will be 1 of the 32 caustic texture objects. I simply loop through caustic textures, incrementing the
texture index every few milliseconds. In the shader code, we simply mix the original texture with the caustic texture.
Be sure to tile the caustic textures to a size that resembles that of the surface of the water. Lastly, we apply fog
just like the skybox.
