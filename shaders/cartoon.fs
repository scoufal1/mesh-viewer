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
};
uniform MaterialInfo Material;
const int levels = 3;
const float scaleFactor = 1.0 / levels;
layout( location = 0 ) out vec4 FragColor;
vec3 toonShade( )
{
    vec3 s = normalize( Light.Position.xyz - Position.xyz );
    float cosine = max( 0.0, dot( s, Normal ) );
    vec3 diffuse = Material.Kd * floor( cosine * levels ) *
    scaleFactor;
    return Light.Intensity * (Material.Ka + diffuse);
}
void main() {
    FragColor = vec4(toonShade(), 1.0);
}