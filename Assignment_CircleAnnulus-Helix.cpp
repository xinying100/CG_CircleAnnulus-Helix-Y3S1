/* Heng Chia Ying -- CST2104280 */

#define _USE_MATH_DEFINES
#include <GL/glew.h>
#include <gl/freeglut.h>
#include <cmath>
#include <stdio.h>
#include <cstdlib>
#include <iostream>

// GLOBAL VARIABLES
static int isWire					= 0;
static float greyness				= 0;
static const int circle_Vertices	= 240;				// Circle Vertices
static const int tri_Vertices		= 3;				// Triangle Vertices
static const int helix_Vertices		= 240;				// Helix Vertices
static unsigned int Helix;
static unsigned int buffer[6];							// Array of buffer ids
static long font = (long)GLUT_BITMAP_HELVETICA_18;		// Font selection

// 1st Figure -- Circle Annulus
static float vertices_c1[3 * helix_Vertices];			// LeftCircle  -- BUFFER 0
static float colors_c1[3 * helix_Vertices];				// LeftCircle  -- BUFFER 0
static float vertices_h[3 * helix_Vertices];			// helix       -- BUFFER 5
static float colors_h[3 * helix_Vertices];				// helix       -- BUFFER 5
static unsigned int stripIndices1[circle_Vertices + 3];	// LeftCircle  -- BUFFER 1

// 2nd Figure -- Fake Annulus
static float vertices_c2[3 * circle_Vertices];			// RightCircle  -- BUFFER 2
static float colors_c2[3 * circle_Vertices];			// RightCircle  -- BUFFER 2
static float vertices_t[3 * tri_Vertices];				// triangle     -- BUFFER 4
static float colors_t[3 * tri_Vertices];				// triangle     -- BUFFER 4
static unsigned int stripIndices2[circle_Vertices + 3]; // RightCircle  -- BUFFER 3

void writeBitmapString(void* font, const char* string)
{
	const char* c;							// String Pointer
	for (c = string; *c != '\0'; c++)		// Printing each char till the End Of String
		glutBitmapCharacter(font, *c);		// Not our function
}

void VBO_LeftCircle()
{
	/* Left Circle */
	// Bind vertex buffer and reserve space
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_c1) + sizeof(colors_c1), NULL, GL_DYNAMIC_DRAW);

	// Seperate buffer[0] into 2 half, Copy VERTEX COORDINATES data into 1st half
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_c1), vertices_c1);
	// Copy VERTEX COLOR data into 2nd half.
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices_c1), sizeof(colors_c1), colors_c1);

	// Bind and fill INDICES buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(stripIndices1), stripIndices1, GL_DYNAMIC_DRAW);

	glVertexPointer(3, GL_FLOAT, 0, 0);
	glColorPointer(3, GL_FLOAT, 0, (void*)(sizeof(vertices_c1)));
	glDrawElements(GL_TRIANGLE_STRIP, circle_Vertices + 3, GL_UNSIGNED_INT, 0);
}

void VBO_RightCircle()
{
	/* Right Circle */
	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_c2) + sizeof(colors_c2), NULL, GL_DYNAMIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_c2), vertices_c2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices_c2), sizeof(colors_c2), colors_c2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(stripIndices2), stripIndices2, GL_DYNAMIC_DRAW);

	glVertexPointer(3, GL_FLOAT, 0, 0);
	glColorPointer(3, GL_FLOAT, 0, (void*)(sizeof(vertices_c2)));
	glDrawElements(GL_TRIANGLE_STRIP, circle_Vertices + 3, GL_UNSIGNED_INT, 0);
}

void VBO_Triangle()
{
	/* TRIANGLE VBO */
	glBindBuffer(GL_ARRAY_BUFFER, buffer[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_t) + sizeof(colors_t), NULL, GL_DYNAMIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_t), vertices_t);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices_t), sizeof(colors_t), colors_t);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glColorPointer(3, GL_FLOAT, 0, (void*)(sizeof(vertices_t)));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, tri_Vertices);
}

void VBO_Helix()
{
	/* HELIX VBO */
	glBindBuffer(GL_ARRAY_BUFFER, buffer[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_h) + sizeof(colors_h), NULL, GL_DYNAMIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_h), vertices_h);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices_h), sizeof(colors_h), colors_h);

	Helix = glGenLists(1);								// Return a list index
	glNewList(Helix, GL_COMPILE);						// Begin of a display list
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glColorPointer(3, GL_FLOAT, 0, (void*)(sizeof(vertices_h)));
	glDrawArrays(GL_LINE_STRIP, 0, helix_Vertices);
	glEndList();										// End of a display list
}

void triangle_stripping() {
	float x1_center			= 50;
	float y1_center			= 55;
	float z1_center			= -1;
	float circle1_radius	= 40;
	float tri1_radius		= 30;
	float helix_radius		= 14;
	float circle1_theta		= 0.0;
	float tri1_theta		= 0.0;
	float helix_theta		= 0.0;
	int j = 0, k = 0;

	// Generate StripIndices
	for (int i = 0; i < circle_Vertices; i++)	{stripIndices1[i] = i;}
	stripIndices1[circle_Vertices + 1] = 1;

	// Drawing Circle
	for (int i = 0; i < 3 * circle_Vertices; i += 3)
	{
		if (i%2 == 0)				// even (Inner Triangle Vertex Position)
		{
			/* Distributed the circle_vertices into 3 part,
			   vertices of each part connect to each of the pinit of triangle vertices */
			if (i % (3 * circle_Vertices / tri_Vertices) == 0)		
			{
				tri1_theta		   = (2 * M_PI * j / tri_Vertices) + (M_PI / 2);
				j++;
				vertices_c1[i]     = x1_center + cos(tri1_theta) * tri1_radius;
				vertices_c1[i + 1] = y1_center + sin(tri1_theta) * tri1_radius;
				vertices_c1[i + 2] = z1_center+1;
			}
			else
			{
				vertices_c1[i]     = x1_center + cos(tri1_theta) * tri1_radius;
				vertices_c1[i + 1] = y1_center + sin(tri1_theta) * tri1_radius;
				vertices_c1[i + 2] = z1_center+1;
			}
		}
		else					// odd (Outter Circle Vertex Position)
		{
			vertices_c1[i]     = x1_center + cos(circle1_theta) * circle1_radius;
			vertices_c1[i + 1] = y1_center + sin(circle1_theta) * circle1_radius;
			vertices_c1[i + 2] = z1_center+1;
		}
		
		// Color vary between white and black (default is black)
		colors_c1[i]     = greyness;
		colors_c1[i + 1] = greyness;
		colors_c1[i + 2] = greyness;
		
		circle1_theta    = (2 * M_PI * k / circle_Vertices) + (M_PI / 2);
		k++;
	}

	// Helix 
	for (int i = 0; i < 3 * helix_Vertices; i += 3)
	{
		vertices_h[i]   = x1_center + helix_radius * cos(helix_theta);
		vertices_h[i+1] = y1_center + helix_radius * sin(helix_theta);
		vertices_h[i+2] = z1_center+2;
		colors_h[i]		= 1;
		colors_h[i + 1] = 0;
		colors_h[i + 2] = 0;

		// 6 Spiral Coils --> 1 Coils requires 2 PI
		helix_theta  += (6 * 2) * M_PI / helix_Vertices;
		// Reducing the radius every loop (inner radius further away from viewer)
		helix_radius -= 0.05;
		z1_center    -= 2.0 / helix_Vertices ;
	}
}

void fake_annulus()
{
	float x2_center			= 150;
	float y2_center			= 55;
	float z2_center			= -1;
	float circle2_radius	= 40;
	float tri2_radius		= 30;
	float circle2_theta		= 0.0;
	float tri2_theta		= 0.0;
	int j = 0;

	// Generate StripIndices
	for (int i = 0; i < circle_Vertices; i++)	{stripIndices2[i] = i;}

	// Drawing Circle
	for (int i = 0; i < 3 * circle_Vertices; i += 3)
	{
		if (i % 2 == 1)				// odd (Inner Circle Vertex Position)
		{
			vertices_c2[i]     = x2_center;
			vertices_c2[i + 1] = y2_center;
			vertices_c2[i + 2] = z2_center;
			// Color vary between white and black (default is black)
			colors_c2[i]       = greyness;
			colors_c2[i + 1]   = greyness;
			colors_c2[i + 2]   = greyness;
		}
		else						// even (Outter Circle Vertex Position)
		{
			vertices_c2[i]     = x2_center + cos(circle2_theta) * circle2_radius;
			vertices_c2[i + 1] = y2_center + sin(circle2_theta) * circle2_radius;
			vertices_c2[i + 2] = z2_center;
			// Color vary between white and black (default is black)
			colors_c2[i]       = greyness;
			colors_c2[i + 1]   = greyness;
			colors_c2[i + 2]   = greyness;
		}
		circle2_theta += 2 * M_PI / circle_Vertices;
	}

	// Draw Triangle
	for (int i = 0; i < 3 * tri_Vertices; i += 3)
	{
		tri2_theta		  = (2 * M_PI * j / tri_Vertices) + (M_PI / 2);
		j++;	
		vertices_t[i]	  = x2_center + cos(tri2_theta) * tri2_radius;
		vertices_t[i + 1] = y2_center + sin(tri2_theta) * tri2_radius;
		vertices_t[i + 2] = z2_center + 0.01;
		colors_t[i]       = 1.0;
		colors_t[i + 1]   = 1.0;
		colors_t[i + 2]   = 1.0;
	}
}

void draw_Helix()
{
	// Left Helix
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);						// Translation/duplicate the helix to the left
	glCallList(Helix);
	glPopMatrix();

	// Right Helix 
	glPushMatrix();
	glTranslatef(100.0, 0.0, 0.0);						// Translation/duplicate the helix to the right
	glCallList(Helix);
	glPopMatrix();
}

void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (isWire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glLineWidth(2.0);
	triangle_stripping();
	fake_annulus();
	
	// Perform the VBOs of 4 items (Left Circle Annulus, Right Circle, Helix, Triangle)
	VBO_LeftCircle();
	VBO_RightCircle();
	VBO_Triangle();
	VBO_Helix();
	
	draw_Helix();

	// Write labels.
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos3f(30.0, 10.0, 0.0);
	writeBitmapString((void*)font, "Circle Annulus (using Triangle Strip)");
	glRasterPos3f(135, 10.0, 0.0);
	writeBitmapString((void*)font, "Fake Annulus (using Z-Buffer)");

	glFlush();
}

void setup() {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glGenBuffers(6, buffer);							// Generate 6 buffer id
	glEnable(GL_DEPTH_TEST);
	// Enable two vertex arrays: co-ordinates and color
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

}

// Reshape Window
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 200.0, 0.0, 100.0, -1.0, 1.1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Keyboard Input Function
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		if (isWire == 0)
		{
			isWire = 1;
			std::cout << " -- Wired Frame Circle Annulus shown --" << std::endl;
		}
		else
		{
			isWire = 0;
			std::cout << "   -- Filled Circle Annulus shown --" << std::endl;
		}
		glutPostRedisplay();
		break;
	case '+':
		if (greyness < 1)
		{
			greyness += 0.05;
			std::cout << "Increasing greyness...(White)" << std::endl;
		}
		else
			std::cout << "The greyness of the circle annlus reached maximum. (Lightest)" << std::endl;
		glutPostRedisplay();
		break;
	case '-':
		if (greyness > 0)
		{
			greyness -= 0.05;
			std::cout << "Decreasing greyness...(Black)" << std::endl;
		}
		else
			std::cout << "The greyness of the circle annlus reached minimum. (Darkest)" << std::endl;
		glutPostRedisplay();
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

// Terminal Interaction Window
void printInteraction()
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press +/- to increase/decrease the greyness of the black color of the circle." << std::endl;
	std::cout << "Press the space bar to toggle between wirefrime and filled for the circle annulus object." << std::endl;
	std::cout << "-------------------------------------------------------------------------------------------" << std::endl;
}

// Main Function
int main(int argc, char** argv)
{
	printInteraction();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);		// GLUT_DEPTH give depth on visualization
	glutInitWindowSize(1500,750);
	glutInitWindowPosition(10, 20);
	glutCreateWindow("CST2104280_Assignment.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}