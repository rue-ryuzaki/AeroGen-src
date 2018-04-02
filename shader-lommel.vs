uniform vec4 inColor;
varying vec4 Color;

uniform vec4 lightPos;
uniform vec4 eyePos;

void main() {
    vec3 p = vec3(gl_ModelViewMatrix * gl_Vertex);// transformed point to world space
    vec3 l = normalize(vec3(lightPos) - p);// vector to light source
    vec3 v = normalize(vec3(eyePos) - p);// vector to the eye
    vec3 n = normalize(gl_Normal);// transformed n
    vec3 n2 = normalize(n);
    vec3 l2 = normalize(l);
    vec3 v2 = normalize(v);

    float a = max(0.0, dot(n2, l2));
    float b = max(0.0, dot(n2, v2));
    if (a + b == 0.0) {
        b = 0.5;
    }

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    Color = inColor * a / (a + b);
}
