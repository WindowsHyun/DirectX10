#include "Light.fx"

cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

//월드 변환 행렬을 위한 쉐이더 변수를 선언한다(슬롯 1을 사용). 
cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};

/*(주의) register(b0)에서 b는 레지스터가 상수 버퍼를 위해 사용되는 것을 의미한다. 0는 레지스터의 번호이며 응용 프로그램에서 상수 버퍼를 디바이스 컨텍스트에 연결할 때의 슬롯 번호와 일치하도록 해야 한다.
pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
*/
//정점-쉐이더의 출력을 위한 구조체이다.
struct VS_INPUT
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};
struct VS_INSTANCED_COLOR_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR0;
	column_major float4x4 mtxTransform : INSTANCEPOS;
	float4 instanceColor : INSTANCECOLOR;
};

struct VS_INSTANCED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	//시스템 생성 변수로 정점 쉐이더에 전달되는 객체 인스턴스의 ID를 픽셀 쉐이더로 전달한다.
	float4 instanceID : INDEX;
};
VS_INSTANCED_COLOR_OUTPUT VSInstancedDiffusedColor( VS_INSTANCED_COLOR_INPUT input, uint instanceID : SV_InstanceID )
{
	VS_INSTANCED_COLOR_OUTPUT output = (VS_INSTANCED_COLOR_OUTPUT)0;
	output.position = mul( float4(input.position, 1), input.mtxTransform );
	output.position = mul( output.position, gmtxView );
	output.position = mul( output.position, gmtxProjection );
	output.color = (instanceID % 5) ? input.color : (input.color * 0.3f + input.instanceColor * 0.0f);
	output.instanceID = instanceID;
	return output;
}
// 정점-쉐이더 
VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul( input.position, gmtxWorld );
	output.position = mul( output.position, gmtxView );
	output.position = mul( output.position, gmtxProjection );
	output.color = input.color;
	//입력되는 정점의 색상을 그대로 출력한다. 
	return output;
}

// 픽셀-쉐이더
float4 PS( VS_OUTPUT input ) : SV_Target
{
	return input.color;
//입력되는 정점의 색상을 그대로 출력한다. 
}
float4 PSInstancedDiffusedColor( VS_INSTANCED_COLOR_OUTPUT input ) : SV_Target
{
	return input.color;
}

//조명을 사용하는 경우 정점 쉐이더의 입력을 위한 구조체이다.
struct VS_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};
//조명을 사용하는 경우 정점 쉐이더의 출력을 위한 구조체이다.
struct VS_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	//월드좌표계에서 정점의 위치와 법선 벡터를 나타낸다.
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
};

//인스턴싱을 하면서 조명을 사용하는 경우 정점 쉐이더의 입력을 위한 구조체이다.
struct VS_INSTANCED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4x4 mtxTransform : INSTANCEPOS;
};

//인스턴싱을 하면서 조명을 사용하는 경우 정점 쉐이더의 출력을 위한 구조체이다.
struct VS_INSTANCED_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
};

//조명의 영향을 계산하기 위한 정점의 법선 벡터와 정점의 위치를 계산하는 정점 쉐이더 함수이다.
VS_LIGHTING_COLOR_OUTPUT VSLightingColor( VS_LIGHTING_COLOR_INPUT input )
{
	VS_LIGHTING_COLOR_OUTPUT output = (VS_LIGHTING_COLOR_OUTPUT)0;
	//조명의 영향을 계산하기 위하여 월드좌표계에서 정점의 위치와 법선 벡터를 구한다.
	output.normalW = mul( input.normal, (float3x3)gmtxWorld );
	output.positionW = mul( float4(input.position, 1.0f), gmtxWorld ).xyz;
	output.position = mul( mul( float4(output.positionW, 1.0f), gmtxView ), gmtxProjection );

	return(output);
}

//각 픽셀에 대하여 조명의 영향을 반영한 색상을 계산하기 위한 픽셀 쉐이더 함수이다.
float4 PSLightingColor( VS_LIGHTING_COLOR_OUTPUT input ) : SV_Target
{
	input.normalW = normalize( input.normalW );
float4 cIllumination = Lighting( input.positionW, input.normalW );

return(cIllumination);
}

VS_INSTANCED_LIGHTING_COLOR_OUTPUT VSInstancedLightingColor( VS_INSTANCED_LIGHTING_COLOR_INPUT input )
{
	VS_INSTANCED_LIGHTING_COLOR_OUTPUT output = (VS_INSTANCED_LIGHTING_COLOR_OUTPUT)0;
	output.normalW = mul( input.normal, (float3x3)input.mtxTransform );
	output.positionW = mul( float4(input.position, 1.0f), input.mtxTransform ).xyz;
	output.position = mul( mul( float4(output.positionW, 1.0f), gmtxView ), gmtxProjection );

	return(output);
}

float4 PSInstancedLightingColor( VS_INSTANCED_LIGHTING_COLOR_OUTPUT input ) : SV_Target
{
	input.normalW = normalize( input.normalW );
float4 cIllumination = Lighting( input.positionW, input.normalW );

return(cIllumination);
}