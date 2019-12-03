#include "Material.h"
#include "ShaderID.h"
using namespace std;
using Microsoft::WRL::ComPtr;
Material::Material(
	Shader* shader,
	std::shared_ptr<UploadBuffer> propertyBuffer,
	UINT propertyBufferIndex
) : MObject()
{
	mPropertyIndex = propertyBufferIndex;
	mShader = shader;
	mPropertyBuffer = propertyBuffer;
}
void Material::Dispose()
{
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
		mShader->SetResource(commandList, ShaderID::GetPerMaterialBufferID(), reinterpret_pointer_cast<MObject, UploadBuffer>(mPropertyBuffer), mPropertyIndex);
	}
	for (int i = 0; i < propertiesValue.size(); ++i)
	{
		MatProperty& prop = propertiesValue[i];
		mShader->SetResource(commandList, prop.id, prop.obj, 0);
	}

}

bool Material::SetProperty(UINT id, std::shared_ptr<MObject> obj, ShaderVariable::Type type)
{
	ShaderVariable var;
	if (mShader->TryGetShaderVariable(id, var) && var.type == type)
	{
		RemoveProperty(id);
		propertiesKey[id] = propertiesValue.size();
		char propertyPtr[sizeof(MatProperty)];
		MatProperty* p = reinterpret_cast<MatProperty*>(propertyPtr);
		p->type = type;
		p->id = id;
		p->obj = obj;
		propertiesValue.push_back(std::move(*p));
		return true;
	}
	return false;
}

bool Material::SetBindlessResource(UINT id, std::shared_ptr<DescriptorHeap> targetHeap)
{
	std::shared_ptr<MObject> obj = reinterpret_pointer_cast<MObject, DescriptorHeap>(targetHeap);
	return SetProperty(id, obj, ShaderVariable::Type::BindlessTexture);
}

bool Material::SetTexture2D(UINT id, std::shared_ptr<Texture2D> targetTex)
{
	std::shared_ptr<MObject> obj = reinterpret_pointer_cast<MObject, Texture2D>(targetTex);
	return SetProperty(id, obj, ShaderVariable::Type::Texture2D);
}

void Material::RemoveProperty(UINT key)
{
	auto&& ite = propertiesKey.find(key);
	if (ite == propertiesKey.end()) return;
	propertiesKey.erase(key);
	if (ite->second != propertiesValue.size() - 1) {
		
		memcpy(&propertiesValue[ite->second], &propertiesValue[propertiesValue.size() - 1], sizeof(MatProperty));
		propertiesValue[propertiesValue.size() - 1].obj = nullptr;
		propertiesValue.erase(propertiesValue.end() - 1);
		MatProperty* p = &propertiesValue[ite->second];
		propertiesKey[p->id] = ite->second;
	}
	else
	{
		propertiesValue.erase(propertiesValue.end() - 1);
		
	}
}