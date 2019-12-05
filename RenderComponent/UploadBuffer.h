#pragma once

#include "../Common/d3dUtil.h"
#include "../RenderComponent/MObject.h"

class UploadBuffer : public MObject
{
protected:
	virtual void Dispose()
	{
		if (mUploadBuffer != nullptr)
			mUploadBuffer->Unmap(0, nullptr);
		mMappedData = nullptr;
	}
public:
	void Create(ID3D12Device* device, UINT elementCount, bool isConstantBuffer, size_t stride);
	UploadBuffer() : MObject() {}
    UploadBuffer(const UploadBuffer& rhs) = delete;
    UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
    ID3D12Resource* Resource()const
    {
        return mUploadBuffer.Get();
    }

    void CopyData(int elementIndex, const void* data)
    {
        memcpy(&mMappedData[elementIndex*mElementByteSize], data, mStride);
    }
	size_t GetStride() const { return mStride; }
	size_t GetAlignedStride() const { return mElementByteSize; }
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
    BYTE* mMappedData = nullptr;
	size_t mStride;
    UINT mElementByteSize = 0;
    bool mIsConstantBuffer = false;
};