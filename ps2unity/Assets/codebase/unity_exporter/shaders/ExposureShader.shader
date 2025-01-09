Shader"Hidden/ExposureShader"
{
	Properties
	{
		_InputTex("InputTex", 2D) = "White" {}
		_RelFStops("Exposure", Float) = 0
	}
	SubShader
	{
Cull Off
		Tags { "RenderType" = "Opaque" }
		Pass
		{
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag

#include "UnityCG.cginc"

struct appdata
{
    float4 vertex : POSITION;
    float2 uv0 : TEXCOORD0;
};

struct v2f
{
    float4 vertex : SV_POSITION;
    float2 uv0 : TEXCOORD0;
};

uniform sampler2D _InputTex;
uniform float _RelFStops;
uniform float _IsLinear;

v2f vert(appdata v)
{
    v2f o;
    float2 inTexCoord = v.uv0;
    float2 screenPosTexCoord = float2(inTexCoord.x - 0.5f, -inTexCoord.y + 0.5f) * 2;
    o.vertex = float4(screenPosTexCoord, 0, 1);
    o.uv0 = v.uv0;

    return o;
}

float3 exposure(float3 color, float relative_fstop)
{
    return color * pow(2.0, relative_fstop);
}

inline half3 decodeLightmapRGBM(half4 data, half4 decodeInstructions)
{
    // If Linear mode is not supported we can skip exponent part
    if (_IsLinear > 1)
    {
        return (decodeInstructions.x * pow(data.a, decodeInstructions.y)) * data.rgb;
    }
    else
    {
        //return (decodeInstructions.x * data.a) * sqrt(data.rgb); // force
        return (decodeInstructions.x * data.a) * data.rgb;
    }
    
    
    // linear
}

float4 frag(v2f i) : SV_Target
{
    float4 tex = tex2D(_InputTex, i.uv0);
    //return float4(tex.rgb, 1);
    
    float3 lightmap = decodeLightmapRGBM(tex, half4(1, 1, 1, 1));
    float3 exposed = exposure(lightmap, _RelFStops);

    if (_IsLinear > 1)
    {
					            // power twice on the preview window,
					            // (Unity would convert this to gamma on the GUI part)
        return float4(pow(pow(exposed, 1 / 2.2), 1 / 2.2), 1);
    }
    else if (_IsLinear > 0)
    {
        return float4(pow(exposed, 1 / 2.2), 1);
    }
    else
    {
        return float4(exposed, 1);
    }
}
			    ENDCG
		    }
	    }
}
