#version 330 core
layout(location=0) in vec3 aPos;
uniform vec2 uTranslate;   // Ű �̵� ��Ÿ�� ���� �ݿ��Ϸ��� (���õ� ������ ��Ÿ ���� �� CPU�� verts ������ �� ����)
void main(){
    gl_Position = vec4(aPos, 1.0);
    gl_PointSize = 6.0;    // GL_PROGRAM_POINT_SIZE ��� ��
}