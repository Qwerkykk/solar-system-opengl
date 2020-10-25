#define ROTATION_SPEED 1.8f

#define EARTH_SPEED 0.01f
#define EARTH_DISTANCE 60.0

#define EARTH_SCALE 0.01f

#define MOON_SPEED 0.05f
#define MOON_DISTANCE 10.0

#define MOON_SCALE 0.003f

struct point 
{
	float x; 
	float y; 
	float z; 
};

struct face
{
	int vtx[3];
};

struct model
{
	point obj_points[9123];
	face obj_faces[18241];
	int vertices;
	int faces;
};


//-------- Functions --------------------------------

void Render();
// The function responsible for drawing everything in the 
// OpenGL context associated to a window. 

void Resize(int w, int h);
// Handle the window size changes and define the world coordinate 
// system and projection type

void Setup();
// Set up the OpenGL state machine and create a light source

void Idle();

void ReadFile(model*);
//Function for reading a model file

void DisplayModel(model);
// Function for displaying a model

void Keyboard(unsigned char key,int x,int y);
// Function for handling keyboard events.



