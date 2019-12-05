#include "Material.h"
#include "../Singleton/ShaderID.h"
using namespace std;
using Microsoft::WRL::ComPtr;
Material::Material(
	Shader* shader,
	std::shared_ptr<UploadBuffer> propertyBuffer,
	UINT propertyBufferIndex,
	std::shared_ptr<DescriptorHeap> srvHeap
) : MObject()
{
	shaderResourceHeap = srvHeap;
	mPropertyIndex = propertyBufferIndex;
	mShader = shader;
	mPropertyBuffer = propertyBuffer;
}
void Material::Dispose()
{
	shaderResourceHeap = nullptr;
	mPropertyBuffer = nullptr;
	mShader = nullptr;
	for (int i = 0; i < propertiesValue.size(); ++i)
	{
		propertiesValue[i].obj = nullptr;
	}
}

void Material::BindShaderResource(ID3D12GraphicsCommandList* commandList)
{
	if (mPropertyBuffer != nullptr)
	{
		mShader->SetResource(commandList, 
			ShaderID::GetPerMaterialBufferID(), 
			reinterpret_pointer_cast<MObject, UploadBuffer>(mPropertyBuffer),
			mPropertyIndex);
	}
	for (int i = 0; i < propertiesValue.size(); ++i)
	{
		MatProperty& prop = propertiesValue[i];
		mShader->SetResource(commandList, prop.id, prop.obj, prop.indexOffset);
	}

}

bool Material::SetProperty(UINT id, std::shared_ptr<MObject> obj, ShaderVariable::Type type, UINT offsetIndex)
{
	ShaderVariable var;
	if (mShader->TryGetShaderVariable(id, var) && var.type == type)
	{
		RemoveProperty(id);
		propertiesKey[id] = propertiesValue.size();
		MatProperty p;
		p.type = type;
		p.id = id;
		p.indexOffset = offsetIndex;
		p.obj = obj;
		propertiesValue.push_back(p);
		return true;
	}
	return false;
}

bool Material::SetBindlessResource(UINT id, UINT offsetIndex)
{
	std::shared_ptr<MObject> obj = reinterpret_pointer_cast<MObject, DescriptorHeap>(shaderResourceHeap);
	return SetProperty(id, obj, ShaderVariable::Type::BindlessTexture, offsetIndex);
}

bool Material::SetTexture2D(UINT id, std::shared_ptr<Texture2D> targetTex)
{
	std::shared_ptr<MObject> obj = reinterpret_pointer_cast<MObject, Texture2D>(targetTex);
	return SetProperty(id, obj, ShaderVariable::Type::Texture2D, 0);
}

void Material::RemoveProperty(UINT key)
{
	auto&& ite = propertiesKey.find(key);
	if (ite == propertiesKey.end()) return;
	propertiesKey.erase(key);
	if (ite->second != propertiesValue.size() - 1) {
		propertiesValue[ite->second] = propertiesValue[propertiesValue.size() - 1];
		propertiesValue[propertiesValue.size() - 1].obj = nullptr;
		propertiesValue.erase(propertiesValue.end() - 1);
		MatProperty* p = &propertiesValue[ite->second];
		propertiesKey[p->id] = ite->second;
	}
	else
	{
		propertiesValue[propertiesValue.size() - 1].obj = nullptr;
		propertiesValue.erase(propertiesValue.end() - 1);

	}
}