uniform vec4 inColor;
varying vec4 Color;

uniform vec4 lightPos;
uniform vec4 eyePos;
uniform float k;

void main() {
    vec3 p = vec3(gl_ModelViewMatrix * gl_Vertex);// transformed point to world space
    vec3 l = normalize(vec3(lightPos) - p);// vector to light source
    vec3 v = normalize(vec3(eyePos) - p);// vector to the eye
    vec3 n = normalize(gl_Normal);// transformed n
    
    vec3 n2 = normalize(n);
    vec3 l2 = normalize(l);
    vec3 v2 = normalize(v);

    float d1 = pow(max(dot(n2, l2), 0.0), 1.0 + k);
    float d2 = pow(1.0 - dot(n2, v2), 1.0 - k);

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    Color = inColor * d1 * d2;
}
