#pragma once
#include "Common/d3dUtil.h"
#include "Shader.h"
#include "Common/UploadBuffer.h"
#include "Texture2D.h"
#include <vector>
#include <memory>
#include "Common/DescriptorHeap.h"
class Material : MObject
{
private:
	struct MatProperty
	{
		UINT id;
		ShaderVariable::Type type;
		std::shared_ptr<MObject> obj;
	};
	UINT mPropertyIndex;
	Shader* mShader;
	std::shared_ptr<UploadBuffer> mPropertyBuffer;
	std::unordered_map<UINT, UINT> propertiesKey;
	std::vector<MatProperty> propertiesValue;
	bool SetProperty(UINT id, std::shared_ptr<MObject> obj, ShaderVariable::Type type);
protected:
	virtual void Dispose();
public:
	Material(
		Shader* shader,
		std::shared_ptr<UploadBuffer> propertyBuffer,
		UINT propertyBufferIndex
	);
	void BindShaderResource(ID3D12GraphicsCommandList* commandList);
	bool SetBindlessResource(UINT id, std::shared_ptr<DescriptorHeap> targetHeap);
	bool SetTexture2D(UINT id, std::shared_ptr<Texture2D> targetTex);
	void RemoveProperty(UINT key);
	
};