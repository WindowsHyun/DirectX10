#include "stdafx.h"
#include "Scene.h"

CScene::CScene()
{
	m_ppShaders = NULL;
	m_pShaders = NULL;
	m_nShaders = 0;
	m_pSelectedObject = NULL;
	m_pLights = NULL;
	m_pd3dcbLights = NULL;
}

CScene::~CScene()
{
}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nShaders = 1;
	m_pShaders = new CInstancingShader[m_nShaders];
	m_pShaders[0].CreateShader( pd3dDevice );
	m_pShaders[0].BuildObjects( pd3dDevice );
}

void CScene::ReleaseObjects()
{
	ReleaseShaderVariables();

	for ( int j = 0; j < m_nShaders; j++ )
	{
		if ( m_ppShaders[j] ) m_ppShaders[j]->ReleaseObjects();
		if ( m_ppShaders[j] ) delete m_ppShaders[j];
	}
	if ( m_ppShaders ) delete[] m_ppShaders;
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		/*윈도우의 클라이언트 영역에서 왼쪽 마우스 버튼이 눌려지면 마우스의 위치를 사용하여 픽킹을 처리한다. 마우스 픽킹으로 선택된 객체가 있으면 그 객체를 비활성화한다.*/
		cout << LOWORD( lParam ) << ", " << HIWORD( lParam ) << endl;
		if (m_pSelectedObject = PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam)))
			m_pSelectedObject->SetActive(false);
		
		break;
	}
	return 0;
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput()
{
	return(false);
}
CGameObject *CScene::PickObjectPointedByCursor(int xClient, int yClient)
{
	if (!m_pCamera) return(NULL);

	D3DXMATRIX d3dxmtxView = m_pCamera->GetViewMatrix();
	D3DXMATRIX d3dxmtxProjection = m_pCamera->GetProjectionMatrix();
	D3D11_VIEWPORT d3dViewport = m_pCamera->GetViewport();

	D3DXVECTOR3 d3dxvPickPosition;
	/*화면 좌표계의 점 (xClient, yClient)를 화면 좌표 변환의 역변환과 투영 변환의 역변환을 한다. 그 결과는 카메라 좌표계의 점이다. 투영 평면이 카메라에서 z-축으로 거리가 1이므로 z-좌표는 1로 설정한다.*/
	d3dxvPickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / d3dxmtxProjection._11;
	d3dxvPickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / d3dxmtxProjection._22;
	d3dxvPickPosition.z = 1.0f;

	cout << d3dxvPickPosition.x << ", " << d3dxvPickPosition.y << ", " << d3dxvPickPosition.z << endl;

	int nIntersected = 0;
	float fNearHitDistance = FLT_MAX;
	MESHINTERSECTINFO d3dxIntersectInfo;
	CGameObject *pIntersectedObject = NULL, *pNearestObject = NULL;
	//씬의 모든 쉐이더 객체에 대하여 픽킹을 처리하여 카메라와 가장 가까운 픽킹된 객체를 찾는다.
	for (int i = 0; i < m_nShaders; i++)
	{
		pIntersectedObject = m_pShaders[i].PickObjectByRayIntersection(&d3dxvPickPosition, &d3dxmtxView, &d3dxIntersectInfo);
		if (pIntersectedObject && (d3dxIntersectInfo.m_fDistance < fNearHitDistance))
		{
			fNearHitDistance = d3dxIntersectInfo.m_fDistance;
			pNearestObject = pIntersectedObject;
		}
	}
	return(pNearestObject);
}
void CScene::AnimateObjects(float fTimeElapsed)
{
	if ( m_pLights && m_pd3dcbLights )
	{
		//현재 카메라의 위치 벡터를 조명을 나타내는 상수 버퍼에 설정한다.
		D3DXVECTOR3 d3dxvCameraPosition = m_pCamera->GetPosition();
		m_pLights->m_d3dxvCameraPosition = D3DXVECTOR4( d3dxvCameraPosition, 1.0f );

		//점 조명이 지형의 중앙을 중심으로 회전하도록 설정한다.
		CHeightMapTerrain *pTerrain = GetTerrain();
		static D3DXVECTOR3 d3dxvRotated = D3DXVECTOR3( pTerrain->GetWidth()*0.3f, 0.0f, 0.0f );
		D3DXMATRIX d3dxmtxRotate;
		D3DXMatrixRotationYawPitchRoll( &d3dxmtxRotate, (float)D3DXToRadian( 30.0f*fTimeElapsed ), 0.0f, 0.0f );
		D3DXVec3TransformCoord( &d3dxvRotated, &d3dxvRotated, &d3dxmtxRotate );
		D3DXVECTOR3 d3dxvTerrainCenter = D3DXVECTOR3( pTerrain->GetWidth()*0.5f, pTerrain->GetPeakHeight() + 10.0f, pTerrain->GetLength()*0.5f );
		m_pLights->m_pLights[0].m_d3dxvPosition = d3dxvTerrainCenter + d3dxvRotated;
		m_pLights->m_pLights[0].m_fRange = pTerrain->GetPeakHeight();

		/*두 번째 조명은 플레이어가 가지고 있는 손전등(스팟 조명)이다. 그러므로 플레이어의 위치와 방향이 바뀌면 현재 플레이어의 위치와 z-축 방향 벡터를 스팟 조명의 위치와 방향으로 설정한다.*/
		CPlayer *pPlayer = m_pCamera->GetPlayer();
		m_pLights->m_pLights[1].m_d3dxvPosition = pPlayer->GetPosition();
		m_pLights->m_pLights[1].m_d3dxvDirection = pPlayer->GetLookVector();

		m_pLights->m_pLights[3].m_d3dxvPosition = pPlayer->GetPosition() + D3DXVECTOR3( 0.0f, 40.0f, 0.0f );
	}

	for ( int i = 0; i < m_nShaders; i++ )
	{
		m_ppShaders[i]->AnimateObjects( fTimeElapsed );
	}

	//for (int i = 0; i < m_nShaders; i++)
	//{
	//	m_pShaders[i].AnimateObjects(fTimeElapsed);
	//}
}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera)
{
	if ( m_pLights && m_pd3dcbLights ) UpdateShaderVariable( pd3dDeviceContext, m_pLights );

	for ( int i = 0; i < m_nShaders; i++ )
	{
		m_ppShaders[i]->Render( pd3dDeviceContext, pCamera );
	}

	//for (int i = 0; i < m_nShaders; i++)
	//{
	//	m_pShaders[i].Render(pd3dDeviceContext, pCamera);
	//}
}

void CScene::CreateShaderVariables( ID3D11Device *pd3dDevice )
{
	m_pLights = new LIGHTS;
	::ZeroMemory( m_pLights, sizeof( LIGHTS ) );
	//게임 월드 전체를 비추는 주변조명을 설정한다.
	m_pLights->m_d3dxcGlobalAmbient = D3DXCOLOR( 0.1f, 0.1f, 0.1f, 1.0f );

	//3개의 조명(점 광원, 스팟 광원, 방향성 광원)을 설정한다.
	m_pLights->m_pLights[0].m_bEnable = 1.0f;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 300.0f;
	m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR( 0.1f, 0.0f, 0.0f, 1.0f );
	m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f );
	m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR( 0.1f, 0.1f, 0.1f, 0.0f );
	m_pLights->m_pLights[0].m_d3dxvPosition = D3DXVECTOR3( 300.0f, 300.0f, 300.0f );
	m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_pLights->m_pLights[0].m_d3dxvAttenuation = D3DXVECTOR3( 1.0f, 0.001f, 0.0001f );
	m_pLights->m_pLights[1].m_bEnable = 1.0f;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 100.0f;
	m_pLights->m_pLights[1].m_d3dxcAmbient = D3DXCOLOR( 0.1f, 0.1f, 0.1f, 1.0f );
	m_pLights->m_pLights[1].m_d3dxcDiffuse = D3DXCOLOR( 0.3f, 0.3f, 0.3f, 1.0f );
	m_pLights->m_pLights[1].m_d3dxcSpecular = D3DXCOLOR( 0.1f, 0.1f, 0.1f, 0.0f );
	m_pLights->m_pLights[1].m_d3dxvPosition = D3DXVECTOR3( 500.0f, 300.0f, 500.0f );
	m_pLights->m_pLights[1].m_d3dxvDirection = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	m_pLights->m_pLights[1].m_d3dxvAttenuation = D3DXVECTOR3( 1.0f, 0.01f, 0.0001f );
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos( D3DXToRadian( 40.0f ) );
	m_pLights->m_pLights[1].m_fTheta = (float)cos( D3DXToRadian( 20.0f ) );
	m_pLights->m_pLights[2].m_bEnable = 1.0f;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_d3dxcAmbient = D3DXCOLOR( 0.1f, 0.1f, 0.1f, 1.0f );
	m_pLights->m_pLights[2].m_d3dxcDiffuse = D3DXCOLOR( 0.2f, 0.2f, 0.2f, 1.0f );
	m_pLights->m_pLights[2].m_d3dxcSpecular = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f );
	m_pLights->m_pLights[2].m_d3dxvDirection = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
	m_pLights->m_pLights[3].m_bEnable = 1.0f;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 60.0f;
	m_pLights->m_pLights[3].m_d3dxcAmbient = D3DXCOLOR( 0.1f, 0.0f, 0.0f, 1.0f );
	m_pLights->m_pLights[3].m_d3dxcDiffuse = D3DXCOLOR( 0.5f, 0.0f, 0.0f, 1.0f );
	m_pLights->m_pLights[3].m_d3dxcSpecular = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f );
	m_pLights->m_pLights[3].m_d3dxvPosition = D3DXVECTOR3( 500.0f, 300.0f, 500.0f );
	m_pLights->m_pLights[3].m_d3dxvDirection = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
	m_pLights->m_pLights[3].m_d3dxvAttenuation = D3DXVECTOR3( 1.0f, 0.01f, 0.0001f );
	m_pLights->m_pLights[3].m_fFalloff = 20.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos( D3DXToRadian( 40.0f ) );
	m_pLights->m_pLights[3].m_fTheta = (float)cos( D3DXToRadian( 15.0f ) );

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory( &d3dBufferDesc, sizeof( d3dBufferDesc ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof( LIGHTS );
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = m_pLights;
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dcbLights );
}

void CScene::ReleaseShaderVariables()
{
	if ( m_pLights ) delete m_pLights;
	if ( m_pd3dcbLights ) m_pd3dcbLights->Release();
}

void CScene::UpdateShaderVariable( ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights );
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map( m_pd3dcbLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource );
	LIGHTS *pcbLight = (LIGHTS *)d3dMappedResource.pData;
	memcpy( pcbLight, pLights, sizeof( LIGHTS ) );
	pd3dDeviceContext->Unmap( m_pd3dcbLights, 0 );
	pd3dDeviceContext->PSSetConstantBuffers( PS_SLOT_LIGHT, 1, &m_pd3dcbLights );
}

