#include "Shader.h"
#include "../Singleton/ShaderID.h"
#include "Texture2D.h"
#include "../Common/DescriptorHeap.h"
#include "UploadBuffer.h"
using namespace std;
using Microsoft::WRL::ComPtr;
Shader::~Shader()
{

}

void Shader::BindRootSignature(ID3D12GraphicsCommandList* commandList)
{
	commandList->SetGraphicsRootSignature(mRootSignature.Get());
}

void Shader::GetPassPSODesc(UINT pass, D3D12_GRAPHICS_PIPELINE_STATE_DESC* targetPSO)
{
	Pass& p = allPasses[pass];
	targetPSO->VS = 
	{
		reinterpret_cast<BYTE*>(p.vsShader->GetBufferPointer()),
		p.vsShader->GetBufferSize()
	};
	targetPSO->PS =
	{
		reinterpret_cast<BYTE*>(p.psShader->GetBufferPointer()),
		p.psShader->GetBufferSize()
	};
	targetPSO->BlendState = p.blendState;
	targetPSO->RasterizerState = p.rasterizeState;
	targetPSO->pRootSignature = mRootSignature.Get();
	targetPSO->DepthStencilState = p.depthStencilState;
}

Shader::Shader(
	std::vector<Pass> passPaths,
	std::vector<ShaderVariable> allShaderVariables,
	ID3D12Device* device
)
{
	//Create Pass
	allPasses.reserve(passPaths.size());
	for (int i = 0; i < passPaths.size(); ++i)
	{
		Pass& p = passPaths[i];
		if(p.vsShader == nullptr)
			p.vsShader = d3dUtil::CompileShader(p.filePath, nullptr, p.vertex, "vs_5_1");
		if(p.psShader == nullptr)
			p.psShader = d3dUtil::CompileShader(p.filePath, nullptr, p.fragment, "ps_5_1");
		allPasses.push_back(std::move(p));
	}
	mVariablesDict.reserve(allShaderVariables.size() + 2);
	mVariablesVector.reserve(allShaderVariables.size());
	for (int i = 0; i < allShaderVariables.size(); ++i)
	{
		ShaderVariable& variable = allShaderVariables[i];
		mVariablesDict[ShaderID::PropertyToID(variable.name)] = i;
		mVariablesVector.push_back(variable);
	}

	//texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 2, 1);
	vector<CD3DX12_ROOT_PARAMETER> allParameter;
	auto staticSamplers = d3dUtil::GetStaticSamplers();
	allParameter.reserve(VariableLength());
	std::vector< CD3DX12_DESCRIPTOR_RANGE> allTexTable;
	IterateVariables([&](ShaderVariable& var) -> void {
		if(var.type == ShaderVariable::Type::BindlessTexture)
		{
			CD3DX12_DESCRIPTOR_RANGE texTable;
			texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, var.tableSize, var.registerPos, var.space);
			allTexTable.push_back(texTable);
		}
	});
	UINT offset = 0;
	IterateVariables([&](ShaderVariable& var) -> void
	{
		CD3DX12_ROOT_PARAMETER slotRootParameter;
		switch (var.type)
		{
		case ShaderVariable::Type::Texture2D:
			slotRootParameter.InitAsShaderResourceView(var.registerPos, var.space);
			break;
		case ShaderVariable::Type::BindlessTexture:
			slotRootParameter.InitAsDescriptorTable(1, allTexTable.data() + offset);
			offset++;
			break;
		case ShaderVariable::Type::ConstantBuffer:
			slotRootParameter.InitAsConstantBufferView(var.registerPos, var.space);
			break;
		case ShaderVariable::Type::StructuredBuffer:
			slotRootParameter.InitAsShaderResourceView(var.registerPos, var.space);
			break;
		}
		allParameter.push_back(slotRootParameter);
	});
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(allParameter.size(), allParameter.data(),
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}

void Shader::SetResource(ID3D12GraphicsCommandList* commandList, UINT id, std::shared_ptr<MObject> targetObj, UINT indexOffset)
{
	auto&& ite = mVariablesDict.find(id);
	if (ite == mVariablesDict.end()) return;
	UINT rootSigPos = ite->second;
	ShaderVariable& var = mVariablesVector[rootSigPos];
	std::shared_ptr<UploadBuffer> uploadBufferPtr = reinterpret_pointer_cast<UploadBuffer, MObject>(targetObj);
	switch (var.type)
	{
	case ShaderVariable::Type::Texture2D:
		commandList->SetGraphicsRootShaderResourceView(
			rootSigPos, 
			reinterpret_pointer_cast<Texture2D, MObject>(targetObj)->GetResource()->GetGPUVirtualAddress());
		break;
	case ShaderVariable::Type::BindlessTexture:
		commandList->SetGraphicsRootDescriptorTable(
			rootSigPos,
			reinterpret_pointer_cast<DescriptorHeap, MObject>(targetObj)->hGPU(indexOffset)
		);
		break;
	case ShaderVariable::Type::ConstantBuffer:
		commandList->SetGraphicsRootConstantBufferView(
			rootSigPos,
			uploadBufferPtr->Resource()->GetGPUVirtualAddress() + indexOffset * uploadBufferPtr->GetAlignedStride()
		);
		break;
	case ShaderVariable::Type::StructuredBuffer:
		//TODO
		break;
	}
}

ShaderVariable Shader::GetVariable(std::string name)
{
	return mVariablesVector[mVariablesDict[ShaderID::PropertyToID(name)]];
}

ShaderVariable Shader::GetVariable(UINT id)
{
	return mVariablesVector[mVariablesDict[id]];
}

bool Shader::TryGetShaderVariable(UINT id, ShaderVariable& targetVar)
{
	auto&& ite = mVariablesDict.find(id);
	if (ite == mVariablesDict.end()) return false;
	targetVar = mVariablesVector[ite->second];
	return true;
}
/*

*/
