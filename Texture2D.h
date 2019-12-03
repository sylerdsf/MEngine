#pragma once
#include "Common/d3dUtil.h"
#include <string>
#include <vector>
#include "MObject.h"
class Texture2D : MObject
{
private:
	std::wstring Filename;
	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
protected:
	virtual void Dispose() {
		Resource = nullptr;
		UploadHeap = nullptr;
	}
public:
	std::string Name;
	bool isReadable() const { return UploadHeap == nullptr; }
	bool isAvaliable() const { return Resource == nullptr; }
	ID3D12Resource* GetResource() const
	{
		return Resource.Get();
	}
	
	void MakeNoLongerReadable()
	{
		UploadHeap = nullptr;
	}
	Texture2D(
		ID3D12GraphicsCommandList* commandList,
		ID3D12Device* device,
		std::string name,
		std::wstring filePath
	);
	void GetResourceViewDescriptor(D3D12_SHADER_RESOURCE_VIEW_DESC& desc);
	virtual ~Texture2D();
};

