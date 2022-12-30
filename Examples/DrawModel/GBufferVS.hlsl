#pragma pack_matrix(row_major)

struct SceneVertex
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    uint   materialID : MATERIAL_ID;
};

struct c_Camera
{
    matrix c_model;
    matrix c_view;
    matrix c_proj;
    matrix c_world_inv_transpose;
};

void main(
    in SceneVertex i_vtx,
    out float4 o_position : SV_POSITION,
    out float3 o_wnormal : NORMAL,
    out float2 o_texcoord : TEXCOORD,
    out uint   o_materialID : MATERIAL_ID
)
{
    ConstantBuffer<c_Camera> camera = ResourceDescriptorHeap[0];
    matrix mvp = camera.c_model * camera.c_view * camera.c_proj;
    o_position = mul(mvp, float4(i_vtx.position, 1.0));
    o_texcoord = i_vtx.texcoord;
    o_wnormal = normalize(mul((float3x3)camera.c_world_inv_transpose, i_vtx.normal));
    o_materialID = i_vtx.materialID;
}