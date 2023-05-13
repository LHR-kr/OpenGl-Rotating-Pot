#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;

out vec3 fragmentColor;
out vec3 positionWorldspace;
out vec3 normalViewspace;
out vec3 eyeDirectionViewspace;
out vec3 lightDirectionViewspace;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition;

void main() {
    // 모델 좌표를 view point 좌표계로 변환 -> 원근 투영 적용
    gl_Position = MVP * vec4(vertexPosition, 1.0);

    // fragment shader에서 사용할 정보들
    positionWorldspace = (M * vec4(vertexPosition, 1.0)).xyz;
    vec3 vertexPositionViewspace = (V * M * vec4(vertexPosition, 1.0)).xyz;
    eyeDirectionViewspace = vec3(0, 0, 0) - vertexPositionViewspace;
    vec3 lightPositionViewspace = (V * vec4(LightPosition, 1.0)).xyz;
    lightDirectionViewspace = lightPositionViewspace - vertexPositionViewspace;
    normalViewspace = (V * M * vec4(vec3(1, 1, 1), 0.0)).xyz;
    
    // 프래그먼트 색깔은 흰색으로 고정
    fragmentColor = vec3(1.0,1.0,1.0);
}
