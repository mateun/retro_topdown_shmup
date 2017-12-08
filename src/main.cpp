#include <stdio.h>
#include <SDL2/SDL.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

GLfloat vVertices[] = {
		0, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f
	};

GLuint vb;

GLuint loadShader(const char* shaderSrc, GLenum type) 
{
	GLuint shader;
	GLint compiled;

	shader = glCreateShader(type);
	if (shader == 0)
	{
		SDL_Log("shader 0");
		return 1;
	}

	glShaderSource(shader, 1, &shaderSrc, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char* infoLog = malloc(sizeof(char) * infoLen);

			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			SDL_Log("shader compile error: %s", infoLog);
			free(infoLog);
		}
		glDeleteShader(shader);
		return 1;
	}

	return shader;

}

int initShaders() 
{
	GLbyte vShaderStr[] = 
		"attribute vec4 vPosition;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vPosition;\n"
		"}\n";

	GLbyte fShaderStr[] = 
		"precision mediump float;			\n"
		"void main() 									\n"
		"{														\n"
		"	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}														\n";

	GLuint vertexShader, fragmentShader, programObject;
	GLint linked;

	vertexShader = loadShader(vShaderStr, GL_VERTEX_SHADER);
	fragmentShader = loadShader(fShaderStr, GL_FRAGMENT_SHADER);
	programObject = glCreateProgram();
	if (programObject == 0) 
	{
		SDL_Log("error program object"); 
		return 1;
	}

	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);
	
	glBindAttribLocation(programObject, 0, "vPosition");
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

	// init vertexbuffers
	glGenBuffers(1, &vb);
	SDL_Log("vb: %u", vb);

	glBindBuffer(GL_ARRAY_BUFFER, &vb);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, vVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

	/*glewExperimental=  true;
	GLenum err = glewInit();

	if (GLEW_OK != err) 
	{
		SDL_Log("glew init failed %s", glewGetErrorString(err));
		return 1;
	}*/

	
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

	initShaders();
		
	glClearColor(1,1,0,1);

	

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
	
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		SDL_GL_SwapWindow(window);
	}	

	SDL_JoystickClose(gameController);	
	SDL_GL_DeleteContext(glctx);
	SDL_DestroyWindow(window);
	SDL_Quit();

	printf("hello world\n");	
	return 0;
}
