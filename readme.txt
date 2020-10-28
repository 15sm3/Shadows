
Shadowing using a multipass shader. The first pass computes the shadow map. The second pass determines whether a fragment is in shadow and renders it accordingly.

 - In pass 1, the objects are rendered from the point of view of the
   light source and a 'shadowTexture' is used to store the depth (in
   the light's CCS transformed to the range [0,1]) of each fragment.

 - In pass 2, the objects are rendered from the viewpoint.  Each
   fragment is provided with its coordinates in the WCS.  The fragment
   shader transforms those WCS coordinates into the light's CCS using
   a WCS-to-lightCCS transformation (provided).  Then the depth of the
   transformed fragment is compared to the depth in the shadow texture
   corresponding to that point in the light's CCS.

   The fragment is given reduced illumination if it is in shadow.

The shadow depth is stored in a 'shadowTexture' texture.  The texture
has the same dimensions as the window, so there's a one-to-one
correspondence between texels in the texture map and pixels in the
window.


  Compile the code, then run it as

    ./shader data/carpet.obj data/triceratops.obj data/apple.obj


Pass 1

  The pass 1 shaders are used when the scene is rendered from the
  light's viewpoint, using orthographic projection because the light
  is directional.  (A point light source would use perspective
  projection.)

   Edit pass1.vert to compute the vertex's position in the
  light's CCS.

   Then compute the vertex's depth in the light's CCS, and
  convert this to the range [0,1] and pass that value to the fragment
  shader.


  After this step, you should be able to press 'd' (for debug) and see
  the red shadow map.  Press 'd' again to see the normal view.

Pass 2 vertex shader

  The pass 2 shaders are used when the scene is rendered from the
  eye's viewpoint, using perspective projection.

  Edit pass2.vert:

     Compute the vertex's position in the eye's CCS.

     Set the vertex colour based on the object's diffuse
    illumination coefficient.  You'll have to see how this is done in
    'setMaterial' in wavefront.cpp and use the 'uniform' value that is
    defined there.

     Compute the vertex's normal in the WCS.

     Compute the vertex's position in the WCS.

Pass 2 fragment shader

     Compute the fragment position in the light's CCS.

     Using the above result, compute the fragment's depth
    (in [0,1]).

     Compute the fragment's xy position in the light's CCS
    (in [0,1]x[0,1]) and use this to look up the depth from the shadow
    buffer.

     If the fragment is in shadow, attenuate its intensity.
    Reduce the streaky artifacts if any appear.

     Use the texture colour (if texturing) or the colour from
    the vertex shader (if not texturing).

     Apply Phong illumination


