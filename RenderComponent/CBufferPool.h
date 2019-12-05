#pragma once
#include "../Common/d3dUtil.h"
#include "UploadBuffer.h"
#include "MObject.h"
struct ConstBufferElement
{
	std::shared_ptr<UploadBuffer> buffer;
	UINT element;
};
class CBufferPool
{
private:
	struct PoolValue
	{
		std::shared_ptr<UploadBuffer> buffer;
		std::vector<UINT>* pool;
	};
	UINT mStride;
	UINT initElementCount;
	std::vector<PoolValue> allPoolValues;
	std::unordered_map<UploadBuffer*, UINT> allPoolKeys;
public:
	CBufferPool(UINT stride, UINT initCapacity);
	ConstBufferElement GetBuffer(ID3D12Device* device);
	void Release(std::shared_ptr<UploadBuffer> buffer, UINT ind);
	virtual ~CBufferPool();
};