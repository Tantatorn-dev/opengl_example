#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

struct shaderProgramSource {
	string vertexSource;
	string fragmentSource;
};

static shaderProgramSource parseShader(const string& filepath) {
	ifstream stream(filepath);

	enum class shaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	string line;
	stringstream ss[2];

	shaderType type = shaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("#shader") != string::npos) {
			if (line.find("vertex") != string::npos) {
				type = shaderType::VERTEX;
			}
			else if (line.find("fragment") != string::npos) {
				type = shaderType::FRAGMENT;
			}
		}
		else {
			ss[(int)type]<<line<<'\n';
		}
	}

	return{ ss[0].str(),ss[1].str() };
	}


static unsigned int CompileShader(unsigned int type ,const string& source ) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id,1,&src,nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &result);
	if (result == GL_FALSE) {
		int len;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
		char* message = (char*)alloca(len * sizeof(char));
		glGetShaderInfoLog(id, len, &len, message);

		cout << "failed" << endl;
		cout << message << endl;
		glDeleteShader(id);
		return 0;
	}


	return id;
}



static unsigned int CreateShader(const string& vertexShader,const string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER ,vertexShader );
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}

int main() {
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;


	


	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glewInit();

	cout << glGetString(GL_VERSION) << endl;


	float points[] = {   0.5f,0.5f, //0
						 0.5f,-0.5f, //1
		                -0.5f,-0.5f, //2
						-0.5f,0.5f	//3 
					 };

	unsigned int index[] = {0,1,3,3,1,2};

	
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER,buffer);
	glBufferData(GL_ARRAY_BUFFER,2*6*sizeof(float),points, GL_STATIC_DRAW);

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), index, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float)*2 ,0);


	shaderProgramSource source = parseShader("res/shaders/basic.shader");


	unsigned int shader = CreateShader(source.vertexSource,source.fragmentSource);
	glUseProgram(shader);

	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);


		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}