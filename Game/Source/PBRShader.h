#pragma once
#include "Shader.h"

class PBRShader : public Shader
{
public:
	PBRShader(ID3D11Device* device);
	~PBRShader() override = default;

	// 開始処理
	void Begin(const RenderContext& rc) override;

	// 更新処理
	void Update(const RenderContext& rc, const Model::Mesh& mesh) override;

	// 終了処理
	void End(const RenderContext& rc) override;

private:
    struct CbMesh
    {
        DirectX::XMFLOAT4 materialColor;
        DirectX::XMFLOAT3 emissiveColor;
        float metalness;
        float roughness;
        float adjustMetalness; // 福井先生にいらないって言われたけど消すとめんどいことなるから残してる
        float adjustRoughness;
        float oclusionStrength;
        float alphaCutoff;

        int hasBaseColorTexture;
        int hasEmissiveTexture;
        int hasMetalnessRoughnessTexture;
        int hasOcclusionTexture;
        int hasNormalTexture;

        float pad[2];
    };
    CbMesh cbMesh;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>        vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>            meshConstantBuffer;
};