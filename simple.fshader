#version 330 core
in vec3 fragmentColor;
in vec3 positionWorldspace;
in vec3 normalViewspace;
in vec3 eyeDirectionViewspace;
in vec3 lightDirectionViewspace;

out vec4 color;

void main(){
    vec3 LightColor = vec3(1.0, 1.0, 1.0);


    // 환경광 계산
    vec3 AmbientColor = vec3(0.1, 0.1, 0.1) * fragmentColor;

    vec3 n = normalize(normalViewspace);
    vec3 l = normalize(lightDirectionViewspace);
    float cosTheta = clamp( dot( n, l ), 0, 1 );
    // 난반사 계산
    vec3 DiffuseColor = fragmentColor * LightColor * cosTheta;
	
    vec3 v = normalize(eyeDirectionViewspace);
    vec3 R = reflect(-l, n);
    float cosAlpha = clamp( dot( v, R ), 0, 1 );
    // 정반사 계산
    vec3 SpecularColor = vec3(0.3, 0.3, 0.3) * fragmentColor * LightColor * pow(cosAlpha,5);
    
    // 환경광, 난반사, 정반사의 값들을 이용하여 최종 색상 계산
    color = vec4(AmbientColor + DiffuseColor + SpecularColor, 1.0);
}
