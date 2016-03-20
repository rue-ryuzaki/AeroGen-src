uniform vec4 inColor;
varying vec4 Color;

uniform	vec4 lightPos;
uniform	vec4 eyePos;
uniform vec4 specColor;
uniform float specPower;
uniform float rimPower;
uniform float bias;

void main() {
    vec3 p = vec3(gl_ModelViewMatrix * gl_Vertex);// transformed point to world space
    vec3 l = normalize(vec3(lightPos) - p);// vector to light source
    vec3 v = normalize(vec3(eyePos) - p);// vector to the eye
    vec3 h = normalize(l + v);
    vec3 n = normalize(gl_Normal);// transformed n
    vec3 n2 = normalize(n);
    vec3 l2 = normalize(l);
    vec3 h2 = normalize(h);
    vec3 v2 = normalize(v);

    vec4  diff = inColor * max(dot(n2, l2), 0.0);
    vec4  spec = specColor * pow(max(dot(n2, h2), 0.0), specPower);
    float rim  = pow(1.0 + bias - max(dot(n2, v2), 0.0), rimPower);

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    Color = diff + rim * vec4(0.5, 0.0, 0.2, 1.0) + spec * specColor;
}
