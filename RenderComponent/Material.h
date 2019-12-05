#pragma once
#include "../Common/d3dUtil.h"
#include "Shader.h"
#include "UploadBuffer.h"
#include "Texture2D.h"
#include <vector>
#include <memory>
#include "../Common/DescriptorHeap.h"
class Material : public MObject
{
private:
	struct MatProperty
	{
		UINT id;
		ShaderVariable::Type type;
		std::shared_ptr<MObject> obj;
		UINT indexOffset;
	};
	UINT mPropertyIndex;
	Shader* mShader;
	std::shared_ptr<UploadBuffer> mPropertyBuffer;
	std::unordered_map<UINT, UINT> propertiesKey;
	std::vector<MatProperty> propertiesValue;
	std::shared_ptr<DescriptorHeap> shaderResourceHeap;
	bool SetProperty(UINT id, std::shared_ptr<MObject> obj, ShaderVariable::Type type, UINT offsetIndex);
protected:
	virtual void Dispose();
public:
	Material(
		Shader* shader,
		std::shared_ptr<UploadBuffer> propertyBuffer,
		UINT propertyBufferIndex,
		std::shared_ptr<DescriptorHeap> srvHeap
	);
	void BindShaderResource(ID3D12GraphicsCommandList* commandList);
	bool SetBindlessResource(UINT id, UINT offsetIndex);
	bool SetTexture2D(UINT id, std::shared_ptr<Texture2D> targetTex);
	void RemoveProperty(UINT key);
	
};