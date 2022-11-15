#pragma pack_matrix(row_major)

struct SceneVertex
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
};

cbuffer c_GBuffer : register(b0)
{
	matrix c_model;
	matrix c_view;
	matrix c_proj;
};

void main(
	in SceneVertex i_vtx,
	out float4 o_position : SV_Position
)
{
	o_position = float4(i_vtx.position, 1.0);
}