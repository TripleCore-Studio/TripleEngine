#version 330 core

in vec2 v_UV;
in vec3 v_Color;
in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec3      u_CameraPos;
uniform float     u_Time;
uniform sampler2D u_AlbedoTex;
uniform vec3 u_AmbientColor;

out vec4 o_FragColor;

struct SunLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct CameraLight {
    vec3 color;
    float intensity;
    float radius;
};

uniform SunLight u_Sun;
uniform CameraLight u_CamLight;

vec3 CalcPointLight(CameraLight l, vec3 N, vec3 albedo)
{
    vec3  lightPos = u_CameraPos + vec3(0.0, -0.1, 0.0);
    vec3  toLight = lightPos - v_FragPos;
    float dist    = length(toLight);
    vec3  L       = toLight / dist;

    float att = pow(clamp(1.0 - dist / l.radius, 0.0, 1.0), 2.0);

    float diff    = max(dot(N, L), 0.0);

    vec3  V       = normalize(u_CameraPos - v_FragPos);
    vec3  H       = normalize(L + V);
    float spec    = pow(max(dot(N, H), 0.0), 32.0 * 2.0f) * 1.5;

    return (diff + spec) * l.color * l.intensity * att * albedo;
}

vec3 CalcDirectionalLight(SunLight sun, vec3 N, vec3 albedo)
{
    vec3 L = normalize(sun.direction);

    float diff = max(dot(N, L), 0.0);

    vec3 V    = normalize(u_CameraPos - v_FragPos);
    vec3 H    = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), 64.0) * 1.5;

    return (diff + spec) * sun.color * sun.intensity * albedo;
}

void main()
{
    vec4 albedoSample = texture(u_AlbedoTex, v_UV);
    vec3 albedo = (albedoSample.a < 0.1) ? v_Color : albedoSample.rgb;
    albedo = pow(albedo, vec3(2.2));

    vec3 N = normalize(v_Normal);

    float ambientStrength = mix(0.02, 0.16, clamp(u_Sun.intensity, 0.0, 1.0));
    vec3  ambientColor    = vec3(ambientStrength, ambientStrength, ambientStrength * 1.7) * u_AmbientColor * albedo;

    vec3 lighting = ambientColor;
    lighting += CalcDirectionalLight(u_Sun, N, albedo);
    lighting += CalcPointLight(u_CamLight, N, albedo);

    lighting = lighting / (lighting + vec3(1.0));
    lighting = pow(lighting, vec3(1.0 / 2.2));

    o_FragColor = vec4(lighting, 1.0);
}