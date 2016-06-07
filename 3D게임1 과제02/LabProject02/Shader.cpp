#include "stdafx.h"
#include "Shader.h"
//���� ��ȯ ����� ���� ��� ���۴� ���̴� ��ü�� ����(static) ������ ����̴�.
ID3D11Buffer *CShader::m_pd3dcbWorldMatrix = NULL;

CShader::CShader()
{
	m_ppObjects = NULL;
	m_nObjects = 0;

	m_pd3dVertexShader = NULL;
	m_pd3dVertexLayout = NULL;
	m_pd3dPixelShader = NULL;
}

CShader::~CShader()
{
	if ( m_pd3dVertexShader ) m_pd3dVertexShader->Release();
	if ( m_pd3dVertexLayout ) m_pd3dVertexLayout->Release();
	if ( m_pd3dPixelShader ) m_pd3dPixelShader->Release();
}
CGameObject *CShader::PickObjectByRayIntersection( D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo )
{
	int nIntersected = 0;
	float fNearHitDistance = FLT_MAX;
	CGameObject *pSelectedObject = NULL;
	MESHINTERSECTINFO d3dxIntersectInfo;
	/*���̴� ��ü�� ���ԵǴ� ��� ��ü�鿡 ���Ͽ� ��ŷ ������ �����ϰ� ��ü�� �ٿ�� �ڽ����� ������ �˻��Ѵ�. �����ϴ� ��ü�� �߿� ī�޶�� ���� ����� ��ü�� ������ ��ü�� ��ȯ�Ѵ�.*/
	for ( int i = 0; i < m_nObjects; i++ )
	{
		nIntersected = m_ppObjects[i]->PickObjectByRayIntersection( pd3dxvPickPosition, pd3dxmtxView, &d3dxIntersectInfo );
		if ( (nIntersected > 0) && (d3dxIntersectInfo.m_fDistance < fNearHitDistance) )
		{
			fNearHitDistance = d3dxIntersectInfo.m_fDistance;
			pSelectedObject = m_ppObjects[i];
			if ( pd3dxIntersectInfo ) *pd3dxIntersectInfo = d3dxIntersectInfo;
		}
	}
	return(pSelectedObject);
}
void CShader::BuildObjects( ID3D11Device *pd3dDevice )
{
}

void CShader::ReleaseObjects()
{
	if ( m_ppObjects )
	{
		for ( int j = 0; j < m_nObjects; j++ ) if ( m_ppObjects[j] ) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}
}
void CShader::CreateVertexShaderFromFile( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout )
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*����(pszFileName)���� ���̴� �Լ�(pszShaderName)�� �������Ͽ� �����ϵ� ���̴� �ڵ��� �޸� �ּ�(pd3dShaderBlob)�� ��ȯ�Ѵ�.*/
	if ( SUCCEEDED( hResult = D3DX11CompileFromFile( pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL ) ) )
	{
		//�����ϵ� ���̴� �ڵ��� �޸� �ּҿ��� ����-���̴��� �����Ѵ�. 
		pd3dDevice->CreateVertexShader( pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dVertexShader );
		//�����ϵ� ���̴� �ڵ��� �޸� �ּҿ� �Է� ���̾ƿ����� ���� ���̾ƿ��� �����Ѵ�. 
		pd3dDevice->CreateInputLayout( pd3dInputLayout, nElements, pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), ppd3dVertexLayout );
		pd3dShaderBlob->Release();
	}
}
void CShader::CreatePixelShaderFromFile( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader )
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*����(pszFileName)���� ���̴� �Լ�(pszShaderName)�� �������Ͽ� �����ϵ� ���̴� �ڵ��� �޸� �ּ�(pd3dShaderBlob)�� ��ȯ�Ѵ�.*/
	if ( SUCCEEDED( hResult = D3DX11CompileFromFile( pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL ) ) )
	{
		//�����ϵ� ���̴� �ڵ��� �޸� �ּҿ��� �ȼ�-���̴��� �����Ѵ�. 
		pd3dDevice->CreatePixelShader( pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dPixelShader );
		pd3dShaderBlob->Release();
	}
}
void CShader::AnimateObjects( float fTimeElapsed )
{
	for ( int j = 0; j < m_nObjects; j++ )
	{
		m_ppObjects[j]->Animate( fTimeElapsed );
	}
}
void CShader::CreateShader( ID3D11Device *pd3dDevice )
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE( d3dInputLayout );
	CreateVertexShaderFromFile( pd3dDevice, L"Effect.fx", "VS", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout );
	CreatePixelShaderFromFile( pd3dDevice, L"Effect.fx", "PS", "ps_5_0", &m_pd3dPixelShader );
}
void CShader::OnPrepareRender( ID3D11DeviceContext *pd3dDeviceContext )
{
	pd3dDeviceContext->IASetInputLayout( m_pd3dVertexLayout );
	pd3dDeviceContext->VSSetShader( m_pd3dVertexShader, NULL, 0 );
	pd3dDeviceContext->PSSetShader( m_pd3dPixelShader, NULL, 0 );
}

void CShader::Render( ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera )
{
	OnPrepareRender( pd3dDeviceContext );

	for ( int j = 0; j < m_nObjects; j++ )
	{
		if ( m_ppObjects[j] )
		{
			//ī�޶��� ����ü�� ���ԵǴ� ��ü�鸸�� �������Ѵ�. 
			if ( m_ppObjects[j]->IsVisible( pCamera ) )
			{
				m_ppObjects[j]->Render( pd3dDeviceContext );
			}
		}
	}
}
void CShader::CreateShaderVariables( ID3D11Device *pd3dDevice )
{
	//���� ��ȯ ����� ���� ��� ���۸� �����Ѵ�.
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof( bd ) );
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof( VS_CB_WORLD_MATRIX );
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer( &bd, NULL, &m_pd3dcbWorldMatrix );
}
void CShader::ReleaseShaderVariables()
{
	//���� ��ȯ ����� ���� ��� ���۸� ��ȯ�Ѵ�.
	if ( m_pd3dcbWorldMatrix ) m_pd3dcbWorldMatrix->Release();
}
void CShader::UpdateShaderVariable( ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld )
{
	//���� ��ȯ ����� ��� ���ۿ� �����Ѵ�.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map( m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource );
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose( &pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld );
	pd3dDeviceContext->Unmap( m_pd3dcbWorldMatrix, 0 );

	//��� ���۸� ����̽��� ����(VS_SLOT_WORLD_MATRIX)�� �����Ѵ�.
	pd3dDeviceContext->VSSetConstantBuffers( VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix );
}
CSceneShader::CSceneShader()
{
}

CSceneShader::~CSceneShader()
{
}

void CSceneShader::CreateShader( ID3D11Device *pd3dDevice )
{
	CShader::CreateShader( pd3dDevice );
}

void CSceneShader::BuildObjects( ID3D11Device *pd3dDevice )
{



}

CPlayerShader::CPlayerShader()
{
}

CPlayerShader::~CPlayerShader()
{
}
void CPlayerShader::CreateShader( ID3D11Device *pd3dDevice )
{
	CShader::CreateShader( pd3dDevice );
}
void CPlayerShader::BuildObjects( ID3D11Device *pd3dDevice )
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	ID3D11DeviceContext *pd3dDeviceContext = NULL;
	pd3dDevice->GetImmediateContext( &pd3dDeviceContext );

	CMesh *pAirplaneMesh = new CAirplaneMesh( pd3dDevice, 20.0f, 20.0f, 4.0f, D3DCOLOR_XRGB( 0, 255, 0 ) );
	CAirplanePlayer *pAirplanePlayer = new CAirplanePlayer( pd3dDevice );
	pAirplanePlayer->SetMesh( pAirplaneMesh );
	pAirplanePlayer->CreateShaderVariables( pd3dDevice );
	pAirplanePlayer->ChangeCamera( pd3dDevice, SPACESHIP_CAMERA, 0.0f );

	CCamera *pCamera = pAirplanePlayer->GetCamera();
	pCamera->SetViewport( pd3dDeviceContext, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f );
	pCamera->GenerateProjectionMatrix( 1.01f, 50000.0f, ASPECT_RATIO, 60.0f );
	pCamera->GenerateViewMatrix();

	pAirplanePlayer->SetPosition( D3DXVECTOR3( 0.0f, 10.0f, -50.0f ) );
	m_ppObjects[0] = pAirplanePlayer;

	if ( pd3dDeviceContext ) pd3dDeviceContext->Release();
}
void CPlayerShader::Render( ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera )
{
	//3��Ī ī�޶��� �� �÷��̾ �������Ѵ�.
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	if ( nCameraMode == THIRD_PERSON_CAMERA )
	{
		CShader::Render( pd3dDeviceContext, pCamera );
	}
}

CInstancingShader::CInstancingShader()
{
	m_pd3dCubeInstanceBuffer = NULL;
	m_pd3dSphereInstanceBuffer = NULL;
}

CInstancingShader::~CInstancingShader()
{
	if ( m_pd3dCubeInstanceBuffer ) m_pd3dCubeInstanceBuffer->Release();
	if ( m_pd3dSphereInstanceBuffer ) m_pd3dSphereInstanceBuffer->Release();
}
void CInstancingShader::CreateShader( ID3D11Device *pd3dDevice )
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCECOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	UINT nElements = ARRAYSIZE( d3dInputLayout );
	CreateVertexShaderFromFile( pd3dDevice, L"Effect.fx", "VSInstancedDiffusedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout );
	CreatePixelShaderFromFile( pd3dDevice, L"Effect.fx", "PSInstancedDiffusedColor", "ps_5_0", &m_pd3dPixelShader );
}

ID3D11Buffer *CInstancingShader::CreateInstanceBuffer( ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData )
{
	ID3D11Buffer *pd3dInstanceBuffer = NULL;
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	/*������ �ʱ�ȭ �����Ͱ� ������ ���� ���۷� �����Ѵ�. ��, ���߿� ������ �Ͽ� ������ ä��ų� �����Ѵ�.*/
	d3dBufferDesc.Usage = (pBufferData) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = nBufferStride * nObjects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = (pBufferData) ? 0 : D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = pBufferData;
	pd3dDevice->CreateBuffer( &d3dBufferDesc, (pBufferData) ? &d3dBufferData : NULL, &pd3dInstanceBuffer );
	return(pd3dInstanceBuffer);
}
void CInstancingShader::BuildObjects( ID3D11Device *pd3dDevice )
{
	m_nInstanceBufferStride = sizeof( VS_VB_INSTANCE );
	m_nInstanceBufferOffset = 0;

	default_random_engine dre;    // �⺻ ���� ���
	dre.seed( (unsigned int)time( NULL ) );
	//uniform_int_distribution<>color( 0, 255 );    // ���� ���� ����
	uniform_real_distribution<float>p4( -3, 3 );    //+-
	uniform_real_distribution<float>mP( -10, 10 );    // ��ġ��
	uniform_real_distribution<float>mD( -0.0005, 0.0100000 );    // �����̴� ����
	uniform_int_distribution<> ui;

	m_nObjects = 50;
	int nCubeObjects = m_nObjects;
	m_ppObjects = new CGameObject*[m_nObjects];

	CCubeMesh *pCubeMesh = new CCubeMesh( pd3dDevice, 3.0f, 3.0f, 3.0f, D3DCOLOR_XRGB( 0, 0, 0 ) );
	CRotatingObject *pRotatingObject = NULL;

	for ( int i = 0; i < m_nObjects; ++i ) {
		pRotatingObject = new CRotatingObject();
		pRotatingObject->SetMesh( pCubeMesh );
		pRotatingObject->setDirection( mD( dre ) * p4( dre ), mD( dre ) * p4( dre ), mD( dre ) * p4( dre ) );
		pRotatingObject->SetPosition( 0, 10, 0 );
		//pRotatingObject->SetRotationAxis( D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
		pRotatingObject->SetRotationSpeed( 45 );
		m_ppObjects[i] = pRotatingObject;
	}


	m_pd3dCubeInstanceBuffer = CreateInstanceBuffer( pd3dDevice, nCubeObjects, m_nInstanceBufferStride, NULL );
	pCubeMesh->AssembleToVertexBuffer( 1, &m_pd3dCubeInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset );

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// ū�ڽ��� �׸��� ���Ͽ� �߰��Ͽ���. (�߰�)
	m_bnObjects = 1;
	int nBCubeObjects = m_bnObjects;
	m_bppObjects = new CGameObject*[m_bnObjects];

	CBCubeMesh *pBCubeMesh = new CBCubeMesh( pd3dDevice, 35.0f, 35.0f, 35.0f, D3DCOLOR_XRGB( 0, 0, 0 ) );
	CRotatingObject *pBRotatingObject = NULL;

	for ( int i = 0; i < m_bnObjects; ++i ) {
		pBRotatingObject = new CRotatingObject();
		pBRotatingObject->SetMesh( pBCubeMesh );
		pBRotatingObject->setDirection( mD( dre ) * p4( dre ), mD( dre ) * p4( dre ), mD( dre ) * p4( dre ) );
		pBRotatingObject->SetPosition( 0, 10, 0 );
		//pRotatingObject->SetRotationAxis( D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
		pBRotatingObject->SetRotationSpeed( 45 );
		m_bppObjects[i] = pBRotatingObject;
	}


	m_pd3dSphereInstanceBuffer = CreateInstanceBuffer( pd3dDevice, nBCubeObjects, m_nInstanceBufferStride, NULL );
	pBCubeMesh->AssembleToVertexBuffer( 1, &m_pd3dSphereInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset );

}


void CInstancingShader::Render( ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera )
{
	/*
	���̴��� ������ �������� ���⼭ �Ѵ�.
	���⼭ �Ϲ� ť���, �� ť�긦 ���� �������� �Ͽ���.
	�ڲ� ���ٺ��� ť�� ��ȭ ���� �;�����... (�߰�)
	*/
	OnPrepareRender( pd3dDeviceContext );

	CMesh *pCubeMesh = m_ppObjects[0]->GetMesh();

	int nCubeInstances = 0;
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map( m_pd3dCubeInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource );
	VS_VB_INSTANCE *pCubeInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for ( int j = 0; j < m_nObjects; j++ )
	{
		if ( m_ppObjects[j] )
		{
			/*������ü ��ü�� ī�޶� ���� ��(������ü ��ü�� �ٿ�� �ڽ��� ����ü�� ������ ��) �ν��Ͻ� �����͸� �ν��Ͻ� ���۷� ����*/
			if ( m_ppObjects[j]->IsVisible( pCamera ) )
			{
				D3DXMatrixTranspose( &pCubeInstances[nCubeInstances].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld );
				pCubeInstances[nCubeInstances++].m_d3dxColor = (j % 2) ? D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) : RANDOM_COLOR;
			}
		}
	}
	pd3dDeviceContext->Unmap( m_pd3dCubeInstanceBuffer, 0 );

	//ī�޶� ���̴� ������ü�鸸 �ν��Ͻ��� �Ѵ�.
	pCubeMesh->RenderInstanced( pd3dDeviceContext, nCubeInstances, 0 );
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	CMesh *pBCubeMesh = m_bppObjects[0]->GetMesh();

	int nSphereInstances = 0;
	pd3dDeviceContext->Map( m_pd3dSphereInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource );
	VS_VB_INSTANCE *pSphereInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for ( int j = 0; j < m_bnObjects; j++ )
	{
		if ( m_bppObjects[j] )
		{
			/*�� ��ü�� ī�޶� ���� ��(�� ��ü�� �ٿ�� �ڽ��� ����ü�� ������ ��) �ν��Ͻ� �����͸� �ν��Ͻ� ���۷� ����*/
			if ( m_bppObjects[j]->IsVisible( pCamera ) )
			{
				D3DXMatrixTranspose( &pSphereInstances[nSphereInstances].m_d3dxTransform, &m_bppObjects[j]->m_d3dxmtxWorld );
				pSphereInstances[nSphereInstances++].m_d3dxColor = RANDOM_COLOR;
			}
		}
	}
	pd3dDeviceContext->Unmap( m_pd3dSphereInstanceBuffer, 0 );

	//ī�޶� ���̴� ���鸸 �ν��Ͻ��� �Ѵ�.
	pBCubeMesh->RenderInstanced( pd3dDeviceContext, nSphereInstances, 0 );

}




