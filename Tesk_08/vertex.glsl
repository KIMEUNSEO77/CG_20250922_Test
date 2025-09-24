#version 330 core
layout(location=0) in vec3 aPos;
uniform vec2 uTranslate;   // 키 이동 델타를 직접 반영하려면 (선택된 도형만 델타 적용 시 CPU로 verts 수정이 더 간단)
void main(){
    gl_Position = vec4(aPos, 1.0);
    gl_PointSize = 6.0;    // GL_PROGRAM_POINT_SIZE 사용 시
}