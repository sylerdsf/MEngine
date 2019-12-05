#include "CBufferPool.h" 
#include <math.h>
CBufferPool::CBufferPool(UINT stride, UINT initCapacity) : initElementCount(initCapacity)
{
	mStride = d3dUtil::CalcConstantBufferByteSize(stride);
	allPoolKeys.reserve(10);
	allPoolValues.reserve(10);
}

ConstBufferElement CBufferPool::GetBuffer(ID3D12Device* device)
{
	PoolValue pv;
	if (allPoolValues.size() == 0)
	{
		pv.buffer = std::make_shared<UploadBuffer>();
		pv.buffer->Create(device, initElementCount, true, mStride);
		pv.pool = new std::vector<UINT>(initElementCount);
		for (int i = 0; i < pv.pool->size(); ++i)
		{
			(*pv.pool)[i] = i;
		}
		allPoolKeys[pv.buffer.operator->()] = allPoolValues.size();
		allPoolValues.push_back(pv);
		
	}
	else
	{
		pv = allPoolValues[allPoolValues.size() - 1];
	}
	ConstBufferElement ele;
	ele.element = (*pv.pool)[pv.pool->size() - 1];
	pv.pool->erase(pv.pool->end() - 1);
	if (pv.pool->size() <= 0)
	{
		delete pv.pool;
		allPoolKeys.erase(pv.buffer.operator->());
		allPoolValues[allPoolValues.size() - 1].buffer = nullptr;
		allPoolValues[allPoolValues.size() - 1].pool = nullptr;
		allPoolValues.erase(allPoolValues.end() - 1);
	}
	ele.buffer = pv.buffer;
	return ele;
}

void CBufferPool::Release(std::shared_ptr<UploadBuffer> buffer, UINT ind)
{
	auto&& ite = allPoolKeys.find(buffer.operator->());
	if (ite != allPoolKeys.end())
	{
		allPoolValues[ite->second].pool->push_back(ind);
	}
	else {
		PoolValue pv;
		pv.buffer = buffer;
		pv.pool = new std::vector<UINT>();
		allPoolKeys[buffer.operator->()] = allPoolValues.size();
		pv.pool->reserve(initElementCount);
		allPoolValues.push_back(pv);
		pv.pool->push_back(ind);
	}
}

CBufferPool::~CBufferPool()
{
	for (int i = 0; i < allPoolValues.size(); ++i)
	{
		auto& a = allPoolValues[i];
		a.buffer = nullptr;
		if (a.pool != nullptr)
		{
			delete a.pool;
			a.pool = nullptr;
		}
	}
}