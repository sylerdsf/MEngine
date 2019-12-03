#include "ShaderID.h"
std::unordered_map<std::string, unsigned int> ShaderID::allShaderIDs;
unsigned int ShaderID::currentCount = 0;
unsigned int ShaderID::mPerCameraBuffer = 0;
unsigned int ShaderID::mPerMaterialBuffer = 0;
unsigned int ShaderID::mPerObjectBuffer = 0;
unsigned int ShaderID::PropertyToID(std::string str)
{
	auto&& ite = allShaderIDs.find(str);
	if (ite == allShaderIDs.end())
	{
		unsigned int value = currentCount;
		allShaderIDs[str] = currentCount;
		++currentCount;
		return value;
	}
	else
	{
		return ite->second;
	}
}

void ShaderID::Init()
{
	allShaderIDs.reserve(INIT_CAPACITY);
	mPerCameraBuffer = PropertyToID("Per_Camera_Buffer");
	mPerMaterialBuffer = PropertyToID("Per_Material_Buffer");
	mPerObjectBuffer = PropertyToID("Per_Object_Buffer");
}