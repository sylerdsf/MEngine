#include "PSOContainer.h"
#include "MeshLayout.h"
std::unordered_map<PSODescriptor, Microsoft::WRL::ComPtr<ID3D12PipelineState>> PSOContainer::allPSOState;
bool PSODescriptor::operator==(const PSODescriptor& other) const
{
	if (other.shaderPass == shaderPass &&
		other.depthFormat == depthFormat &&
		other.rtCount == rtCount &&
		other.shaderPtr == shaderPtr &&
		other.meshLayoutIndex == meshLayoutIndex)
	{
		for (UINT i = 0; i < rtCount; ++i)
		{
			if (rtFormat[i] != other.rtFormat[i]) return false;
		}
		return true;
	}
	return false;
}

bool PSODescriptor::operator==(const PSODescriptor&& other) const
{
	if (other.shaderPass == shaderPass &&
		other.depthFormat == depthFormat &&
		other.rtCount == rtCount &&
		other.shaderPtr == shaderPtr &&
		other.meshLayoutIndex == meshLayoutIndex)
	{
		for (UINT i = 0; i < rtCount; ++i)
		{
			if (rtFormat[i] != other.rtFormat[i]) return false;
		}
		return true;
	}
	return false;
}


ID3D12PipelineState* PSOContainer::GetState(PSODescriptor& desc, ID3D12Device* device)
{
	auto&& ite = allPSOState.find(desc);
	if (ite == allPSOState.end())
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

		//
		// PSO for opaque objects.
		//
		ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		std::vector<D3D12_INPUT_ELEMENT_DESC>* inputElement = MeshLayout::GetMeshLayoutValue(desc.meshLayoutIndex);
		opaquePsoDesc.InputLayout = { inputElement->data(), (UINT)inputElement->size() };
		desc.shaderPtr->GetPassPSODesc(desc.shaderPass, &opaquePsoDesc);
		opaquePsoDesc.SampleMask = UINT_MAX;
		opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		opaquePsoDesc.NumRenderTargets = desc.rtCount;
		for (UINT i = 0; i < desc.rtCount; ++i)
		{
			opaquePsoDesc.RTVFormats[i] = desc.rtFormat[i];
		}
		opaquePsoDesc.SampleDesc.Count = 1;
		opaquePsoDesc.SampleDesc.Quality = 0;
		opaquePsoDesc.DSVFormat = desc.depthFormat;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> result = nullptr;
		ThrowIfFailed(device->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(result.GetAddressOf())));
		allPSOState[desc] = result;
		return result.Get();
	}
	return ite->second.Get();
}