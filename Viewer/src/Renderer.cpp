#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

#include "Renderer.h"
#include "InitShader.h"

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(width,x,y) ((x)+(y)*(width))

Renderer::Renderer(int viewport_width, int viewport_height) :
	viewport_width(viewport_width),
	viewport_height(viewport_height)
{
	InitOpenglRendering();
	CreateBuffers(viewport_width, viewport_height);
}

Renderer::~Renderer()
{
	delete[] color_buffer;
}

void Renderer::PutPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewport_width) return;
	if (j < 0) return; if (j >= viewport_height) return;
	
	color_buffer[INDEX(viewport_width, i, j, 0)] = color.x;
	color_buffer[INDEX(viewport_width, i, j, 1)] = color.y;
	color_buffer[INDEX(viewport_width, i, j, 2)] = color.z;
}

void Renderer::DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color)
{
	// TODO: Implement bresenham algorithm
	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

	// endpoints coordinates 
	int x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y;

	bool switch_roles = false, reflect = false;
	
	// the first four cases are trivial and doesn't require bresenham algorithm
	// s.t it is when the slip is 0, infinity, 1 and -1

	// vertical line case - inf slop
	if (x1 == x2) {
		while (y1 != y2) {
			PutPixel(x1, y1, color);
			y1 = (y1 > y2) ? y1 - 1 : y1 + 1; // adjust y1 accordingly to reach y2
		}
		return;
	}

	// horizontal line case - zero slop
	if (y1 == y2) {
		while (x1 != x2) {
			PutPixel(x1, y1, color);
			x1 = (x1 > x2) ? x1 - 1 : x1 + 1; // adjust x1 accordingly to reach x2
		}
		return;
	}

	// determine which endpoint to use as a starting position
	if (x1 > x2) {
		int tmp = x1;
		x1 = x2;
		x2 = tmp;

		//swapping the second coordinate of the point accordingly 
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	float dy = y2 - y1;
	float dx = x2 - x1;
	//slop
	float slop = dy / dx;
	//error
	int e = -dx;

	// slop is 1 or -1
	if (slop == 1 || slop == -1) {
		while (x1 <= x2) {
			PutPixel(x1, y1, color);
			x1 = x1 + 1 * slop;
			y1 = y1 + 1 * slop;
		}
	}

	// calculating bresenham algorithm for every octan

	if (slop > 1) {
		switch_roles = true;
		e = -dy;

	}

	if (slop<0 && slop>-1) {
		reflect = true;
		e = -dx;
	}

	if (slop < -1) {
		reflect = true;
		switch_roles = true;
		e = -dy;
	}

	int x = x1, y = y1;

	if (switch_roles) {
		//cases where slop>1 or slop<-1
		while ((y <= y2 && !reflect) || (reflect && y >= y2)) {
			if (e > 0) {
				if (reflect) { // slop<-1
					x = x + 1;
					e = e + 2 * dy;
				}
				else {	// slop>1
					x = x + 1;
					e = e - 2 * dy;
				}
			}
			PutPixel(x, y, color);
			if (reflect) {	// slop<-1
				y = y - 1;
				e = e + 2 * dx;
			}
			else {	// slop>1
				y = y + 1;
				e = e + 2 * dx;
			}
		}
	}
	else {
		//cases where 0<slop<1 or -1<slop<0
		while (x <= x2) {
			if (e > 0) {
				if (reflect) {	//-1<slop<0
					y = y - 1;
					e = e - 2 * dx;
				}
				else {	//0<slop<1
					y = y + 1;
					e = e - 2 * dx;
				}
			}
			PutPixel(x, y, color);
			if (reflect) {	//-1<slop<0
				x = x + 1;
				e = e - 2 * dy;
			}
			else {	//0<slop<1
				x = x + 1;
				e = e + 2 * dy;
			}

		}

	}
	
}


void Renderer::Drawcircle(const glm::ivec2& p1, const float radius, const glm::vec3& color)
{
	int x1 = p1.x;
	int y1 = p1.y;
	int a = 30;	// step size
	for (int i = 0; i < a; i++)
	{
		int x2 = int(x1 + radius * sin(2 * M_PI * i / a));
		int y2 = int(y1 + radius * cos(2 * M_PI * i / a));
		glm::vec2 p2 = glm::vec2(x2, y2);
		DrawLine(p1, p2, color);
	}
	return;
}


void Renderer::CreateBuffers(int w, int h)
{
	CreateOpenglBuffer(); //Do not remove this line.
	color_buffer = new float[3 * w * h];
	ClearColorBuffer(glm::vec3(0.0f, 0.0f, 0.0f));
}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::InitOpenglRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &gl_screen_tex);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &gl_screen_vtc);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(gl_screen_vtc);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[]={
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"),0);
}

void Renderer::CreateOpenglBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewport_width, viewport_height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewport_width, viewport_height);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewport_width, viewport_height, GL_RGB, GL_FLOAT, color_buffer);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(gl_screen_vtc);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewport_width; i++)
	{
		for (int j = 0; j < viewport_height; j++)
		{
			PutPixel(i, j, color);
		}
	}
}

void Renderer::Render(const Scene& scene)
{
	// TODO: Replace this code with real scene rendering code
	int half_width = viewport_width / 2;
	int half_height = viewport_height / 2;
	// draw circle
	Drawcircle(glm::vec2(500, 300), 150, glm::vec3(1, 1, 1));

}

int Renderer::GetViewportWidth() const
{
	return viewport_width;
}

int Renderer::GetViewportHeight() const
{
	return viewport_height;
}