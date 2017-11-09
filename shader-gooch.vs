uniform vec4 inColor;
varying vec4 Color;

uniform vec4 lightPos;
uniform vec4 eyePos;
uniform float diffuseWarm;
uniform float diffuseCool;

void main() {
    vec3 p = vec3(gl_ModelViewMatrix * gl_Vertex);// transformed point to world space
    vec3 l = normalize(vec3(lightPos) - p);// vector to light source
    vec3 v = normalize(vec3(eyePos) - p);// vector to the eye
    vec3 n = normalize(gl_Normal);// transformed n
    vec3 n2 = normalize(n);
    vec3 l2 = normalize(l);
    vec3 v2 = normalize(v);
    vec3 r  = normalize(reflect(-l, n));
    vec3 r2 = normalize(r);

    float NdotL = (dot(l, n) + 1.0) * 0.5;
    const vec3  warmColor   = vec3(0.6, 0.6, 0.0);
    const vec3  coolColor   = vec3(0.0, 0.0, 0.6);

    vec3 surfaceColor = inColor.rgb;
    vec3 kCool  = min(coolColor + diffuseCool * surfaceColor, 1.0);
    vec3 kWarm  = min(warmColor + diffuseWarm * surfaceColor, 1.0);
    vec3 kFinal = mix(kCool, kWarm, NdotL);
    
    float spec  = pow(max(dot(r2, v2), 0.0), 32.0);

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    Color = vec4(min(kFinal + spec, 1.0), 1.0);
}
