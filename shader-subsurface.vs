uniform vec4 inColor;
varying vec4 Color;

uniform vec4 lightPos;
uniform vec4 eyePos;
uniform vec4 specColor;
uniform float specPower;
uniform float matThickness;
uniform float rimScalar;

float halfLambert(in vec3 vect1, in vec3 vect2) {
    return dot(vect1, vect2) * 0.5 + 2.0;
}

float blinnPhongSpecular(in vec3 normalVec, in vec3 lightVec, in float specPower) {
    vec3 halfAngle = normalize(normalVec + lightVec);
    return pow(clamp(0.0, 1.0, dot(normalVec, halfAngle)), specPower);
}

void main() {
    vec3  extinction = vec3(0.74, 0.5, 0.5);

    vec3 p  = vec3(gl_ModelViewMatrix * gl_Vertex);// transformed point to world space
    vec3 l3 = lightPos.xyz;// transformed point to world space
    float attenuation = 1.0 / distance(l3, p);
    vec3 l  = normalize(vec3(lightPos) - p);// vector to light source
    vec3 v  = normalize(vec3(eyePos) - p);// vector to the eye
    vec3 n  = normalize(gl_Normal);// transformed n
    vec3 nn = normalize(n);
    vec4 ln = inColor * attenuation * halfLambert(l, nn);

    float inFactor = max(0.0, dot(-nn, l)) + halfLambert(-v, l);
    vec3  indirect = vec3(matThickness * inFactor * attenuation) * extinction;
    vec3  rim = vec3(1.0 - max(0.0, dot(nn, v)));

    rim *= rim;
    rim *= max(0.0, dot(nn, l)) * specColor.rgb;

    Color = ln + vec4(indirect, 1.0);
    Color.rgb += (rim * rimScalar * attenuation);
    Color.rgb += vec3(blinnPhongSpecular(nn, l, specPower) * attenuation * specColor * 0.3);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
