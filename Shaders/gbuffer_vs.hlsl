#pragma pack_matrix(row_major)

struct SceneVertex
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct c_GBuffer
{
	matrix c_model;
	matrix c_view;
	matrix c_proj;
	matrix c_world_inv_transpose;
};

void main(
	in SceneVertex i_vtx,
	out float4 o_position : SV_POSITION,
	out float2 o_texcoord : TEXCOORD,
	out float3 o_wnormal : NORMAL
)
{
	ConstantBuffer<c_GBuffer> camera = ResourceDescriptorHeap[0];
	matrix mvp = camera.c_model * camera.c_view * camera.c_proj;
	o_position = mul(mvp, float4(i_vtx.position, 1.0));
	o_texcoord = i_vtx.texcoord;
	o_wnormal = normalize(mul((float3x3)camera.c_world_inv_transpose, i_vtx.normal));
}