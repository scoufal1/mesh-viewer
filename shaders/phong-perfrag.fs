#version 400
in vec3 Position;
in vec3 Normal;
struct LightInfo {
    vec4 Position;
    vec3 Intensity;
};
uniform LightInfo Light;
struct MaterialInfo {
    vec3 Kd; // Diffuse reflectivity
    vec3 Ka; // Ambient reflectivity
    vec3 Ks; // Specular reflectivity
    float Shininess; // Specular shininess factor
};
uniform MaterialInfo Material;
layout( location = 0 ) out vec4 FragColor;
vec3 ads( )
{
    vec3 n = normalize( Normal );
    vec3 s = normalize( vec3(Light.Position) - Position );
    vec3 v = normalize(vec3(-Position));
    vec3 r = reflect( -s, n );
    return
    Light.Intensity *
    ( Material.Ka +
    Material.Kd * max( dot(s, n), 0.0 ) +
    Material.Ks * pow( max( dot(r,v), 0.0 ), Material.Shininess ) );
}
void main() {
    FragColor = vec4(ads(), 1.0);
}