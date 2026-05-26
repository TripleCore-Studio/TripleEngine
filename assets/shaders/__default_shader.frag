#version 330 core

in vec2 v_UV;
in vec3 v_Color;
in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec3      u_CameraPos;
uniform float     u_Time;
uniform sampler2D u_AlbedoTex;

out vec4 o_FragColor;

struct PointLight {
    vec3  pos;
    vec3  color;
    float intensity;
    float radius;
};

const int LIGHT_COUNT = 2;

PointLight lights[LIGHT_COUNT] = PointLight[LIGHT_COUNT](
    // Оружие — ближние
    // PointLight(u_CameraPos + vec3( 1.0,  0.2,  0.0), vec3(0.2, 0.5, 1.0), 10.0,  0.0),
    PointLight(u_CameraPos + vec3(0.0, -0.1,  0.0), vec3(0.8, 0.29, 0.97),  7.0, 10.0),

    PointLight(vec3(u_CameraPos.x, 1.0, u_CameraPos.z), vec3(0.7, 0.67, 0.58), 10.0, 80.0)
);

vec3 CalcPointLight(PointLight l, vec3 N, vec3 albedo)
{
    vec3  toLight = l.pos - v_FragPos;
    float dist    = length(toLight);
    vec3  L       = toLight / dist;

    float att = pow(clamp(1.0 - dist / l.radius, 0.0, 1.0), 2.0);

    float diff    = max(dot(N, L), 0.0);

    vec3  V       = normalize(u_CameraPos - v_FragPos);
    vec3  H       = normalize(L + V);
    float spec    = pow(max(dot(N, H), 0.0), 32.0 * 2.0f) * 1.5;

    return (diff + spec) * l.color * l.intensity * att * albedo;
}

void main()
{
    vec4 albedoSample = texture(u_AlbedoTex, v_UV);

    vec3 albedo = (albedoSample.a < 0.1) ? v_Color : albedoSample.rgb;

    albedo = pow(albedo, vec3(2.2));

    vec3 N = normalize(v_Normal);

    vec3 ambientColor = vec3(0.16, 0.16, 0.27) * albedo;

    vec3 lighting = ambientColor;
    for (int i = 0; i < LIGHT_COUNT; i++)
        lighting += CalcPointLight(lights[i], N, albedo);

    lighting = lighting / (lighting + vec3(1.0));
    lighting = pow(lighting, vec3(1.0 / 2.2));

    o_FragColor = vec4(lighting, 1.0);
}