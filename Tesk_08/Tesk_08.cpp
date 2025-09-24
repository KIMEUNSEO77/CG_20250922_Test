//--- 필요한헤더파일선언
#define _CRT_SECURE_NO_WARNINGS 
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <glew.h>
#include <freeglut.h>
#include <freeglut_ext.h> 
//--- 아래5개함수는사용자정의함수임
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
//--- 필요한변수선언
GLint width, height;
GLuint shaderProgramID; // 세이더 프로그램 이름
GLuint vertexShader;    // 버텍스 세이더 객체
GLuint fragmentShader;  // 프래그먼트 세이더 객체

GLuint VAO, VBO;

// 위치와 색상을 한개의 데이터로 저장
const float vertexData[] =
{
	0.5, 1.0, 0.0,   1.0, 0.0, 0.0,  // Vertex 0: Red
	0.0, 0.0, 0.0,   0.0, 1.0, 0.0,  // Vertex 1: Green
	1.0, 0.0, 0.0,   0.0, 0.0, 1.0,  // Vertex 2: Blue
};

void InitBuffer()
{
	// VAO 객체 생성 및 바인딩
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);              // VBO 객체 생성
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // VBO 바인딩

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// 위치 속성: 0번째 속성
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 0번째 속성 사용

	// 색상 속성: 1번째 속성
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // 1번째 속성 사용
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
	//--- 프로그램맨앞에선언할것
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

// 메인 함수
void main(int argc, char** argv)  // 윈도우 출력하고 콜백함수 설정
{
	width = 800;
	height = 600;
	// 윈도우생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Test_glsl");

	// GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	// 세이더 읽어와서 세이더 프로그램 만들기: 사용자 정의 함수 호출
	make_vertexShaders();                     // 버텍스 세이더 만들기
	make_fragmentShaders();                   // 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();   // 세이더프로그램만들기

	InitBuffer();  // 버퍼 생성 및 초기화

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

// 버텍스 세이더 객체 만들기
void make_vertexShaders()
{
	GLchar* vertexSource;
	// 버텍스세이더읽어저장하고컴파일하기
	// filetobuf: 사용자 정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
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
		std::cerr << "Error: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

// 프래그먼트 세이더 객체 만들기
void make_fragmentShaders()
{
	GLchar* fragmentSource;
	// 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = filetobuf("fragment.glsl");    // 프래그세이더 읽어오기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

// 세이더 프로그램 만들고 세이더 객체 연결하기
GLuint make_shaderProgram()
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram();               // 세이더 프로그램 만들기

	glAttachShader(shaderID, vertexShader);     // 세이더 프로그램에 세이더 객체 붙이기
	glAttachShader(shaderID, fragmentShader);   // 세이더 프로그램에 프래그먼트 객체 붙이기

	glLinkProgram(shaderID);                    // 세이더 프로그램 연결하기

	glDeleteShader(vertexShader);               // 세이더 객체를 세이더 프로그램에 연결했으므로, 
	glDeleteShader(fragmentShader);             // 세이더 객체는 삭제해도됨

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);  // 세이더가 잘 연결되었는지 확인
	if (!result)
	{
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID);
	return shaderID;
}

// 그리기 콜백 함수
GLvoid drawScene()
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = 1.0;
	bColor = 1.0;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glPointSize(5.0);
	glDrawArrays(GL_TRIANGLES, 0, 3);   // 도형 그리기(렌더링 시작 인덱스, 렌더링할 인덱스 개수)
	glutSwapBuffers();
}

//--- 다시그리기콜백함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}
