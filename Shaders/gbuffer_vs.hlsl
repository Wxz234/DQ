#pragma pack_matrix(row_major)

struct SceneVertex
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

cbuffer c_GBuffer : register(b0)
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
	matrix mvp = c_model * c_view * c_proj;
	o_position = mul(mvp, float4(i_vtx.position, 1.0));
	o_texcoord = i_vtx.texcoord;
	o_wnormal = normalize(mul((float3x3)c_world_inv_transpose, i_vtx.normal));
}