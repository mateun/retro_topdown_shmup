#include <stdio.h>
#include <SDL2/SDL.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

GLfloat vVertices[] = {
		-0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f, 
		0.5f, 0.5f, 0.0f
		
	};

GLubyte vColors[] = {
	255, 255, 0,
	100, 100, 20,
	200, 0, 100, 
	40, 200, 0
};

GLfloat vTexCoords[] = {
	0.0, 0.0, 
	0.0, 1.0, 
	1.0, 1.0, 
	1.0, 0.0
};

GLushort vIndices[] = {
	0, 1, 2, 
	2, 3, 0
};

GLuint buffers[4];
unsigned short numVertices = 12;
unsigned short numIndices = 6;
unsigned short numColors = 12;
unsigned short numTexCoords = 8;

GLuint textures[3];

GLuint loadShader(const GLbyte* shaderSrc, GLenum type) 
{
	GLuint shader;
	GLint compiled;

	shader = glCreateShader(type);
	if (shader == 0)
	{
		SDL_Log("shader 0");
		return 1;
	}

	glShaderSource(shader, 1, reinterpret_cast<const GLchar**>(&shaderSrc), NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char* infoLog = (char*) malloc(sizeof(char) * infoLen);

			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			SDL_Log("shader compile error: %s", infoLog);
			free(infoLog);
		}
		glDeleteShader(shader);
		return 1;
	}

	return shader;

}

int initScene() 
{
	GLbyte vShaderStr[] = 
		"attribute vec4 vPosition;	\n"
		"attribute vec3 a_color;		\n"
		"attribute vec2 a_texCoord;	\n"
		"varying vec2 v_texCoord; 	\n"
		"varying vec3 col;					\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vPosition;	\n"
		" col = a_color;						\n"
		" v_texCoord = a_texCoord;	\n"
		"}\n";

	GLbyte fShaderStr[] = 
		"precision mediump float;				\n"
		"varying vec3 col;							\n"
		"varying vec2 v_texCoord;				\n"
		"uniform sampler2D s_texture;		\n"
		"void main() 										\n"
		"{															\n"
		"	vec4 v_bgr = texture2D(s_texture, v_texCoord); \n"
		" gl_FragColor = v_bgr.zyxw; 		\n"
		"}															\n";

	GLuint vertexShader, fragmentShader, programObject;
	GLint linked;

	vertexShader = loadShader(reinterpret_cast<const GLbyte*>(vShaderStr), GL_VERTEX_SHADER);
	fragmentShader = loadShader(reinterpret_cast<const GLbyte*>(fShaderStr), GL_FRAGMENT_SHADER);
	programObject = glCreateProgram();
	if (programObject == 0) 
	{
		SDL_Log("error program object"); 
		return 1;
	}

	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);
	
	glBindAttribLocation(programObject, 0, "vPosition");
	glBindAttribLocation(programObject, 1, "a_color");
	glBindAttribLocation(programObject, 2, "a_texCoord");
	glLinkProgram(programObject);

	glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

	if (!linked) 
	{
		SDL_Log("link error");
		GLint infoLen = 0;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen > 1)
		{
			char* infoLog = (char*) malloc(sizeof(char) * infoLen);
			glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
			SDL_Log("linker error %s", infoLog);
			free(infoLog);
		}
		glDeleteProgram(programObject);
		return 1;
	}		
	
	glUseProgram(programObject);

	GLint posIndex = glGetAttribLocation(programObject, "vPosition");
	GLint colIndex = glGetAttribLocation(programObject, "a_color");
	GLint texCoordIndex = glGetAttribLocation(programObject, "a_texCoord");
	SDL_Log("posIndex: %d colIndex: %d texCoordIndex: %d", 
			posIndex, colIndex, texCoordIndex);

	// init vertexbuffers
	glGenBuffers(4, buffers);
	SDL_Log("vb  : %d", buffers[0]);
	SDL_Log("ib  : %d", buffers[1]);
	SDL_Log("colb: %d", buffers[2]);
	SDL_Log("texb: %d", buffers[3]);

	// Buffers for position, color and texture coords
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVertices, vVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLubyte) * numColors, vColors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numTexCoords, vTexCoords, GL_STATIC_DRAW);

	// Indexbuffer setup
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * numIndices , 
		vIndices, GL_STATIC_DRAW);
	
	// cleanly unbind all the buffer targets
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// setup textures
	// First, we generate the textures we will need
	glGenTextures(3, textures);
	SDL_Log("texture[0] %u", textures[0]);
	// Now, lets work on our first texture, 
	// which will be the procedural one.
	// First, we set the currently active 
	// texture unit. 
	// Subsequent texture bind will assiciate the
	// bound texture to the active unit.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	
	// lets create a blue texture
	GLushort texW = 2;
	GLushort texH = 2;
	GLuint sizeTexture = texW * texH * 9;
	GLubyte proceduralPixels[sizeTexture];
	int counter=0;
	for(int i = 0; i < sizeTexture; i+=3) 
	{
			proceduralPixels[i]	= 255;
			proceduralPixels[i+1]	= 0;
			proceduralPixels[i+2]	= 0;

			SDL_Log("i: %d val: %u",counter,  proceduralPixels[i]);
			SDL_Log("i+1: %d val: %u", counter+1, proceduralPixels[i+1]);
			SDL_Log("i+2: %d val: %u", counter+2, proceduralPixels[i+2]);
	} 
	
	SDL_Surface* texSurface = SDL_LoadBMP("../assets/image_one.bmp");
	if (!texSurface) 
	{
		SDL_Log("error surface %s", SDL_GetError());
		return 1;
	}

		glTexImage2D(GL_TEXTURE_2D, 0,
								GL_RGB,
								texSurface->w, 
								texSurface->h, 
								0, // ignore border
								GL_RGB,
								GL_UNSIGNED_BYTE,
								texSurface->pixels);
		
		/*glTexImage2D(GL_TEXTURE_2D,
								0, GL_RGB,
								texW, texH, 
								0, 
								GL_RGB,
								GL_UNSIGNED_BYTE,
								proceduralPixels);*/
	


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLint textureIndex = glGetUniformLocation(programObject, "s_texture");
	SDL_Log("textureIndex %d", textureIndex);
	// Now we tell the sampler in our fragment shader to use 
	// texture unit 0. 
	// At runtime, the sampler will take the image data which is
	// stored in the texture at unit 0. 
	// This seems a bit weired and complicated, but oh well... :) 
	glUniform1i(textureIndex, 0);
	
}

int main(int argc, char** args) 
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) 
	{
		SDL_Log("error %s", SDL_GetError());	
		return 1;
	}

	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	window = SDL_CreateWindow("gltest", 0, 0, 640, 480,
				SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

	if (!window)
	{
		SDL_Log("window error %s", SDL_GetError());
		return 1;
	}

	SDL_GLContext glctx = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, glctx);

	const GLubyte* version = glGetString(GL_VERSION);
	printf("version: %s\n", version);
	
	// Init our joystick
	if (SDL_NumJoysticks() < 1) 
	{
		SDL_Log("no joystick!!");
		return 1;
	}

	SDL_Joystick* gameController = SDL_JoystickOpen(0);
	printf("Opened Joystick 0\n");
  printf("Name: %s\n", SDL_JoystickNameForIndex(0));
  printf("Number of Axes: %d\n", SDL_JoystickNumAxes(gameController));
  printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(gameController));
  printf("Number of Balls: %d\n", SDL_JoystickNumBalls(gameController));
	if (gameController == NULL) 
	{
		SDL_Log("error joystick %s", SDL_GetError());
		return 1;
	}

	initScene();
		
	glClearColor(0.2,0.2,0.2,1);

	

	SDL_Event event;
	while (true) 
	{
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
		{
			break;
		}

		if (event.type == SDL_KEYDOWN)
		{
			SDL_Log("keyup!");
			break;
		}

		if (event.type == SDL_JOYBUTTONDOWN)
		{
			SDL_Log("joybutton registered!");
			break;
		}
		

		glClear(GL_COLOR_BUFFER_BIT);
	
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);
		
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
		glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const void*) 0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void*) 0);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);

		SDL_GL_SwapWindow(window);
	}	

	SDL_JoystickClose(gameController);	
	SDL_GL_DeleteContext(glctx);
	SDL_DestroyWindow(window);
	SDL_Quit();

	printf("hello world\n");	
	return 0;
}
