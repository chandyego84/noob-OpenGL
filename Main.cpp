#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;
const unsigned int NUM_TRIANGLES = 2;
const unsigned int FLOATS_PER_TRIANGLE = 18; // vertex floats (x,y,z) + color floats (r,g,b) = 6 * 3 = 18/Triangle

const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n" // position variable with attribute position 0
	"layout (location = 1) in vec3 aColor;\n" // position variable with attribute position 1

	// "uniform float xOffset;\n"

	"out vec3 vertexPosition;\n" // output the position to the fragment shader
	"out vec3 vertexColor;\n" // output a color fragment shader

	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"	vertexPosition = aPos;\n"	
	"	vertexColor = aColor;\n"
	"}\0";

const char* fragmentShaderSource1 = "#version 330 core\n"
	"in vec3 vertexPosition;\n"
	"in vec3 vertexColor;\n" // input variable from the vertex shader (same name and type)
	

	"out vec4 FragColor;\n"
	
	"void main()\n"
	"{\n"
	"	FragColor = vec4(vertexPosition, 1.0);\n"
	"}\0";

// uniforms: another way to pass data from our app on the CPU to shaders on the GPU
	// global -- unique per shader program object, can be accessed from any shader at any stage in shader program
const char* fragmentShaderSource2 = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"uniform vec4 ourColor;\n"

	"void main()\n"
	"{\n"
	"	FragColor = ourColor;\n"
	"}\0";


int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// vertices for two triangles
	std::vector<float> vertices;
	for (int v = 0; v < 2; v++) {
		float xOff = (v == 0) ? -0.5f : 0.5f;
		int isUpsideDown = (v == 0) ? 1 : -1;

		// bottom left
		vertices.push_back(-0.5f + xOff);
		vertices.push_back(isUpsideDown * -0.5f);
		vertices.push_back(0.0f);
		// color (R)
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);

		// bottom right
		vertices.push_back(0.5f + xOff);
		vertices.push_back(isUpsideDown * -0.5f);
		vertices.push_back(0.0f);
		// color (G)
		vertices.push_back(0.0f);
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);

		// top
		vertices.push_back(0.0f + xOff);
		vertices.push_back(isUpsideDown * 0.5f); 
		vertices.push_back(0.0f);
		// color (B)
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		vertices.push_back(1.0f);
	}	

	// Window Init
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "NoobOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create OpenGL window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// GLAD init -- load openGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Viewport init
	glViewport(0, 0, WIDTH, HEIGHT);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// VERTEX SHADER - processing on the vertex attributes
	int success;
	char infoLog[512];
	unsigned int vertexShader, vertexShaderBasic;

	// create vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// bind shader code to shader obj
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader errors
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// FRAGMENT SHADER - calcualting color outputs of pixels
	unsigned int fragmentShader1, fragmentShader2;

	// fragment1
	fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
	glCompileShader(fragmentShader1);
	// check for shader errors
	glGetShaderiv(fragmentShader1, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader1, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// fragment2
	fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
	glCompileShader(fragmentShader2);
	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT2::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// SHADER PROGRAM (obj) - final linked version of multiple shaders combined
	unsigned int shaderProgram1, shaderProgram2; // fragment shaders
	shaderProgram1 = glCreateProgram();
	shaderProgram2 = glCreateProgram();

	glAttachShader(shaderProgram1, vertexShader);
	glAttachShader(shaderProgram1, fragmentShader1);
	glLinkProgram(shaderProgram1);

	glGetProgramiv(shaderProgram1, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram1, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// SECOND SHADER PROGRAM w/ FRAGMENT SHADER 2
	glAttachShader(shaderProgram2, vertexShader);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);
	
	// delete shaders since we have shader programs already
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader1);
	glDeleteShader(fragmentShader2);

	// VERTEX ARRAY OBJECT (VAO) -- makes above config process more scalable
		// store all state configs into an object and bind this object to restore its state.
		// can be bound just like a VBO and any subsequent vertex attribute calls from that point on will be stored in the VAO.
		// allows to make vertex attrib calls only once. when you want to draw an object, just bind corresponding VAO.
		// makes switching between different vertex data attribute configs as easy as bindnig to a different VAO.
		// -------------
		// Stores the following:
			// calls to 'glEnableVertexAttribArray' (or its disable)
			// Vertex attribute configs via glVertexAttribPointer
			// VBOs associated with vertex attribs by calls to 'glVertexAttribPointer'
	GLuint VAO[2], VBO[2];
	
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO); // VBO - vertex buffer - memory on the GPU to store vertex 

	// SET UP FOR VAO1
	glBindVertexArray(VAO[0]);

	// copy vertices array in a vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	// static draw - data set once (vertices don't change) and used many times
	glBufferData(GL_ARRAY_BUFFER, FLOATS_PER_TRIANGLE * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	// LINKING VERTEX SHADER ATTRIBUTES TO VERTEX DATA -- specifying how openGL should interpret vertex data before rendering
		// specifying 'position' attribute in vertex shader w/ layout (location = 0)
		// vertex attribute is a vec3, each vec coordinate is a float. thus, each vertex is 12B apart
	// position attribute
	glVertexAttribPointer(0,		// layout(location = 0) -- position
		3,							// 3 componenets (x,y,z)
		GL_FLOAT,					// type
		GL_FALSE,					// don't normalize
		6 * sizeof(float),			// stride (each vertex is 6 floats)
		(void*)0);					// offset -- of this attribute (position)
	glEnableVertexAttribArray(0);
	
	// color attribute
	glVertexAttribPointer(1, 
		3, 
		GL_FLOAT, 
		GL_FALSE, 
		6 * sizeof(float), 
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // we are unbinding hre, but we do not actually need to unbind as we bind to a different VAO in next lines
	glBindBuffer(GL_ARRAY_BUFFER, 0); // also no need to unbind here, but let's keep them for learning's sake

	// SET UP FOR VAO2
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, FLOATS_PER_TRIANGLE * sizeof(float) * sizeof(float), vertices.data() + FLOATS_PER_TRIANGLE, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// uncomment to enable wireframe render 
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// event loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Drawing
		glUseProgram(shaderProgram1);
		// Draw first shape
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Draw second shape
			// we are setting uniform ("ourColor") value in the CPU instead of passing from one shader to the next
		glUseProgram(shaderProgram2);
		float timeValue = glfwGetTime();
		float greenValue = sin(timeValue) / 2.0f + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram2, "ourColor");
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		
		/*
		float vOffset = -0.5f;
		int vertexOffsetLocation = glGetUniformLocation(shaderProgram2, "xOffset");
		glUniform1f(vertexOffsetLocation, vOffset);
		*/

		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteProgram(shaderProgram1);
	glDeleteProgram(shaderProgram2);

	glfwTerminate();
	return 0;
}

// window resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}
