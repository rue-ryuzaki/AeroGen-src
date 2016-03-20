uniform vec4 inColor;
varying vec4 Color;

uniform	vec4 lightPos;
uniform	vec4 eyePos;
uniform vec4 specColor;
uniform float specPower;

void main() {
    vec3 p = vec3(gl_ModelViewMatrix * gl_Vertex);// transformed point to world space
    vec3 l = normalize(vec3(lightPos) - p);// vector to light source
    vec3 v = normalize(vec3(eyePos) - p);// vector to the eye
    vec3 h = normalize(l + v);
    vec3 n = normalize(gl_Normal);// transformed n
    vec3 t = normalize(gl_MultiTexCoord1.xyz); // transformed t
    vec3 b = normalize(gl_MultiTexCoord2.xyz); // transformed b
    vec3 n2 = normalize(n);
    vec3 t2 = normalize(t);
    vec3 b2 = normalize(b);
    vec3 l2 = normalize(l);
    vec3 h2 = normalize(h);
    vec3 v2 = normalize(v);

    const float PI = 3.1415926;
    float mx = 12.0;
    float my = 12.0;
    float ks = 0.5;
    float kd = 1.0;
    float r0 = 0.5;
    float A  = 5.0;//sqrt ((mx + 1) * (my + 1) / (8 * PI)); // 25.73;

    float nv  = max(0.0, dot(n2, v2));
    float nl  = max(0.0, dot(n2, l2));
    float nh  = max(0.0, dot(n2, h2));
    float hl  = max(0.0, dot(h2, l2));
    float t1h = dot(b2, h);
    float t2h = dot(t2, h);

    // calculate diffuse
    float rd = (28.0 / (23.0 * PI)) * (1.0 - pow(1.0 - 0.5 * nv, 5.0)) * (1.0 - pow(1.0 - 0.5 * nl, 5.0));

    // calculate specular
    float B  = pow (nh, (mx * t1h * t1h + my * t2h * t2h) / (1.0 - nh * nh));
    float F  = (r0 + (1.0 - r0) * pow(1.0 - hl, 5.0)) / (hl * max(nv, nl));
    float rs = A * B * F;

    gl_TexCoord[0] = gl_MultiTexCoord0;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    Color = nl * (inColor * kd * (1.0 - ks) * rd + specColor * ks * rs);
}
