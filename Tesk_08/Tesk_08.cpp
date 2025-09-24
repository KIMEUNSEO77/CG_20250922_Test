#define _CRT_SECURE_NO_WARNINGS 
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <glew.h>
#include <freeglut.h>
#include <freeglut_ext.h> 
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);

int width = 1000, height = 800;
GLuint shaderProgramID; // ���̴� ���α׷� �̸�
GLuint vertexShader;    // ���ؽ� ���̴� ��ü
GLuint fragmentShader;  // �����׸�Ʈ ���̴� ��ü

enum class ShapeType { Point, Line, Triangle, Rect };

struct Shape 
{
	ShapeType type;
	GLuint VAO = 0, VBO = 0;
	vector<glm::vec3> vertices;
	glm::vec4 color;
	int vertCount() const { return (int)vertices.size(); }
};
vector<Shape> shapes;    // �ִ� 10��. ������ ������ ����
int selectedShape = -1;  // ���õ� ���� �ε���

float mouseX = 0.0f, mouseY = 0.0f; // ���콺 ��ǥ

void updateShape(Shape& shape)
{
	if (!shape.VAO) glGenVertexArrays(1, &shape.VAO);
	if (!shape.VBO) glGenBuffers(1, &shape.VBO);

	glBindVertexArray(shape.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
	glBufferData(GL_ARRAY_BUFFER, shape.vertices.size() * sizeof(glm::vec3), shape.vertices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ���콺 Ŭ������ �� �߰�
void AddPoint(float nx, float ny)
{
	if (shapes.size() >= 10) return; // �ִ� 10�� ����
	Shape shape;
	shape.type = ShapeType::Point;
	shape.color = glm::vec4(1.0, 0.0, 0.0, 1.0); // ������
	shape.vertices = { glm::vec3(nx, ny, 0.0f) };
	updateShape(shape);
	shapes.push_back(shape);
}

// �� �߱�
void AddLine(float nx, float ny)
{
	if (shapes.size() >= 10) return; // �ִ� 10�� ����
	Shape shape;
	shape.type = ShapeType::Line;
	shape.color = glm::vec4(0.0, 1.0, 0.0, 1.0); // �ʷϻ�
	shape.vertices = { glm::vec3(nx, ny, 0.0f), glm::vec3(nx - 0.2f, ny - 0.1f, 0.0f) };
	updateShape(shape);
	shapes.push_back(shape);
}

// �ﰢ�� �׸���
void AddTriangle(float nx, float ny)
{
	if (shapes.size() >= 10) return; // �ִ� 10�� ����
	Shape shape;
	shape.type = ShapeType::Triangle;
	shape.color = glm::vec4(0.0, 0.0, 1.0, 1.0); // �Ķ���
	shape.vertices = { glm::vec3(nx, ny, 0.0f), glm::vec3(nx - 0.2f, ny - 0.25f, 0.0f), glm::vec3(nx + 0.2f, ny - 0.25f, 0.0f) };
	updateShape(shape);
	shapes.push_back(shape);
}

// �簢�� �׸���
void AddRect(float nx, float ny)
{
	if (shapes.size() >= 10) return; // �ִ� 10�� ����
	Shape shape;
	shape.type = ShapeType::Rect;
	shape.color = glm::vec4(1.0, 0.0, 1.0, 1.0); // �����

	glm::vec3 topLeft(nx - 0.2f, ny + 0.2f, 0.0f);
	glm::vec3 bottomLeft(nx - 0.2f, ny - 0.2f, 0.0f);
	glm::vec3 bottomRight(nx + 0.2f, ny - 0.2f, 0.0f);
	glm::vec3 topRight(nx + 0.2f, ny + 0.2f, 0.0f);
	shape.vertices = {
		topLeft, bottomLeft, bottomRight,
		bottomRight, topRight, topLeft
	};

	updateShape(shape);
	shapes.push_back(shape);
}

// ��ġ�� ������ �Ѱ��� �����ͷ� ����
/*
const float vertexData[] =
{
	0.5, 1.0, 0.0,   1.0, 0.0, 0.0,  // Vertex 0: Red
	0.0, 0.0, 0.0,   0.0, 1.0, 0.0,  // Vertex 1: Green
	1.0, 0.0, 0.0,   0.0, 0.0, 1.0,  // Vertex 2: Blue
};
*/

/*
void InitBuffer()
{
	// VAO ��ü ���� �� ���ε�
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);              // VBO ��ü ����
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // VBO ���ε�

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// ��ġ �Ӽ�: 0��° �Ӽ�
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 0��° �Ӽ� ���

	// ���� �Ӽ�: 1��° �Ӽ�
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // 1��° �Ӽ� ���
}
*/

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
	//--- ���α׷��Ǿտ������Ұ�
	// Open file for reading 
	// Return NULL on failure 
	// Seek to the end of the file 
	// Find out how many bytes into the file we are 
	// Allocate a buffer for the entire length of the file and a null terminator 
	// Go back to the beginning of the file 
	// Read the contents of the file in to the buffer 
	// Close the file 
	// Null terminator 
	// Return the buffer 
}

// ���콺 Ŭ���� ��ǥ ����ȭ
void PixelTrans(int px, int py, float& nx, float& ny)
{
	nx = 2.0f * px / width - 1.0f;
	ny = -2.0f * py / height + 1.0f;
}

// �浹 üũ(������ ������ -1, ������ �� �ε��� ��ȯ)
int IsEmpty(float nx, float ny)
{
	for (int i = 0; i < shapes.size(); i++)
	{

	}
	return -1;
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			PixelTrans(x, y, mouseX, mouseY);
		}
	}
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'p':
		AddPoint(mouseX, mouseY);
		glutPostRedisplay();
		break;
	case 'l':
		AddLine(mouseX, mouseY);
		glutPostRedisplay();
		break;
	case 't':
		AddTriangle(mouseX, mouseY);
		glutPostRedisplay();
		break;
	case 'r':
		AddRect(mouseX, mouseY);
		glutPostRedisplay();
		break;
	case 'c':
		shapes.clear();
		glutPostRedisplay();
		break;
	}
}

// ���� �Լ�
void main(int argc, char** argv)  // ������ ����ϰ� �ݹ��Լ� ����
{
	// ����������ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Tesk_08");

	// GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();

	// ���̴� �о�ͼ� ���̴� ���α׷� �����: ����� ���� �Լ� ȣ��
	make_vertexShaders();                     // ���ؽ� ���̴� �����
	make_fragmentShaders();                   // �����׸�Ʈ ���̴� �����
	shaderProgramID = make_shaderProgram();   // ���̴����α׷������

	// InitBuffer();  // ���� ���� �� �ʱ�ȭ
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

// ���ؽ� ���̴� ��ü �����
void make_vertexShaders()
{
	GLchar* vertexSource;
	// ���ؽ����̴��о������ϰ��������ϱ�
	// filetobuf: ����� ���� �Լ��� �ؽ�Ʈ�� �о ���ڿ��� �����ϴ� �Լ�
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "Error: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

// �����׸�Ʈ ���̴� ��ü �����
void make_fragmentShaders()
{
	GLchar* fragmentSource;
	// �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
	fragmentSource = filetobuf("fragment.glsl");    // �����׼��̴� �о����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

// ���̴� ���α׷� ����� ���̴� ��ü �����ϱ�
GLuint make_shaderProgram()
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram();               // ���̴� ���α׷� �����

	glAttachShader(shaderID, vertexShader);     // ���̴� ���α׷��� ���̴� ��ü ���̱�
	glAttachShader(shaderID, fragmentShader);   // ���̴� ���α׷��� �����׸�Ʈ ��ü ���̱�

	glLinkProgram(shaderID);                    // ���̴� ���α׷� �����ϱ�

	glDeleteShader(vertexShader);               // ���̴� ��ü�� ���̴� ���α׷��� ���������Ƿ�, 
	glDeleteShader(fragmentShader);             // ���̴� ��ü�� �����ص���

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);  // ���̴��� �� ����Ǿ����� Ȯ��
	if (!result)
	{
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program ���� ����\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID);
	return shaderID;
}

// �׸��� �ݹ� �Լ�
GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	GLint locColor = glGetUniformLocation(shaderProgramID, "uColor");

	for (int i = 0; i < (int)shapes.size(); ++i) 
	{
		auto& s = shapes[i];
		glUniform4fv(locColor, 1, glm::value_ptr(s.color));
		glBindVertexArray(s.VAO);

		GLenum mode = GL_POINTS;
		if (s.type == ShapeType::Line)     mode = GL_LINES;
		if (s.type == ShapeType::Triangle) mode = GL_TRIANGLES;
		if (s.type == ShapeType::Rect)     mode = GL_TRIANGLES;

		if (mode == GL_POINTS) 
		{
			glEnable(GL_PROGRAM_POINT_SIZE);
			glPointSize(6.f);
		}

		glDrawArrays(mode, 0, s.vertCount());
	}
	glBindVertexArray(0);

	glutSwapBuffers();
}

//--- �ٽñ׸����ݹ��Լ�
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}
