uniform float smooth;
uniform float metal;
uniform float transp;

uniform vec4 inColor;
varying vec4 Color;

uniform	vec4 lightPos;
uniform	vec4 eyePos;

float fresnel(float x, float kf) {
    float dx = x - kf;
    float d1 = 1.0 - kf;
    float kf2 = kf * kf;
	
    return (1.0 / (dx * dx) - 1.0 / kf2) / (1.0 / (d1 * d1) - 1.0 / kf2);
    //return 1.0;
}

float shadow(float x, float ks) {
    float dx = x - ks;
    float d1 = 1.0 - ks;
    float ks2 = ks * ks;
    //return (1.0 / (dx * dx) - 1.0 / ks2) / (1.0 / (d1 * d1) - 1.0 / ks2);
    return 1.0;
}

void main() {
    vec3 p = vec3(gl_ModelViewMatrix * gl_Vertex);// transformed point to world space
    vec3 l = normalize(vec3(lightPos) - p);// vector to light source
    vec3 v = normalize(vec3(eyePos) - p);// vector to the eye
    vec3 h = normalize(l + v);
    vec3 n = normalize(gl_Normal);// transformed n
    vec3 n2 = normalize(n);
    vec3 l2 = normalize(l);
    vec3 v2 = normalize(v);
    vec3 h2 = reflect(l2, n2);
    const vec4  specColor = vec4(1.0, 1.0, 0.0, 1.0);
    const float	k  = 0.1;
    const float kf = 1.12;
    const float ks = 1.01;

    float nl = dot(n2, l2);
    float nv = dot(n2, v2);
    float hv = dot(h2, v2);
    float f  = fresnel(nl, kf);
    float s3 = smooth * smooth * smooth;
    // diffuse term
    float d  = (1.0 - metal * smooth);
    float Rd = (1.0 - s3) * (1.0 - transp);
    vec4 diff = nl * d * Rd * inColor;

    // inputs into the specular term
    float r = (1.0 - transp) - Rd;
    float j = f * shadow(nl, ks) * shadow(nv, ks);
    float reflect = min(1.0, r + j * (r + k));
    vec4 C1 = vec4(1.0);
    vec4 Cs = C1 + metal * (1.0 - f) * (inColor - C1); 
    vec4 spec = Cs * reflect;

    spec *= pow(-hv, 3.0 / (1.0 - smooth));
    diff = max(vec4(0.0), diff);
    spec = max(vec4(0.0), spec);

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    Color = diff + spec * specColor;
}
