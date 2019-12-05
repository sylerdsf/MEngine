#pragma once
#include <unordered_map>
#include "../Common/d3dUtil.h"
class MeshLayout
{
	/*
	Normal
	Tangent
	Color
	UV0
	UV2
	UV3
	UV4
	*/
private:
	static std::unordered_map<USHORT, UINT> layoutDict;
	static std::vector<std::vector<D3D12_INPUT_ELEMENT_DESC>*> layoutValues;
	static void GenerateDesc(
		std::vector<D3D12_INPUT_ELEMENT_DESC>& target,
		bool normal,
		bool tangent,
		bool color,
		bool uv0,
		bool uv2,
		bool uv3,
		bool uv4
		);
public:
	static std::vector<D3D12_INPUT_ELEMENT_DESC>* GetMeshLayoutValue(UINT index);
	static std::vector<D3D12_INPUT_ELEMENT_DESC>* GetMeshLayoutValue(
		bool normal,
		bool tangent,
		bool color,
		bool uv0,
		bool uv2,
		bool uv3,
		bool uv4
	);
	static UINT GetMeshLayoutIndex(
		bool normal,
		bool tangent,
		bool color,
		bool uv0,
		bool uv2,
		bool uv3,
		bool uv4
	);
};

