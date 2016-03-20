uniform vec4 inColor;
varying vec4 Color;

uniform	vec4 lightPos;
uniform	vec4 eyePos;
uniform float a;
uniform float b;

void main() {
    vec3 p = vec3(gl_ModelViewMatrix * gl_Vertex);// transformed point to world space
    vec3 l = normalize(vec3(lightPos) - p);// vector to light source
    vec3 v = normalize(vec3(eyePos) - p);// vector to the eye
    vec3 h = normalize(l + v);
    vec3 n = normalize(gl_Normal);// transformed n
    vec3 n2 = normalize(n);
    vec3 l2 = normalize(l);
    vec3 v2 = normalize(v);
    float nl = dot(n2, l2);
    float nv = dot(n2, v2);
    vec3  lProj = normalize(l2 - n2 * nl);
    vec3  vProj = normalize(v2 - n2 * nv);
    float cx    = max(dot(lProj, vProj), 0.0);

    float cosAlpha = nl > nv ? nl : nv;
    float cosBeta  = nl > nv ? nv : nl;
    float dx       = sqrt ((1.0 - cosAlpha * cosAlpha) * (1.0 - cosBeta * cosBeta)) / cosBeta;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    Color = max (0.0, nl) * inColor * (a + b * cx * dx);
}
