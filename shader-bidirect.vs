uniform vec4 inColor;
varying vec4 Color;

uniform vec4 lightPos;
uniform vec4 eyePos;
uniform vec4 color2;

void main() {
    vec3 p = vec3(gl_ModelViewMatrix * gl_Vertex);// transformed point to world space
    vec3 l = normalize(vec3(lightPos) - p);// vector to light source
    vec3 n = normalize(gl_Normal);// transformed n
    vec3 n2 = normalize(n);
    vec3 l2 = normalize(l);

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    Color = inColor * max(dot(n2, l2), 0.0) + color2 * max(dot(n2, -l2), 0.0);
}
