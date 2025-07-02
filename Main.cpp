#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
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

		// bottom left
		vertices.push_back(-0.5f + xOff);
		vertices.push_back(-0.5f);
		vertices.push_back(0.0f);

		// bottom right
		vertices.push_back(0.5f + xOff);
		vertices.push_back(-0.5f);
		vertices.push_back(0.0f);

		// top
		vertices.push_back(0.0f + xOff);
		vertices.push_back(0.5f);
		vertices.push_back(0.0f);
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
	unsigned int vertexShader;

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
	unsigned int fragmentShader;

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// check for shader errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// SHADER PROGRAM (obj) - final linked version of multiple shaders combined
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

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
	GLuint VAO, VBO;
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO); // VBO - vertex buffer - memory on the GPU to store vertex 

	// bind VAO
	glBindVertexArray(VAO);

	// copy vertices array in a vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// static draw - data set once (vertices don't change) and used many times
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW); // copies vertex data into 


	// LINKING VERTEX SHADER ATTRIBUTES TO VERTEX DATA -- specifying how openGL should interpret vertex data before rendering
		// specifying 'position' attribute in vertex shader w/ layout (location = 0)
		// vertex attribute is a vec3, each vec coordinate is a float. thus, each vertex is 12B apart
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered the VBO (recorded into VAO) as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// uncomment to enable wireframe render 
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// event loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Drawing
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

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
