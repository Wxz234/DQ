#pragma pack_matrix(row_major)

cbuffer c_GBuffer : register(b0)
{
    matrix c_model;
};

void main(
	in float4 i_position : POSITION,
	out float4 o_position : SV_Position
)
{
	o_position = i_position;
}