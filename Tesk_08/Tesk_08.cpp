//--- �ʿ���������ϼ���
#define _CRT_SECURE_NO_WARNINGS 
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <glew.h>
#include <freeglut.h>
#include <freeglut_ext.h> 
//--- �Ʒ�5���Լ��»���������Լ���
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
//--- �ʿ��Ѻ�������
GLint width, height;
GLuint shaderProgramID; // ���̴� ���α׷� �̸�
GLuint vertexShader;    // ���ؽ� ���̴� ��ü
GLuint fragmentShader;  // �����׸�Ʈ ���̴� ��ü

GLuint VAO, VBO;

// ��ġ�� ������ �Ѱ��� �����ͷ� ����
const float vertexData[] =
{
	0.5, 1.0, 0.0,   1.0, 0.0, 0.0,  // Vertex 0: Red
	0.0, 0.0, 0.0,   0.0, 1.0, 0.0,  // Vertex 1: Green
	1.0, 0.0, 0.0,   0.0, 0.0, 1.0,  // Vertex 2: Blue
};

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

// ���� �Լ�
void main(int argc, char** argv)  // ������ ����ϰ� �ݹ��Լ� ����
{
	width = 800;
	height = 600;
	// ����������ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Test_glsl");

	// GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();

	// ���̴� �о�ͼ� ���̴� ���α׷� �����: ����� ���� �Լ� ȣ��
	make_vertexShaders();                     // ���ؽ� ���̴� �����
	make_fragmentShaders();                   // �����׸�Ʈ ���̴� �����
	shaderProgramID = make_shaderProgram();   // ���̴����α׷������

	InitBuffer();  // ���� ���� �� �ʱ�ȭ

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
	GLfloat rColor, gColor, bColor;
	rColor = gColor = 1.0;
	bColor = 1.0;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glPointSize(5.0);
	glDrawArrays(GL_TRIANGLES, 0, 3);   // ���� �׸���(������ ���� �ε���, �������� �ε��� ����)
	glutSwapBuffers();
}

//--- �ٽñ׸����ݹ��Լ�
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}
