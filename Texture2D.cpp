#include "Texture2D.h"

Texture2D::Texture2D(
	ID3D12GraphicsCommandList* commandList,
	ID3D12Device* device,
	std::string name,
	std::wstring filePath
) : MObject()
{
	Name = name;
	Filename = filePath;
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(device,
		commandList, Filename.c_str(),
		Resource, UploadHeap));
}
void Texture2D::GetResourceViewDescriptor(D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc)
{
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = Resource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = Resource->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
}

Texture2D::~Texture2D()
{
	Dispose();
 }