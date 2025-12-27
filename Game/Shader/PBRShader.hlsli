#include "Scene.hlsli"


struct VS_OUT
{
    float4 position : SV_Position;
    float4 w_position : POSITION;
    float3 w_normal : NORMAL;
    float4 w_tangent : TANGENT;
    float2 texcoord : TEXCOORD;
};

cbuffer MATERIAL_CONSTANT_BUFFER : register(b13)
{
    float4 materialColor; // 光をそのまま反射する反射率のデータを持ったテクスチャ
    float3 emissiveColor; // 自ら発行するデータを持ったテクスチャ
    float metalness;
    float roughness; // 質感・粗さを決定する値
    float adjustmetalness;
    float adjustroughness;
    float occlusionStrength; // 環境光が届きにくくなる遮蔽率を持ったデータテクスチャ
    float alphaCutoff; 
    
    int hasBaseColorTexture;
    int hasEmissiveTexture;
    int hasMetalnessRoughnessTexture;
    int hasOcclusionTexture;
    int hasNormalTexture;
    
    float2 pad;
};

#include "shading_functions.hlsli"
#include "physical_based_rendering_functions.hlsli"
