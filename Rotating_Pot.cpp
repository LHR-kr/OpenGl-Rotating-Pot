#include<GL/glew.h> // �̰� ���� include
#include<GL/glut.h>
#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<glm/gtc/matrix_transform.hpp>
#include<math.h>


GLuint vertexBufferID;
GLuint ColorBufferID;
GLuint MatrixID;
GLuint NormalBufferID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint LightID;


glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
glm::mat4 mvp;

glm::vec3 vertices[20000];

float angle;
float camera_y=3;
float r = 0;

std::vector<glm::vec3> vv;

bool loadOBJ(const char* path, std::vector<glm::vec3>& vertices);


int input;

// �ð��� ���� ���� ���� �ݰ�� ī�޶� ������ �Ÿ��� Ŀ����.
void timer(int value) {
	angle += glm::radians(5.0f);

	camera_y += 0	.08;
	r += 0.03;

	glutPostRedisplay();
	glutTimerFunc(20, timer, 0);
}

void transform() {
	// Model matrix : an identity matrix (model will be at the origin)
	Model = glm::mat4(1.0f);

	
	// ���� affine ��ȯ�� ���� ���
	// ��� t�� transform���� ��ü�� ī�޶󿡼� �־����� �����
	// ��� r�� rotate�� ��ü�� ȸ�� ��Ų��
	glm::mat4 t = glm::translate(Model, glm::vec3(r, 0 , 0));
	glm::mat4 r = glm::rotate(Model, angle, glm::vec3(0, 1, 0));

	//��İ�. transform�� ���� �Ͼ�� rotate�� �Ͼ��. ������ �߿�
	Model = r * t;

	Model = glm::rotate(Model, angle, glm::vec3(0, 1, 0));

	//ī�޶� ���� ����
	View = glm::lookAt(
		glm::vec3(0, camera_y, 0),// Camera is at (0,0,3), in World Space
		glm::vec3(0, 0, 0),// and looks at the origin
		glm::vec3(0, 0, 1)// Head is up (set to 0,-1,0 to look upside-down
	);

	// ModelViewProjection, ���� ����
	mvp = Projection * View * Model;

	

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
	glm::vec3 lightPos = glm::vec3(10, 10, 10);
	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

}

void mydisplay()
{
	transform();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	

	//vertex
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//color
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferID);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//normal vector
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBufferID);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	if (input == 1) {
		glDrawArrays(GL_TRIANGLES, 0, 36);

	}

	else if (input == 2)
	{
		glDrawArrays(GL_TRIANGLES, 0, 18960);
	}
	else if (input == 3)
	{
		glDrawArrays(GL_TRIANGLES, 0, 12612);
	}

	glDisableVertexAttribArray(0);

	glutSwapBuffers();

}

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
GLuint programID;

void init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID); 
	glBindVertexArray(VertexArrayID); 

	int s = vv.size();

	for (int i = 0; i < s; i++)
	{
		vertices[i] = vv[i];
	}


	glGenBuffers(1, &vertexBufferID); //vram ������ �ּ� �Ҵ�
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID); // �־��� vram �ּҿ� ���� �Ҵ�
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glm::vec3 colors[36] = {};

	glGenBuffers(1, &ColorBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);


	auto getNormal = [](const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& point3)
	{
		glm::vec3 edge1 = point2 - point1;
		glm::vec3 edge2 = point3 - point2;
		return glm::normalize(glm::cross(edge1, edge2));
	};
	glm::vec3 normals[36] = {};

	glGenBuffers(1, &NormalBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	programID = LoadShaders("simple.vshader", "simple.fshader");

	// ���̴��� ���ε�
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	LightID = glGetUniformLocation(programID, "LightPosition");


	glUseProgram(programID);


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); 

	MatrixID = glGetUniformLocation(programID, "MVP");
}

void myreshape(int w, int h)
{
	glViewport(0, 0, w, h);

	Projection = glm::perspective(glm::radians(45.0f),
		(float)w / (float)h, 0.1f, 100.0f);

	View = glm::lookAt(
		glm::vec3(0, camera_y, 0),// Camera is at (0,0,3), in World Space
		glm::vec3(0, 0, 0),// and looks at the origin
		glm::vec3(0, 1, 0)// Head is up (set to 0,-1,0 to look upside-down

	);

	transform();
}

int main(int argc, char** argv)
{

	std::cout << "obj�� �������ּ���." << std::endl;
	std::cout << "tetrahdron.obj�� ���Ͻø� 1�� �����ּ���." << std::endl;
	std::cout << "teapot.obj�� ���Ͻø� 2�� �����ּ���." << std::endl;
	std::cout << "bones.obj�� ���Ͻø� 3�� �����ּ���." << std::endl;

	std::cin >> input;


	glutInit(&argc, argv);

	glutInitWindowSize(400, 400);
	glutInitWindowPosition(500, 500);

	if(input==1){
		if (!loadOBJ("tetrahedron.obj", vv))
		{
			std::cout << "can't load obj file" << std::endl;
			return 0;
		}
	}
	if (input == 2) {
		if (!loadOBJ("teapot.obj", vv))
		{
			std::cout << "can't load obj file" << std::endl;
			return 0;
		}
	}if (input == 3) {
		if (!loadOBJ("bones.obj", vv))
		{
			std::cout << "can't load obj file" << std::endl;
			return 0;
		}
	}

	glutCreateWindow("18101257 ���ض�");
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutTimerFunc(0, timer, 0);
	glutDisplayFunc(mydisplay);
	glutReshapeFunc(myreshape);


	GLenum err = glewInit();
	if (err == GLEW_OK)
	{

		init();		
		glutMainLoop();
	}
}

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}
	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}
	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}


bool loadOBJ(const char* path, std::vector<glm::vec3>& vertices)
{
	std::vector<unsigned int> vertexIndices;
	std::vector<glm::vec3> temp_vertices;

	FILE* file = NULL;

	if (fopen_s(&file, path, "r") != NULL) {
		printf("File can't be opened\n");
		return false;
	}

	while (true) {
		char lineHeader[128];
		// read the first word of the line
		if (fscanf_s(file, "%s", lineHeader, 128) == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}

		else if (strcmp(lineHeader, "f") == 0) {
			unsigned int vertexIndex[3];
			int matches = fscanf_s(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
			if (matches != 3) {
				printf("File can't be read\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
		}
	}

	fclose(file);

	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		vertices.push_back(vertex);
	}

}