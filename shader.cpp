// CISC/CMPE 454 Assignment 3
//
// Deferred shading for shadows


#include "headers.h"
#include "linalg.h"
#include "wavefront.h"
#include "renderer.h"
#include "shader.h"
#include "font.h"
#include "seq.h"
#include "axes.h"


GLFWwindow* window;
seq<wfModel *> objs;		// the objects
Renderer *renderer;		// class to do multipass rendering
Axes *axes;

float theta = 0;
bool sleeping = false;
float timeOffset = 0;
bool showAxes = true;
bool useLightView = false;
bool transitioningViews = false;
bool renderOrthographic = false;
 
GLuint windowWidth = 1200;
GLuint windowHeight = 900;
float factor = 0;

GLFWcursor* rotationCursor;
GLFWcursor* translationCursor;

TextureMode textureMode = NEAREST;


// Viewpoint movement using the mouse

typedef enum { TRANSLATE, ROTATE } ModeType;
typedef enum { LEFT_BUTTON, MIDDLE_BUTTON, RIGHT_BUTTON, NO_BUTTON } ButtonType;

ModeType mode = ROTATE;		// translate or rotate mode
ButtonType button;		// which button is currently down
vec2 mouse;			// mouse position on button DOWN


// Drawing function

float fovy;
vec3 eyePosition;
vec3 upDir(0,1,0);
vec3 lookAt;
float worldRadius;
vec3 initEyeDir = vec3(0.7,0.3,1).normalize();

vec3 prevEyePosition, prevLookAt, prevUpDir;
vec3 targetEyePosition, targetLookAt, targetUpDir;
float targetFOVY, prevFOVY;

const float initEyeDistance = 5.0;

// Compute the light direction, rotate the the current 'theta'

vec3 rotatedLightDir()

{
  vec3 lightDir = vec3(1,1.5,1).normalize();
  vec4 rotatedLightDir = rotate( theta, vec3(0,1,0) ) * vec4( lightDir, 0 );
  return vec3( rotatedLightDir.x, rotatedLightDir.y, rotatedLightDir.z );
}



void display()

{
  // WCS-to-VCS

  mat4 WCS_to_VCS
    = lookat( eyePosition, lookAt, upDir );

  // WCS-to-CCS

  float n = (eyePosition - lookAt).length() - worldRadius;
  float f = (eyePosition - lookAt).length() + worldRadius;

  mat4 WCS_to_CCS
    = perspective( fovy, windowWidth / (float) windowHeight, n, f )
    * WCS_to_VCS;

  // Light direction is in the WCS, but rotates about y axis

  vec3 lightDir = rotatedLightDir();

  // Draw the objects

  renderer->render( objs, WCS_to_VCS, WCS_to_CCS, lightDir, window, renderOrthographic );

  // Draw the world axes

  if (showAxes && !renderer->debugOn()) {
    mat4 axesTransform = WCS_to_CCS * scale( 10, 10, 10 );
    axes->draw( axesTransform, lightDir );
  }

  // Output status message

#ifdef USE_FREETYPE
  if (!renderer->debugOn()) {
    char buffer[1000];
    renderer->makeStatusMessage( buffer );
    render_text( buffer, 10, 10, window );
  }
#endif
}


// Handle windox size changes

void windowSizeCallback( GLFWwindow* window, int width, int height )

{
  windowWidth = width;
  windowHeight = height;
  glViewport(0, 0, width, height);
  renderer->reshape( width, height );
}



// Update the viewpoint angle upon idle

void updateState( float elapsedSeconds )

{
  // Set angle based on elapsed time

  if (!sleeping)
    theta += elapsedSeconds * 0.3;

  // Transition toward targetEyePosition, targetLookAt, targetUpDir

  if (transitioningViews) {

    const float totalTransitionTime = 1.0; // in seconds

    vec3 prevViewDir   = (prevEyePosition - prevLookAt).normalize();
    vec3 targetViewDir = (targetEyePosition - targetLookAt).normalize();
    
    float angleView    = acos( prevViewDir * targetViewDir ); // angle between viewDir and targetViewDir
    float angleUp      = acos( prevUpDir * targetUpDir ); // angle between upDir and targetUpDir

    float viewDirRate  = angleView / totalTransitionTime;
    float upDirRate    = angleUp / totalTransitionTime;
    float lookAtRate   = (prevLookAt - targetLookAt).length() / totalTransitionTime;
    float eyeDistanceRate = ((targetEyePosition-targetLookAt).length() - (prevEyePosition-prevLookAt).length()) / totalTransitionTime;
    float fovyRate     = (targetFOVY - prevFOVY) / totalTransitionTime;

    vec3 viewDir       = (eyePosition - lookAt).normalize(); // we'll rotate the view direction

    float angleRemaining = acos( viewDir * targetViewDir );

    if (fabs(angleRemaining) < viewDirRate*elapsedSeconds) { // now within one time step of target
      
      eyePosition = targetEyePosition;
      upDir   = targetUpDir;
      lookAt  = targetLookAt;

      transitioningViews = false;

    } else { // still transitioning views

      vec3 axis;
      float eyeDistance = (eyePosition - lookAt).length();

      // Rotate the upDir a bit

      axis = (upDir ^ targetUpDir).normalize();
      upDir = (rotate( upDirRate*elapsedSeconds, axis ) * vec4( upDir )).toVec3(); // rotate viewDir a bit

      // Move the lookAt a bit

      lookAt = lookAt + (lookAtRate*elapsedSeconds) * (targetLookAt - prevLookAt).normalize() ;

      // Rotate the viewDir a bit

      axis = (viewDir ^ targetViewDir).normalize(); // rotation axis from viewDir to targetViewDir
      viewDir = (rotate( viewDirRate*elapsedSeconds, axis ) * vec4( viewDir )).toVec3(); // rotate viewDir a bit

      eyeDistance = eyeDistance + (eyeDistanceRate*elapsedSeconds);
      eyePosition = lookAt + eyeDistance*viewDir;

      // Update the FOVY

      fovy = fovy + (fovyRate*elapsedSeconds);
    }

  }
}


// GLFW Error callback

void GLFWErrorCallback( int error, const char* description )

{
  cerr << "Error " << error << ": " << description << endl;
  exit(1);
}



void toggleSleep()

{
  static struct timeb startTime;

  sleeping = !sleeping;
  struct timeb thisTime;
  ftime( &thisTime );
  if (sleeping) {
    startTime.time = thisTime.time;
    startTime.millitm = thisTime.millitm;
  } else
    timeOffset += (thisTime.time - startTime.time) + (thisTime.millitm - startTime.millitm) / 1000.0;
}


// Handle a key press


void keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
  
{
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch (key) {

    case GLFW_KEY_ESCAPE:
      exit(0);

    case 'P':
      toggleSleep();
      break;

    case 'O':
      renderOrthographic = !renderOrthographic;
      break;

    case 'D':
      renderer->incDebug();
      break;

    case 'A':
      showAxes = !showAxes;
      break;

    case 'L':
      useLightView = !useLightView;
      transitioningViews = true;

      if (!sleeping)
	toggleSleep();	  // stop any light movement

      if (useLightView) {
	targetEyePosition = vec3(0,0,0) + 5*worldRadius * rotatedLightDir(); // settings in Renderer::render()
	targetLookAt      = vec3(0,0,0);
	targetUpDir       = vec3(0,1,0);
	targetFOVY        = 2 * atan2( 1, initEyeDistance );
      } else {
	targetEyePosition = prevEyePosition;
	targetLookAt      = prevLookAt;
	targetUpDir       = prevUpDir;
	targetFOVY        = prevFOVY;
      }

      prevEyePosition   = eyePosition; // store the original view to go back to, later
      prevLookAt        = lookAt;
      prevUpDir         = upDir;
      prevFOVY          = fovy;

      renderOrthographic = false;
      break;

    case ' ':
      if (mode == ROTATE)
	mode = TRANSLATE;
      else
	mode = ROTATE;
      glfwSetCursor( window, (mode == TRANSLATE ? translationCursor : rotationCursor) );
      break;

    case 'F':
      if (mods & GLFW_MOD_SHIFT) {
	factor += 0.01;
	cout << "factor = " << factor << endl;
      } else {
	factor -= 0.01;
	cout << "factor = " << factor << endl;
      }
      break;

    case GLFW_KEY_UP:
      eyePosition = (1.0/1.1) * eyePosition;
      break;
      
    case GLFW_KEY_DOWN:
      eyePosition = 1.1 * eyePosition;
      break;
      
    case GLFW_KEY_LEFT:
      fovy *= 1.1;
      break;
      
    case GLFW_KEY_RIGHT:
      fovy /= 1.1;
      break;
    }
  }
}




// Find 2d mouse position on 3D arcball

vec3 arcballPos( vec2 pos )

{
  vec3 p(  pos.x/(float)windowWidth * 2 - 1, -(pos.y/(float)windowHeight * 2 - 1), 0 );
  float rr = p * p;
  if (rr <= 1)			// inside arcball
    p.z = sqrt( 1 - rr );
  else
    p = p.normalize();
  return p;
}


mat4 arcballTransform( vec2 pos, vec2 prevPos )

{
  vec3 p1 = arcballPos( pos );
  vec3 p0 = arcballPos( prevPos );

  float dot = p0 * p1;
  if (dot > 1) dot=1;
  float angle = acos( dot );

  vec3 axis = p0 ^ p1;

  return rotate( -1 * angle, axis );
}


// Mouse motion callback
//
// Only enabled when mouse button is down (which is done in mouseButtonCallback())

void mousePositionCallback( GLFWwindow* window, double x, double y )

{
  vec3 xdir, ydir, zdir;

  ydir = upDir.normalize();
  zdir = (eyePosition - lookAt).normalize();
  xdir = (ydir ^ zdir).normalize();

  if (mode == TRANSLATE) {

    if (glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS) { // pan in x and y

      lookAt = lookAt + fovy * ((mouse.x-x) * xdir + (y-mouse.y) * ydir);
      eyePosition = eyePosition + fovy * ((mouse.x-x) * xdir + (y-mouse.y) * ydir);

    } else if (button == RIGHT_BUTTON) { // move in z

      lookAt = lookAt + (mouse.y-y)*0.2 * zdir;
      eyePosition = eyePosition + (mouse.y-y)*0.2 * zdir;
    }

  } else { // mode == ROTATE

    if (glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT ) == GLFW_PRESS) { // zoom

      fovy *= 1.0 + 0.001*(mouse.y-y);
      if (fovy > 135)
	fovy = 135;
      if (fovy < 0.001)
	fovy = 0.001;

    } else if (glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS) { // rotate

      mat4 WCS_to_VCS = lookat( eyePosition, lookAt, upDir );
      mat4 VCS_to_WCS = WCS_to_VCS.inverse();
      mat4 T = VCS_to_WCS * arcballTransform( vec2(x,y), mouse ) * WCS_to_VCS;

      upDir = (T * vec4( upDir, 0 )).toVec3();
      vec3 eyeDir = eyePosition - lookAt;
      eyePosition = (T * vec4( eyeDir, 0)).toVec3() + lookAt;
    }
  }

  mouse.x = x;
  mouse.y = y;
}


// Mouse button callback

void mouseButtonCallback( GLFWwindow* window, int button, int action, int mods )

{
  if (action == GLFW_PRESS) {

    // get and store initial mouse position
      
    double x, y;
    glfwGetCursorPos(window, &x, &y );
    mouse.x = x;
    mouse.y = y;

    // enable mouse movement events
      
    glfwSetCursorPosCallback( window, mousePositionCallback );
      
  } else // (action == GLFW_RELEASE)

    // disable mouse movement events
      
    glfwSetCursorPosCallback( window, NULL );
}



// Main program


int main( int argc, char **argv )

{
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " scene.obj ..." << endl;
    exit(1);
  }

  // Set up GLFW

  glfwSetErrorCallback( GLFWErrorCallback );
  
  if (!glfwInit())
    return 1;

  glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );

  // Set up the window

  window = glfwCreateWindow( windowWidth, windowHeight, "Shadowing with G-Buffers", NULL, NULL);
  
  if (!window) {
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent( window );
  glfwSwapInterval( 1 );
  gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress );

  glfwSetKeyCallback( window, keyCallback );
  glfwSetMouseButtonCallback( window, mouseButtonCallback );
  glfwSetWindowSizeCallback( window, windowSizeCallback );

  rotationCursor = glfwCreateStandardCursor( GLFW_ARROW_CURSOR );
  translationCursor = glfwCreateStandardCursor( GLFW_CROSSHAIR_CURSOR );

  // Init fonts

#ifdef USE_FREETYPE
  initFont( "FreeSans.ttf", 20 ); // 20 = font height in pixels
#endif

  // Set up world objects

  for (int i=1; i<argc; i++)
    objs.add( new wfModel( argv[i], textureMode ) );

  if (objs.size() == 0) {
    cout << "no objects defined" << endl;
    exit(1);
  }

  // Find world centre and radius

  lookAt = vec3(0,0,0);
  for (int i=0; i<objs.size(); i++) {
    vec4 objCentre4 = objs[i]->objToWorldTransform * vec4( objs[i]->centre, 1 );
    vec3 objCentre = 1.0/objCentre4.w * vec3( objCentre4.x, objCentre4.y, objCentre4.z );
    lookAt = lookAt + objCentre;
  }
  lookAt = (1/(float)objs.size()) * lookAt;

  worldRadius = 0;
  for (int i=0; i<objs.size(); i++) {

    vec4 objCentre4 = objs[i]->objToWorldTransform * vec4( objs[i]->centre, 1 );
    vec3 objCentre = 1.0/objCentre4.w * vec3( objCentre4.x, objCentre4.y, objCentre4.z );

    
    float radius = (objCentre - lookAt).length() + objs[i]->objToWorldTransform.rows[0][0] * objs[i]->radius;
    if (radius > worldRadius)
      worldRadius = radius;
  }

  // Point camera to the model

  eyePosition = (initEyeDistance * worldRadius) * initEyeDir + lookAt;
  fovy = 2 * atan2( 1, initEyeDistance );

  vec3 t = upDir ^ initEyeDir;
  upDir = (initEyeDir ^ t).normalize();

  // Set up renderer

  axes = new Axes();

  renderer = new Renderer( windowWidth, windowHeight );

  // Main loop

  struct timeb prevTime, thisTime; // record the last rendering time
  ftime( &prevTime );

  glEnable( GL_DEPTH_TEST );

  while (!glfwWindowShouldClose( window )) {

    // Find elapsed time since last render

    ftime( &thisTime );
    float elapsedSeconds = (thisTime.time + thisTime.millitm / 1000.0) - (prevTime.time + prevTime.millitm / 1000.0);
    prevTime = thisTime;

    // Update the world state

    updateState( elapsedSeconds );

    // Clear, display, and check for events

    glClearColor( 1, 1, 1, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear depth buffer

    display();

    glfwSwapBuffers( window );
    glfwPollEvents();
  }

  // Clean up

  glfwDestroyWindow( window );
  glfwTerminate();

  return 0;
}
