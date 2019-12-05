#pragma once
#include "../RenderComponent/Shader.h"
#include <unordered_map>
struct PSODescriptor
{
	Shader* shaderPtr;
	UINT shaderPass;
	DXGI_FORMAT depthFormat;
	UINT rtCount;
	DXGI_FORMAT rtFormat[8];
	UINT meshLayoutIndex;
	bool operator==(const PSODescriptor& other)const;
	bool operator==(const PSODescriptor&& other)const;
};
namespace std
{
	template <>
	struct hash<PSODescriptor>
	{
		size_t operator()(const PSODescriptor& key) const
		{
			size_t value = reinterpret_cast<size_t>(key.shaderPtr);
			value += key.shaderPass;
			value += key.depthFormat;
			value += key.rtCount;
			value += key.meshLayoutIndex;
			for (UINT i = 0; i < key.rtCount; ++i)
			{
				value += key.rtFormat[i];
			}
			return value;
		}
	};
}
class PSOContainer
{
private:
	static std::unordered_map<PSODescriptor, Microsoft::WRL::ComPtr<ID3D12PipelineState>> allPSOState;
public:
	static ID3D12PipelineState* GetState(PSODescriptor& desc, ID3D12Device* device);
};