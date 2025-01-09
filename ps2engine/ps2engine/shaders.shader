// Define constants for the light type
#define SPOT 0
#define DIRECTIONAL 1
#define POINT 2

// Define the light struct with the properties you need
struct Light
{
    float4 position; // light position, for point and spot lights
    float4 direction; // light direction, for directional and spot lights
    float4 color; // light color
    int type; // light type: directional, point, spot
    float range; // light range, for point and spot lights
    float spotAngle; // spot angle in degrees, for spot lights
    float intensity; // light intensity
};

// Create an array of lights
cbuffer ShaderData
{
    matrix World;
    matrix View;
    matrix Projection;
    float3 CameraPos;
    int ActiveLights;
    
    Light Lights[10];
    
    // Define material properties
    float4 materialAmbientColor;
    float4 materialDiffuseColor;
    float3 materialSpecularColor;
    float materialShininess;
    
    float hasTexture;
};

Texture2D texDiffuse : register(t0);
SamplerState samplerDiffuse : register(s0);

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 viewDir : VIEWDIR;
    float4 uv : TEXCOORD0;
};


float3 CalculateLighting(float3 normal, float3 viewDir)
{
    float3 lighting = float3(0, 0, 0);

    for (int i = 0; i < ActiveLights; i++)
    {
        Light light = Lights[i];
        float3 lightDir;
        float attenuation = 1.0;
        bool skip = false;
        
        switch (light.type)
        {
            case DIRECTIONAL:{
                    lightDir = normalize(-light.direction.xyz);
                    break;
                }

            //case POINT:
            //    lightDir = normalize(light.position - input.Position);
            //    attenuation = 1.0 / max(0.001, distance(light.position, input.Position) / light.range);
            //    break;

            //case SPOT:
            //    lightDir = normalize(light.position - input.Position);
            //    float angle = dot(lightDir, normalize(-light.direction));
            //    if (angle < cos(radians(light.spotAngle)))
            //    {
            //        skip = true;
            //    }
            //    break;
        }
        
        if (skip)
        {
            continue;
        }
        
        // Ambient
        float3 ambient = materialAmbientColor.rgb * light.color.rgb * light.intensity;
        
        // Diffuse
        float ndl = max(dot(normal, lightDir), 0.0);
        float3 diffuse = materialDiffuseColor.rgb * light.color.rgb * ndl * light.intensity;

        // Specular
        float3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
        float3 specular = materialSpecularColor * light.color.rgb * spec * light.intensity;

        lighting += (ambient + diffuse + specular) * attenuation;
    }

    return lighting;
}

PixelInputType VShader(float4 position : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL)
{
    PixelInputType output;

    float4 worldPosition = mul(float4(position.xyz, 1), World);
    float4 viewPosition = mul(worldPosition, View);
    output.position = mul(viewPosition, Projection);
    
    output.normal = mul(float4(normal.xyz, 0.0), World).xyz;
    output.normal = normalize(output.normal);
    
    output.uv = uv;
    
    // Calculate the view direction
    output.viewDir = normalize(CameraPos - worldPosition.xyz);

    return output;
}

float4 PShader(PixelInputType input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 viewDir = normalize(input.viewDir);

    // Calculate lighting using Phong model
    float3 lighting = CalculateLighting(normal, viewDir);
    
    float3 color = materialDiffuseColor;
    
    float4 tex = float4(1, 1, 1, 1);
    if (hasTexture > 0.5f)
    {
        tex = texDiffuse.Sample(samplerDiffuse, float2(input.uv.x, input.uv.y));
        return float4(tex.rgb, 1.0f);
        
        color *= tex.rgb;
    }

    return float4(lighting * color, 1.0 * tex.a);
}