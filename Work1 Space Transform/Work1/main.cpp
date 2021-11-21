//============================================================
// STUDENT NAME: Chen DongCan
// NUS User ID.: t0922729
// COMMENTS TO GRADER:
//
// ============================================================

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CONSTANTS
/////////////////////////////////////////////////////////////////////////////

#define PI                  3.1415926535897932384626433832795

#define PLANET_RADIUS       100.0

#define NUM_CARS            12      // Total number of cars.

#define CAR_LENGTH          32.0
#define CAR_WIDTH           16.0
#define CAR_HEIGHT          14.0

#define CAR_MIN_ANGLE_INCR  0.5     // Min degrees to rotate car around planet each frame.
#define CAR_MAX_ANGLE_INCR  3.0     // Max degrees to rotate car around planet each frame.

#define CAR_TOP_DIST        (PLANET_RADIUS + CAR_HEIGHT)  // Distance of the top of a car from planet's center.

#define EYE_INIT_DIST       (3.0 * CAR_TOP_DIST)  // Initial distance of eye from planet's center.
#define EYE_DIST_INCR       (0.1 * CAR_TOP_DIST)  // Distance increment when changing eye's distance.
#define EYE_MIN_DIST        (1.5 * CAR_TOP_DIST)  // Min eye's distance from planet's center.

#define EYE_LATITUDE_INCR   1.0     // Degree increment when changing eye's latitude.
#define EYE_MIN_LATITUDE    -85.0   // Min eye's latitude (in degrees).
#define EYE_MAX_LATITUDE    85.0    // Max eye's latitude (in degrees).
#define EYE_LONGITUDE_INCR  1.0     // Degree increment when changing eye's longitude.

#define CLIP_PLANE_DIST     (1.1 * CAR_TOP_DIST)  // Distance of near or far clipping plane from planet's center.

#define VERT_FOV            45.0    // Vertical FOV (in degrees) of the perspective camera.

#define DESIRED_FPS         60      // Approximate desired number of frames per second.


// Planet's color.
const GLfloat planetColor[] = { 0.9, 0.6, 0.4 };

// Car tyre color.
const GLfloat tyreColor[] = { 0.2, 0.2, 0.2 };

// Material properties for all objects.
const GLfloat materialSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
const GLfloat materialShininess[] = { 100.0 };
const GLfloat materialEmission[] = { 0.0, 0.0, 0.0, 1.0 };

// Light 0.
const GLfloat light0Ambient[] = { 0.1, 0.1, 0.1, 1.0 };
const GLfloat light0Diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
const GLfloat light0Specular[] = { 0.9, 0.9, 0.9, 1.0 };
const GLfloat light0Position[] = { 1.0, 1.0, 1.0, 0.0 };

// Light 1.
const GLfloat light1Ambient[] = { 0.1, 0.1, 0.1, 1.0 };
const GLfloat light1Diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
const GLfloat light1Specular[] = { 0.9, 0.9, 0.9, 1.0 };
const GLfloat light1Position[] = { -1.0, 0.0, -0.5, 0.0 };


// Define the cars.
typedef struct CarType {
    float bodyColor[3]; // RGB color of the car body.
    double angleIncr;   // Degrees to rotate car around planet each frame.
    double angularPos;  // Angular position of car around planet (in degrees).

    double xzAxis[2];   // A vector in the x-z plane. Contains the x and z components respectively.
    double rotAngle;    // Rotation angle about the xzAxis[].
} CarType;

CarType car[ NUM_CARS ];    // Array of cars.

//To save the angleIncr of each car,and when the cars try to run,give the values to them
double eachIncreAngle[NUM_CARS];

// Define eye position.
// Initial eye position is at [ 0, 0, EYE_INIT_DIST ] in the world frame,
// looking at the world origin.
// The up-vector is assumed to be [0, 1, 0].
double eyeLatitude = 0;
double eyeLongitude = 0;
double eyeDistance = EYE_INIT_DIST;


// Window's size.
int winWidth = 800;     // Window width in pixels.
int winHeight = 600;    // Window height in pixels.


// Others.
bool pauseAnimation = false;    // Freeze the cars iff true.
bool drawAxes = true;           // Draw world coordinate frame axes iff true.
bool drawWireframe = false;     // Draw polygons in wireframe if true, otherwise polygons are filled.

void UpdateCars();


void DrawOneCar( float bodyColor[3] )
{
    glColor3fv(bodyColor);

    //****************************
    // WRITE YOUR CODE HERE.
    glPushMatrix();
    glScalef(CAR_LENGTH / 2, CAR_HEIGHT / 2, CAR_WIDTH);
    glutSolidCube(1);
    glPopMatrix();
    glPushMatrix();
    glTranslated(0, CAR_HEIGHT / 2, 0);
    glScalef(CAR_LENGTH / 4, CAR_HEIGHT / 2, CAR_WIDTH);
    glutSolidCube(1);
    glPopMatrix();
    // Draw the car body.
    //****************************

    glColor3fv(tyreColor);

    //****************************
    // WRITE YOUR CODE HERE.

    //tyre 1
    glPushMatrix();
    glTranslated(CAR_WIDTH / 2, -CAR_HEIGHT / 3, CAR_LENGTH / 4);
    glutSolidSphere(CAR_WIDTH / 4.5, 72, 36);
    glPopMatrix();
    //tyre 2
    glPushMatrix();
    glTranslated(-CAR_WIDTH / 2, -CAR_HEIGHT / 3, CAR_LENGTH / 4);
    glutSolidSphere(CAR_WIDTH / 4.5, 72, 36);
    glPopMatrix();
    //tyre 3
    glPushMatrix();
    glTranslated(CAR_WIDTH / 2, -CAR_HEIGHT / 3, -CAR_LENGTH / 4);
    glutSolidSphere(CAR_WIDTH / 4.5, 72, 36);
    glPopMatrix();
    //tyre 4
    glPushMatrix();
    glTranslated(-CAR_WIDTH / 2, -CAR_HEIGHT / 3, -CAR_LENGTH / 4);
    glutSolidSphere(CAR_WIDTH / 4.5, 72, 36);
    glPopMatrix();

    // Draw the four tyres.
    //****************************
}



/////////////////////////////////////////////////////////////////////////////
// Draw all the cars. Each is put correctly on its great circle.
/////////////////////////////////////////////////////////////////////////////

void DrawAllCars( void )
{
    for ( int i = 0; i < NUM_CARS; i++ )
    {    
        glPushMatrix();
        
       
        double yRotate_Angle = acos((double)car[i].xzAxis[1] / sqrt(car[i].xzAxis[1] * car[i].xzAxis[1] + car[i].xzAxis[0] * car[i].xzAxis[0]));

        glRotatef(yRotate_Angle * 180 / PI, 0, 1, 0);

        //Rotate the car on the surface of the sphere
        glRotatef(car[i].rotAngle, car[i].xzAxis[0] * cos(yRotate_Angle) - car[i].xzAxis[1] * sin(yRotate_Angle), 
            0, 
            car[i].xzAxis[0] * sin(yRotate_Angle) + car[i].xzAxis[1] * cos(yRotate_Angle));

        glRotatef(car[i].angularPos, 1, 0, 0);

        glTranslated(0, PLANET_RADIUS + CAR_HEIGHT / 2, 0);

        DrawOneCar(car[i].bodyColor);
        glPopMatrix();
    }
}


void DrawAxes( double length )
{
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable( GL_LIGHTING );
    glLineWidth( 3.0 );
    glBegin( GL_LINES );
        // x-axis.
        glColor3f( 1.0, 0.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 );
        glVertex3d( length, 0.0, 0.0 );
        // y-axis.
        glColor3f( 0.0, 1.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 );
        glVertex3d( 0.0, length, 0.0 );
        // z-axis.
        glColor3f( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 0.0, 0.0 );
        glVertex3d( 0.0, 0.0, length );
    glEnd();
    glPopAttrib();
}



/////////////////////////////////////////////////////////////////////////////
// The display callback function.
/////////////////////////////////////////////////////////////////////////////

void MyDisplay( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(VERT_FOV, (double)winWidth / winHeight, eyeDistance - CLIP_PLANE_DIST, eyeDistance + CLIP_PLANE_DIST);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    
    gluLookAt(
        //Camera position vector
        eyeDistance * cos(eyeLatitude / 180 * PI) * sin(eyeLongitude / 180 * PI), 
        eyeDistance * sin(eyeLatitude / 180 * PI), 
        eyeDistance * cos(eyeLatitude / 180 * PI) * cos(eyeLongitude / 180 * PI),
        //Object position vector
        0.0, 0.0, 0,
        //Up vector
        0, 1.0, 0);
   
   
    // Set world positions of the two lights.
    glLightfv( GL_LIGHT0, GL_POSITION, light0Position);
    glLightfv( GL_LIGHT1, GL_POSITION, light1Position);

    // Draw axes.
    if ( drawAxes ) DrawAxes( 2 * PLANET_RADIUS );

    // Draw planet.
    glColor3fv( planetColor );
    glutSolidSphere( PLANET_RADIUS, 72, 36 );

    UpdateCars();

    // Draw the cars.
    DrawAllCars();

    glutSwapBuffers();
}



void UpdateCars( void )
{
    for ( int i = 0; i < NUM_CARS; i++ )
    {
        car[i].angularPos += car[i].angleIncr;
        if ( car[i].angularPos > 360.0 ) car[i].angularPos -= 360.0;
    }
    glutPostRedisplay();
}


void InitCars( void )
{
    for ( int i = 0; i < NUM_CARS; i++ )
    {
        car[i].bodyColor[0] = (float)rand() / RAND_MAX;  // 0.0 to 1.0.
        car[i].bodyColor[1] = (float)rand() / RAND_MAX;  // 0.0 to 1.0.
        car[i].bodyColor[2] = (float)rand() / RAND_MAX;  // 0.0 to 1.0.

        car[i].angleIncr = (double)rand() / RAND_MAX *
                           ( CAR_MAX_ANGLE_INCR - CAR_MIN_ANGLE_INCR ) + CAR_MIN_ANGLE_INCR;
                           // CAR_MIN_ANGLE_INCR to CAR_MAX_ANGLE_INCR.
        car[i].angleIncr = car[i].angleIncr / 4;

        //save the angleIncre
        eachIncreAngle[i] = car[i].angleIncr;

        car[i].angularPos = (double)rand() / RAND_MAX * 360.0;      // 0.0 to 360.0.

        // The following 3 items defines a random great circle.
        car[i].xzAxis[0] = (double)rand() / RAND_MAX * 2.0 - 1.0;   // -1.0 to 1.0.
        car[i].xzAxis[1] = (double)rand() / RAND_MAX * 2.0 - 1.0;   // -1.0 to 1.0.
        car[i].rotAngle = (double)rand() / RAND_MAX * 360.0;        // 0.0 to 360.0.
    }
}


void MyTimer( int v )
{
    //resume the car
    if ( !pauseAnimation )
    {
        //****************************
        // WRITE YOUR CODE HERE.
        for (int i = 0; i < NUM_CARS; i++)
        {
            if (car[i].angleIncr==0)
            {
                car[i].angleIncr = eachIncreAngle[i];
            }
        }
        //****************************
    }
    //stop the car
    if (pauseAnimation)
    {
        for (int i = 0; i < NUM_CARS; i++)
        {
            car[i].angleIncr = 0;
        }
    }
}


void MyKeyboard( unsigned char key, int x, int y )
{
    switch ( key )
    {
        // Quit program.
        case 'q':
        case 'Q':
            exit(0);
            break;

        // Toggle between wireframe and filled polygons.
        case 'w':
        case 'W':
            drawWireframe = !drawWireframe;
            if ( drawWireframe )
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            else
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            glutPostRedisplay();
            break;

        // Toggle axes.
        case 'x':
        case 'X':
            drawAxes = !drawAxes;
            glutPostRedisplay();
            break;

         // Pause or resume animation.
        case 'p':
        case 'P':
            pauseAnimation = !pauseAnimation;
            glutTimerFunc( 0, MyTimer, 0 );
            break;

       // Reset to initial view.
        case 'r':
        case 'R':
            eyeLatitude = 0.0;
            eyeLongitude = 0.0;
            eyeDistance = EYE_INIT_DIST;
            glutPostRedisplay();
            break;

    }
}


void KeyCallback( int key, int x, int y )
{
    switch ( key )
    {
        case GLUT_KEY_LEFT:
            eyeLongitude -= EYE_LONGITUDE_INCR;
            if ( eyeLongitude < -360.0 ) eyeLongitude += 360.0 ;
            glutPostRedisplay();
            break;

        case GLUT_KEY_RIGHT:
            eyeLongitude += EYE_LONGITUDE_INCR;
            if ( eyeLongitude > 360.0 ) eyeLongitude -= 360.0 ;
            glutPostRedisplay();
            break;

        case GLUT_KEY_DOWN:
            eyeLatitude -= EYE_LATITUDE_INCR;
            if ( eyeLatitude < EYE_MIN_LATITUDE ) eyeLatitude = EYE_MIN_LATITUDE;
            glutPostRedisplay();
            break;

        case GLUT_KEY_UP:
            eyeLatitude += EYE_LATITUDE_INCR;
            if ( eyeLatitude > EYE_MAX_LATITUDE ) eyeLatitude = EYE_MAX_LATITUDE;
            glutPostRedisplay();
            break;

        case GLUT_KEY_PAGE_UP:
            eyeDistance -= EYE_DIST_INCR;
            if ( eyeDistance < EYE_MIN_DIST ) eyeDistance = EYE_MIN_DIST;
            glutPostRedisplay();
            break;

        case GLUT_KEY_PAGE_DOWN:
            eyeDistance += EYE_DIST_INCR;
            glutPostRedisplay();
            break;
    }
}


void ViewRest( int w, int h )
{
    winWidth = w;
    winHeight = h;
    glViewport( 0, 0, w, h );
}


void MyInit( void )
{
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); // Set black background color.
    glEnable( GL_DEPTH_TEST ); // Use depth-buffer for hidden surface removal.
    glShadeModel( GL_SMOOTH );
    // Set Light 0.
    glLightfv( GL_LIGHT0, GL_AMBIENT, light0Ambient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, light0Diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, light0Specular );
    glEnable( GL_LIGHT0 );

    // Set Light 1.
    glLightfv( GL_LIGHT1, GL_AMBIENT, light1Ambient );
    glLightfv( GL_LIGHT1, GL_DIFFUSE, light1Diffuse );
    glLightfv( GL_LIGHT1, GL_SPECULAR, light1Specular );
    glEnable( GL_LIGHT1 );

    glEnable( GL_LIGHTING );

    // Set some global light properties.
    GLfloat globalAmbient[] = { 0.1, 0.1, 0.1, 1.0 };
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, globalAmbient );
    glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE );
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );

    // Set the universal material properties.
    // The diffuse and ambient components can be changed using glColor*().
    glMaterialfv( GL_FRONT, GL_SPECULAR, materialSpecular );
    glMaterialfv( GL_FRONT, GL_SHININESS, materialShininess );
    glMaterialfv( GL_FRONT, GL_EMISSION, materialEmission );
    glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
    glEnable( GL_COLOR_MATERIAL );

    glEnable( GL_NORMALIZE ); // Let OpenGL automatically renomarlize all normal vectors.
}


static void WaitForEnterKeyBeforeExit(void)
{
    printf("Press Enter to exit.\n");
    fflush(stdin);
    getchar();
}


int main( int argc, char** argv )
{
    atexit(WaitForEnterKeyBeforeExit); // atexit() is declared in stdlib.h

    srand(27);  // set random seed

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( winWidth, winHeight );
    glutCreateWindow( "main" );

    MyInit();
    InitCars();

    // Register the callback functions.
    glutDisplayFunc( MyDisplay );
    glutReshapeFunc( ViewRest );
    glutKeyboardFunc( MyKeyboard );
    glutSpecialFunc( KeyCallback );
    glutTimerFunc( 0, MyTimer, 0 );

    // Display user instructions in console window.
    printf( "Press LEFT ARROW to move eye left.\n" );
    printf( "Press RIGHT ARROW to move eye right.\n" );
    printf( "Press DOWN ARROW to move eye down.\n" );
    printf( "Press UP ARROW to move eye up.\n" );
    printf( "Press PAGE UP to move closer.\n" );
    printf( "Press PAGE DN to move further.\n" );
    printf( "Press 'P' to toggle car animation.\n" );
    printf( "Press 'W' to toggle wireframe.\n" );
    printf( "Press 'X' to toggle axes.\n" );
    printf( "Press 'R' to reset to initial view.\n" );
    printf( "Press 'Q' to quit.\n\n" );

    // Enter GLUT event loop.
    glutMainLoop();
    return 0;
}
