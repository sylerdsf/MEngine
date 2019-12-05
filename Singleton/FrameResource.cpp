#include "FrameResource.h"
std::vector<std::unique_ptr<FrameResource>> FrameResource::mFrameResources;
FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount)
{
    ThrowIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

  //  FrameCB = std::make_unique<UploadBuffer<FrameConstants>>(device, 1, true);
	ObjectCB = std::make_shared<UploadBuffer>();
    ObjectCB->Create(device, objectCount, true, sizeof(ObjectConstants));
	cameraCBs.reserve(50);
}

void FrameResource::UpdateBeforeFrame(ID3D12Fence* mFence)
{
	if (Fence != 0 && mFence->GetCompletedValue() < Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}
void FrameResource::UpdateAfterFrame(UINT64& currentFence, ID3D12CommandQueue* commandQueue, ID3D12Fence* mFence)
{
	// Advance the fence value to mark commands up to this fence point.
	Fence = ++currentFence;

	// Add an instruction to the command queue to set a new fence point. 
	// Because we are on the GPU timeline, the new fence point won't be 
	// set until the GPU finishes processing all the commands prior to this Signal().
	commandQueue->Signal(mFence, currentFence);
}

FrameResource::~FrameResource()
{

}