#include "stdafx.h"
#include "Mesh.h"

void AABB::Merge(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum)
{
	if (d3dxvMinimum.x < m_d3dxvMinimum.x) m_d3dxvMinimum.x = d3dxvMinimum.x;
	if (d3dxvMinimum.y < m_d3dxvMinimum.y) m_d3dxvMinimum.y = d3dxvMinimum.y;
	if (d3dxvMinimum.z < m_d3dxvMinimum.z) m_d3dxvMinimum.z = d3dxvMinimum.z;
	if (d3dxvMaximum.x > m_d3dxvMaximum.x) m_d3dxvMaximum.x = d3dxvMaximum.x;
	if (d3dxvMaximum.y > m_d3dxvMaximum.y) m_d3dxvMaximum.y = d3dxvMaximum.y;
	if (d3dxvMaximum.z > m_d3dxvMaximum.z) m_d3dxvMaximum.z = d3dxvMaximum.z;
}

void AABB::Merge(AABB *pAABB)
{
	Merge(pAABB->m_d3dxvMinimum, pAABB->m_d3dxvMaximum);
}
void AABB::Update(D3DXMATRIX *pmtxTransform)
{
	D3DXVECTOR3 vVertices[8];
	vVertices[0] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMinimum.y, m_d3dxvMinimum.z);
	vVertices[1] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMinimum.y, m_d3dxvMaximum.z);
	vVertices[2] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMinimum.y, m_d3dxvMaximum.z);
	vVertices[3] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMinimum.y, m_d3dxvMinimum.z);
	vVertices[4] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMaximum.y, m_d3dxvMinimum.z);
	vVertices[5] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMaximum.y, m_d3dxvMaximum.z);
	vVertices[6] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMaximum.y, m_d3dxvMaximum.z);
	vVertices[7] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMaximum.y, m_d3dxvMinimum.z);
	m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	//8개의 정점에서 x, y, z 좌표의 최소값과 최대값을 구한다.
	for (int i = 0; i < 8; i++)
	{
		//정점을 변환한다.
		D3DXVec3TransformCoord(&vVertices[i], &vVertices[i], pmtxTransform);
		if (vVertices[i].x < m_d3dxvMinimum.x) m_d3dxvMinimum.x = vVertices[i].x;
		if (vVertices[i].y < m_d3dxvMinimum.y) m_d3dxvMinimum.y = vVertices[i].y;
		if (vVertices[i].z < m_d3dxvMinimum.z) m_d3dxvMinimum.z = vVertices[i].z;
		if (vVertices[i].x > m_d3dxvMaximum.x) m_d3dxvMaximum.x = vVertices[i].x;
		if (vVertices[i].y > m_d3dxvMaximum.y) m_d3dxvMaximum.y = vVertices[i].y;
		if (vVertices[i].z > m_d3dxvMaximum.z) m_d3dxvMaximum.z = vVertices[i].z;
	}
}

CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	m_nBuffers = 0;
	m_pd3dPositionBuffer = NULL;
	m_pd3dColorBuffer = NULL;
	m_ppd3dVertexBuffers = NULL;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nSlot = 0;
	m_nStartVertex = 0;

	m_pd3dIndexBuffer = NULL;
	m_nIndices = 0;
	m_nStartIndex = 0;
	m_nBaseVertex = 0;
	m_nIndexOffset = 0;
	m_dxgiIndexFormat = DXGI_FORMAT_R32_UINT;

	m_pd3dRasterizerState = NULL;

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	m_nReferences = 0;
	m_pd3dxvPositions = NULL;
	m_pnIndices = NULL;
}

CMesh::~CMesh()
{
	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();
	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();
	if (m_pd3dColorBuffer) m_pd3dColorBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();

	if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
	if (m_pnVertexStrides) delete[] m_pnVertexStrides;
	if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;
	if (m_pd3dxvPositions) delete[] m_pd3dxvPositions;
	if (m_pnIndices) delete[] m_pnIndices;
}
void CMesh::RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance)
{
	//인스턴싱의 경우 입력 조립기에 메쉬의 정점 버퍼와 인스턴스 정점 버퍼가 연결된다.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	//객체들의 인스턴스들을 렌더링한다. 
	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexedInstanced(m_nIndices, nInstances, m_nStartIndex, m_nBaseVertex, nStartInstance);
	else
		pd3dDeviceContext->DrawInstanced(m_nVertices, nInstances, m_nStartVertex, nStartInstance);
}
void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//메쉬의 정점은 여러 개의 정점 버퍼로 표현된다.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, m_nStartVertex);
}

void CMesh::AssembleToVertexBuffer(int nBuffers, ID3D11Buffer **ppd3dBuffers, UINT *pnBufferStrides, UINT *pnBufferOffsets)
{
	ID3D11Buffer **ppd3dNewVertexBuffers = new ID3D11Buffer*[m_nBuffers + nBuffers];
	UINT *pnNewVertexStrides = new UINT[m_nBuffers + nBuffers];
	UINT *pnNewVertexOffsets = new UINT[m_nBuffers + nBuffers];

	if (m_nBuffers > 0)
	{
		for (int i = 0; i < m_nBuffers; i++)
		{
			ppd3dNewVertexBuffers[i] = m_ppd3dVertexBuffers[i];
			pnNewVertexStrides[i] = m_pnVertexStrides[i];
			pnNewVertexOffsets[i] = m_pnVertexOffsets[i];
		}
		if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
		if (m_pnVertexStrides) delete[] m_pnVertexStrides;
		if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;
	}

	for (int i = 0; i < nBuffers; i++)
	{
		ppd3dNewVertexBuffers[m_nBuffers + i] = ppd3dBuffers[i];
		pnNewVertexStrides[m_nBuffers + i] = pnBufferStrides[i];
		pnNewVertexOffsets[m_nBuffers + i] = pnBufferOffsets[i];
	}

	m_nBuffers += nBuffers;
	m_ppd3dVertexBuffers = ppd3dNewVertexBuffers;
	m_pnVertexStrides = pnNewVertexStrides;
	m_pnVertexOffsets = pnNewVertexOffsets;
}
int CMesh::CheckRayIntersection(D3DXVECTOR3 *pd3dxvRayPosition, D3DXVECTOR3 *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	//모델 좌표계의 광선의 시작점(pd3dxvRayPosition)과 방향이 주어질 때 메쉬와의 충돌 검사를 한다.
	int nIntersections = 0;
	BYTE *pbPositions = (BYTE *)m_pd3dxvPositions + m_pnVertexOffsets[0];

	int nOffset = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	//메쉬의 프리미티브(삼각형)들의 개수이다. 삼각형 리스트인 경우 (정점의 개수 / 3) 또는 (인덱스의 개수 / 3), 삼각형 스트립의 경우 (정점의 개수 - 2) 또는 (인덱스의 개수 - 2)이다.
	int nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	D3DXVECTOR3 v0, v1, v2;
	float fuHitBaryCentric, fvHitBaryCentric, fHitDistance, fNearHitDistance = FLT_MAX;
	/*메쉬의 모든 프리미티브(삼각형)들에 대하여 픽킹 광선과의 충돌을 검사한다. 충돌하는 모든 삼각형을 찾아 광선의 시작점(실제로는 카메라 좌표계의 원점)에 가장 가까운 삼각형을 찾는다.*/
	for (int i = 0; i < nPrimitives; i++)
	{
		v0 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 0]) : ((i*nOffset) + 0)) * m_pnVertexStrides[0]);
		v1 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 1]) : ((i*nOffset) + 1)) * m_pnVertexStrides[0]);
		v2 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 2]) : ((i*nOffset) + 2)) * m_pnVertexStrides[0]);
		if (D3DXIntersectTri(&v0, &v1, &v2, pd3dxvRayPosition, pd3dxvRayDirection, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
		{
			if (fHitDistance < fNearHitDistance)
			{
				fNearHitDistance = fHitDistance;
				if (pd3dxIntersectInfo)
				{
					pd3dxIntersectInfo->m_dwFaceIndex = i;
					pd3dxIntersectInfo->m_fU = fuHitBaryCentric;
					pd3dxIntersectInfo->m_fV = fvHitBaryCentric;
					pd3dxIntersectInfo->m_fDistance = fHitDistance;
				}
			}
			nIntersections++;
		}
	}
	return(nIntersections);
}

CTriangleMesh::CTriangleMesh(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*정점(삼각형의 꼭지점)의 색상은 시계방향 순서대로 빨간색, 녹색, 파란색으로 지정한다. D3DXCOLOR 매크로는 RGBA(Red, Green, Blue, Alpha) 4개의 파라메터를 사용하여 색상을 표현하기 위하여 사용한다. 각 파라메터는 0.0~1.0 사이의 실수값을 가진다.*/
	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(D3DXVECTOR3(0.0f, 0.5f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(D3DXVECTOR3(0.5f, -0.5f, 0.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(D3DXVECTOR3(-0.5f, -0.5f, 0.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = m_nStride * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dVertexBuffer);

	CreateRasterizerState(pd3dDevice);
}




CTriangleMesh::~CTriangleMesh()
{
}
void CTriangleMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	//래스터라이저 단계에서 컬링(은면 제거)을 하지 않도록 래스터라이저 상태를 생성한다.
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}
void CTriangleMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}
CCubeMesh::CCubeMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, D3DXCOLOR d3dxColor) : CMesh(pd3dDevice)
{
	m_nVertices = 8;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	//직육면체 메쉬는 2개의 정점 버퍼(위치 벡터 버퍼와 색상 버퍼)로 구성된다.
	//직육면체 메쉬의 정점 버퍼(위치 벡터 버퍼)를 생성한다.
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	m_pd3dxvPositions[0] = D3DXVECTOR3(-fx, +fy, -fz);
	m_pd3dxvPositions[1] = D3DXVECTOR3(+fx, +fy, -fz);
	m_pd3dxvPositions[2] = D3DXVECTOR3(+fx, +fy, +fz);
	m_pd3dxvPositions[3] = D3DXVECTOR3(-fx, +fy, +fz);
	m_pd3dxvPositions[4] = D3DXVECTOR3(-fx, -fy, -fz);
	m_pd3dxvPositions[5] = D3DXVECTOR3(+fx, -fy, -fz);
	m_pd3dxvPositions[6] = D3DXVECTOR3(+fx, -fy, +fz);
	m_pd3dxvPositions[7] = D3DXVECTOR3(-fx, -fy, +fz);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	//직육면체 메쉬의 정점 버퍼(색상 버퍼)를 생성한다.
	D3DXCOLOR pd3dxColors[8];
	for (int i = 0; i < 8; i++) pd3dxColors[i] = d3dxColor + RANDOM_COLOR;

	d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_nIndices = 18;
	m_pnIndices = new UINT[m_nIndices];
	m_pnIndices[0] = 5; //5,6,4 - cw
	m_pnIndices[1] = 6; //6,4,7 - ccw
	m_pnIndices[2] = 4; //4,7,0 - cw
	m_pnIndices[3] = 7; //7,0,3 - ccw
	m_pnIndices[4] = 0; //0,3,1 - cw
	m_pnIndices[5] = 3; //3,1,2 - ccw
	m_pnIndices[6] = 1; //1,2,2 - cw 
	m_pnIndices[7] = 2; //2,2,3 - ccw
	m_pnIndices[8] = 2; //2,3,3 - cw  - Degenerated Index(2)
	m_pnIndices[9] = 3; //3,3,7 - ccw - Degenerated Index(3)
	m_pnIndices[10] = 3;//3,7,2 - cw  - Degenerated Index(3)
	m_pnIndices[11] = 7;//7,2,6 - ccw
	m_pnIndices[12] = 2;//2,6,1 - cw
	m_pnIndices[13] = 6;//6,1,5 - ccw
	m_pnIndices[14] = 1;//1,5,0 - cw
	m_pnIndices[15] = 5;//5,0,4 - ccw
	m_pnIndices[16] = 0;
	m_pnIndices[17] = 4;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	//인덱스 버퍼를 생성한다.
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	CreateRasterizerState(pd3dDevice);

	//정점 버퍼 데이터를 생성한 다음 최소점과 최대점을 저장한다. 
	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}


CCubeMesh::~CCubeMesh()
{
}
void CCubeMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	// 이부분이 실질적으로 행동하는 부분 (추가)
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}
void CCubeMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}

CAirplaneMesh::CAirplaneMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, D3DXCOLOR d3dxColor) : CMesh(pd3dDevice)
{
	m_nVertices = 24 * 3;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	//비행기 메쉬는 2개의 정점 버퍼(위치 벡터 버퍼와 색상 버퍼)로 구성된다.
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];

	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;
	//Upper Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);

	//Lower Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);

	//Right Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);

	//Back/Right Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);

	//Left Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);

	//Back/Left Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	D3DXCOLOR pd3dxColors[24 * 3];
	for (int j = 0; j < m_nVertices; j++) pd3dxColors[j] = d3dxColor + RANDOM_COLOR;
	pd3dxColors[0] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[3] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[6] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[9] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[12] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[15] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[18] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[21] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

	d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);

	ID3D11Buffer *ppd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, ppd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}


CAirplaneMesh::~CAirplaneMesh()
{
}

CSphereMesh::CSphereMesh(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks, D3DXCOLOR d3dxColor) : CMesh(pd3dDevice)
{
	m_nVertices = (nSlices * nStacks) * 3 * 2;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//구 메쉬는 2개의 정점 버퍼(위치 벡터 버퍼와 색상 버퍼)로 구성된다.
	//구 메쉬의 정점 버퍼(위치 벡터 버퍼)를 생성한다.
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	float theta_i, theta_ii, phi_j, phi_jj, fRadius_j, fRadius_jj, y_j, y_jj;
	for (int j = 0, k = 0; j < nStacks; j++)
	{
		phi_j = float(D3DX_PI / nStacks) * j;
		phi_jj = float(D3DX_PI / nStacks) * (j + 1);
		fRadius_j = fRadius * sinf(phi_j);
		fRadius_jj = fRadius * sinf(phi_jj);
		y_j = fRadius*cosf(phi_j);
		y_jj = fRadius*cosf(phi_jj);
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = float(2 * D3DX_PI / nSlices) * i;
			theta_ii = float(2 * D3DX_PI / nSlices) * (i + 1);
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_i), y_j, fRadius_j*sinf(theta_i));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_ii), y_jj, fRadius_jj*sinf(theta_ii));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
		}
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	

	//구 메쉬의 정점 버퍼(색상 버퍼)를 생성한다.
	D3DXCOLOR *pd3dxColors = new D3DXCOLOR[m_nVertices];
	for (int i = 0; i < m_nVertices; i++) pd3dxColors[i] = d3dxColor + RANDOM_COLOR;

	d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);

	delete[] pd3dxColors;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fRadius, -fRadius, -fRadius);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fRadius, +fRadius, +fRadius);
}

CSphereMesh::~CSphereMesh()
{
}
void CSphereMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	d3dRasterizerDesc.DepthClipEnable = true;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CBCubeMesh::CBCubeMesh( ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, D3DXCOLOR d3dxColor ) : CMesh( pd3dDevice )
{
	m_nVertices = 8;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	//직육면체 메쉬는 2개의 정점 버퍼(위치 벡터 버퍼와 색상 버퍼)로 구성된다.
	//직육면체 메쉬의 정점 버퍼(위치 벡터 버퍼)를 생성한다.
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	m_pd3dxvPositions[0] = D3DXVECTOR3( -fx, +fy, -fz );
	m_pd3dxvPositions[1] = D3DXVECTOR3( +fx, +fy, -fz );
	m_pd3dxvPositions[2] = D3DXVECTOR3( +fx, +fy, +fz );
	m_pd3dxvPositions[3] = D3DXVECTOR3( -fx, +fy, +fz );
	m_pd3dxvPositions[4] = D3DXVECTOR3( -fx, -fy, -fz );
	m_pd3dxvPositions[5] = D3DXVECTOR3( +fx, -fy, -fz );
	m_pd3dxvPositions[6] = D3DXVECTOR3( +fx, -fy, +fz );
	m_pd3dxvPositions[7] = D3DXVECTOR3( -fx, -fy, +fz );

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof( D3DXVECTOR3 ) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer );

	//직육면체 메쉬의 정점 버퍼(색상 버퍼)를 생성한다.
	D3DXCOLOR pd3dxColors[8];
	for ( int i = 0; i < 8; i++ ) pd3dxColors[i] = d3dxColor;

	d3dBufferDesc.ByteWidth = sizeof( D3DXCOLOR ) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer );

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof( D3DXVECTOR3 ), sizeof( D3DXCOLOR ) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer( 2, pd3dBuffers, pnBufferStrides, pnBufferOffsets );

	m_nIndices = 18;
	m_pnIndices = new UINT[m_nIndices];
	m_pnIndices[0] = 5; //5,6,4 - cw
	m_pnIndices[1] = 6; //6,4,7 - ccw
	m_pnIndices[2] = 4; //4,7,0 - cw
	m_pnIndices[3] = 7; //7,0,3 - ccw
	m_pnIndices[4] = 0; //0,3,1 - cw
	m_pnIndices[5] = 3; //3,1,2 - ccw
	m_pnIndices[6] = 1; //1,2,2 - cw 
	m_pnIndices[7] = 2; //2,2,3 - ccw
	m_pnIndices[8] = 2; //2,3,3 - cw  - Degenerated Index(2)
	m_pnIndices[9] = 3; //3,3,7 - ccw - Degenerated Index(3)
	m_pnIndices[10] = 3;//3,7,2 - cw  - Degenerated Index(3)
	m_pnIndices[11] = 7;//7,2,6 - ccw
	m_pnIndices[12] = 2;//2,6,1 - cw
	m_pnIndices[13] = 6;//6,1,5 - ccw
	m_pnIndices[14] = 1;//1,5,0 - cw
	m_pnIndices[15] = 5;//5,0,4 - ccw
	m_pnIndices[16] = 0;
	m_pnIndices[17] = 4;

	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof( UINT ) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = m_pnIndices;
	//인덱스 버퍼를 생성한다.
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer );

	CreateRasterizerState( pd3dDevice );

	//정점 버퍼 데이터를 생성한 다음 최소점과 최대점을 저장한다. 
	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3( -fx, -fy, -fz );
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3( +fx, +fy, +fz );
}


CBCubeMesh::~CBCubeMesh()
{
}
void CBCubeMesh::CreateRasterizerState( ID3D11Device *pd3dDevice )
{
	// 이부분이 실질적으로 행동하는 부분 (추가)
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory( &d3dRasterizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState( &d3dRasterizerDesc, &m_pd3dRasterizerState );
}
void CBCubeMesh::Render( ID3D11DeviceContext *pd3dDeviceContext )
{
	CMesh::Render( pd3dDeviceContext );
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CMCubeMesh::CMCubeMesh( ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, D3DXCOLOR d3dxColor ) : CMesh( pd3dDevice )
{
	m_nVertices = 8;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	//직육면체 메쉬는 2개의 정점 버퍼(위치 벡터 버퍼와 색상 버퍼)로 구성된다.
	//직육면체 메쉬의 정점 버퍼(위치 벡터 버퍼)를 생성한다.
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	m_pd3dxvPositions[0] = D3DXVECTOR3( -fx, +fy, -fz );
	m_pd3dxvPositions[1] = D3DXVECTOR3( +fx, +fy, -fz );
	m_pd3dxvPositions[2] = D3DXVECTOR3( +fx, +fy, +fz );
	m_pd3dxvPositions[3] = D3DXVECTOR3( -fx, +fy, +fz );
	m_pd3dxvPositions[4] = D3DXVECTOR3( -fx, -fy, -fz );
	m_pd3dxvPositions[5] = D3DXVECTOR3( +fx, -fy, -fz );
	m_pd3dxvPositions[6] = D3DXVECTOR3( +fx, -fy, +fz );
	m_pd3dxvPositions[7] = D3DXVECTOR3( -fx, -fy, +fz );

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof( D3DXVECTOR3 ) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer );

	//직육면체 메쉬의 정점 버퍼(색상 버퍼)를 생성한다.
	D3DXCOLOR pd3dxColors[8];
	for ( int i = 0; i < 8; i++ ) pd3dxColors[i] = d3dxColor;

	d3dBufferDesc.ByteWidth = sizeof( D3DXCOLOR ) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer );

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof( D3DXVECTOR3 ), sizeof( D3DXCOLOR ) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer( 2, pd3dBuffers, pnBufferStrides, pnBufferOffsets );

	m_nIndices = 18;
	m_pnIndices = new UINT[m_nIndices];
	m_pnIndices[0] = 5; //5,6,4 - cw
	m_pnIndices[1] = 6; //6,4,7 - ccw
	m_pnIndices[2] = 4; //4,7,0 - cw
	m_pnIndices[3] = 7; //7,0,3 - ccw
	m_pnIndices[4] = 0; //0,3,1 - cw
	m_pnIndices[5] = 3; //3,1,2 - ccw
	m_pnIndices[6] = 1; //1,2,2 - cw 
	m_pnIndices[7] = 2; //2,2,3 - ccw
	m_pnIndices[8] = 2; //2,3,3 - cw  - Degenerated Index(2)
	m_pnIndices[9] = 3; //3,3,7 - ccw - Degenerated Index(3)
	m_pnIndices[10] = 3;//3,7,2 - cw  - Degenerated Index(3)
	m_pnIndices[11] = 7;//7,2,6 - ccw
	m_pnIndices[12] = 2;//2,6,1 - cw
	m_pnIndices[13] = 6;//6,1,5 - ccw
	m_pnIndices[14] = 1;//1,5,0 - cw
	m_pnIndices[15] = 5;//5,0,4 - ccw
	m_pnIndices[16] = 0;
	m_pnIndices[17] = 4;

	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof( UINT ) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = m_pnIndices;
	//인덱스 버퍼를 생성한다.
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer );

	CreateRasterizerState( pd3dDevice );

	//정점 버퍼 데이터를 생성한 다음 최소점과 최대점을 저장한다. 
	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3( -fx, -fy, -fz );
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3( +fx, +fy, +fz );
}


CMCubeMesh::~CMCubeMesh()
{
}
void CMCubeMesh::CreateRasterizerState( ID3D11Device *pd3dDevice )
{
	// 이부분이 실질적으로 행동하는 부분 (추가)
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory( &d3dRasterizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState( &d3dRasterizerDesc, &m_pd3dRasterizerState );
}
void CMCubeMesh::Render( ID3D11DeviceContext *pd3dDeviceContext )
{
	CMesh::Render( pd3dDeviceContext );
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CMazeCubeMesh::CMazeCubeMesh( ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, D3DXCOLOR d3dxColor ) : CMesh( pd3dDevice )
{
	m_nVertices = 8;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	//직육면체 메쉬는 2개의 정점 버퍼(위치 벡터 버퍼와 색상 버퍼)로 구성된다.
	//직육면체 메쉬의 정점 버퍼(위치 벡터 버퍼)를 생성한다.
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	m_pd3dxvPositions[0] = D3DXVECTOR3( -fx, +fy, -fz );
	m_pd3dxvPositions[1] = D3DXVECTOR3( +fx, +fy, -fz );
	m_pd3dxvPositions[2] = D3DXVECTOR3( +fx, +fy, +fz );
	m_pd3dxvPositions[3] = D3DXVECTOR3( -fx, +fy, +fz );
	m_pd3dxvPositions[4] = D3DXVECTOR3( -fx, -fy, -fz );
	m_pd3dxvPositions[5] = D3DXVECTOR3( +fx, -fy, -fz );
	m_pd3dxvPositions[6] = D3DXVECTOR3( +fx, -fy, +fz );
	m_pd3dxvPositions[7] = D3DXVECTOR3( -fx, -fy, +fz );

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof( D3DXVECTOR3 ) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer );

	//직육면체 메쉬의 정점 버퍼(색상 버퍼)를 생성한다.
	D3DXCOLOR pd3dxColors[8];
	for ( int i = 0; i < 8; i++ ) pd3dxColors[i] = d3dxColor;

	d3dBufferDesc.ByteWidth = sizeof( D3DXCOLOR ) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer );

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof( D3DXVECTOR3 ), sizeof( D3DXCOLOR ) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer( 2, pd3dBuffers, pnBufferStrides, pnBufferOffsets );

	m_nIndices = 18;
	m_pnIndices = new UINT[m_nIndices];
	m_pnIndices[0] = 5; //5,6,4 - cw
	m_pnIndices[1] = 6; //6,4,7 - ccw
	m_pnIndices[2] = 4; //4,7,0 - cw
	m_pnIndices[3] = 7; //7,0,3 - ccw
	m_pnIndices[4] = 0; //0,3,1 - cw
	m_pnIndices[5] = 3; //3,1,2 - ccw
	m_pnIndices[6] = 1; //1,2,2 - cw 
	m_pnIndices[7] = 2; //2,2,3 - ccw
	m_pnIndices[8] = 2; //2,3,3 - cw  - Degenerated Index(2)
	m_pnIndices[9] = 3; //3,3,7 - ccw - Degenerated Index(3)
	m_pnIndices[10] = 3;//3,7,2 - cw  - Degenerated Index(3)
	m_pnIndices[11] = 7;//7,2,6 - ccw
	m_pnIndices[12] = 2;//2,6,1 - cw
	m_pnIndices[13] = 6;//6,1,5 - ccw
	m_pnIndices[14] = 1;//1,5,0 - cw
	m_pnIndices[15] = 5;//5,0,4 - ccw
	m_pnIndices[16] = 0;
	m_pnIndices[17] = 4;

	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof( UINT ) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = m_pnIndices;
	//인덱스 버퍼를 생성한다.
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer );

	CreateRasterizerState( pd3dDevice );

	//정점 버퍼 데이터를 생성한 다음 최소점과 최대점을 저장한다. 
	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3( -fx, -fy, -fz );
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3( +fx, +fy, +fz );
}


CMazeCubeMesh::~CMazeCubeMesh()
{
}
void CMazeCubeMesh::CreateRasterizerState( ID3D11Device *pd3dDevice )
{
	// 이부분이 실질적으로 행동하는 부분 (추가)
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory( &d3dRasterizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState( &d3dRasterizerDesc, &m_pd3dRasterizerState );
}
void CMazeCubeMesh::Render( ID3D11DeviceContext *pd3dDeviceContext )
{
	CMesh::Render( pd3dDeviceContext );
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CCellCubeMesh::CCellCubeMesh( ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, D3DXCOLOR d3dxColor ) : CMesh( pd3dDevice )
{
	m_nVertices = 8;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	//직육면체 메쉬는 2개의 정점 버퍼(위치 벡터 버퍼와 색상 버퍼)로 구성된다.
	//직육면체 메쉬의 정점 버퍼(위치 벡터 버퍼)를 생성한다.
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	m_pd3dxvPositions[0] = D3DXVECTOR3( -fx, +fy, -fz );
	m_pd3dxvPositions[1] = D3DXVECTOR3( +fx, +fy, -fz );
	m_pd3dxvPositions[2] = D3DXVECTOR3( +fx, +fy, +fz );
	m_pd3dxvPositions[3] = D3DXVECTOR3( -fx, +fy, +fz );
	m_pd3dxvPositions[4] = D3DXVECTOR3( -fx, -fy, -fz );
	m_pd3dxvPositions[5] = D3DXVECTOR3( +fx, -fy, -fz );
	m_pd3dxvPositions[6] = D3DXVECTOR3( +fx, -fy, +fz );
	m_pd3dxvPositions[7] = D3DXVECTOR3( -fx, -fy, +fz );

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof( D3DXVECTOR3 ) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer );

	//직육면체 메쉬의 정점 버퍼(색상 버퍼)를 생성한다.
	D3DXCOLOR pd3dxColors[8];
	for ( int i = 0; i < 8; i++ ) pd3dxColors[i] = d3dxColor;

	d3dBufferDesc.ByteWidth = sizeof( D3DXCOLOR ) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer );

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof( D3DXVECTOR3 ), sizeof( D3DXCOLOR ) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer( 2, pd3dBuffers, pnBufferStrides, pnBufferOffsets );

	m_nIndices = 18;
	m_pnIndices = new UINT[m_nIndices];
	m_pnIndices[0] = 5; //5,6,4 - cw
	m_pnIndices[1] = 6; //6,4,7 - ccw
	m_pnIndices[2] = 4; //4,7,0 - cw
	m_pnIndices[3] = 7; //7,0,3 - ccw
	m_pnIndices[4] = 0; //0,3,1 - cw
	m_pnIndices[5] = 3; //3,1,2 - ccw
	m_pnIndices[6] = 1; //1,2,2 - cw 
	m_pnIndices[7] = 2; //2,2,3 - ccw
	m_pnIndices[8] = 2; //2,3,3 - cw  - Degenerated Index(2)
	m_pnIndices[9] = 3; //3,3,7 - ccw - Degenerated Index(3)
	m_pnIndices[10] = 3;//3,7,2 - cw  - Degenerated Index(3)
	m_pnIndices[11] = 7;//7,2,6 - ccw
	m_pnIndices[12] = 2;//2,6,1 - cw
	m_pnIndices[13] = 6;//6,1,5 - ccw
	m_pnIndices[14] = 1;//1,5,0 - cw
	m_pnIndices[15] = 5;//5,0,4 - ccw
	m_pnIndices[16] = 0;
	m_pnIndices[17] = 4;

	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof( UINT ) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = m_pnIndices;
	//인덱스 버퍼를 생성한다.
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer );

	CreateRasterizerState( pd3dDevice );

	//정점 버퍼 데이터를 생성한 다음 최소점과 최대점을 저장한다. 
	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3( -fx, -fy, -fz );
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3( +fx, +fy, +fz );
}


CCellCubeMesh::~CCellCubeMesh()
{
}
void CCellCubeMesh::CreateRasterizerState( ID3D11Device *pd3dDevice )
{
	// 이부분이 실질적으로 행동하는 부분 (추가)
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory( &d3dRasterizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState( &d3dRasterizerDesc, &m_pd3dRasterizerState );
}
void CCellCubeMesh::Render( ID3D11DeviceContext *pd3dDeviceContext )
{
	CMesh::Render( pd3dDeviceContext );
}