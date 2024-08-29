#version 330 core

// 입력할 vertex attribute (위치값 데이터)의 location 을 0으로 받음.
layout(location = 0) in vec3 aPos;

void main() {
  // 다음 파이프라인으로 넘길 버텍스 쉐이더의 출력값을 gl_Position 에 할당
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
