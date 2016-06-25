#include "stdafx.h"
#include "Shader.h"
//월드 변환 행렬을 위한 상수 버퍼는 쉐이더 객체의 정적(static) 데이터 멤버이다.
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
	/*쉐이더 객체에 포함되는 모든 객체들에 대하여 픽킹 광선을 생성하고 객체의 바운딩 박스와의 교차를 검사한다. 교차하는 객체들 중에 카메라와 가장 가까운 객체의 정보와 객체를 반환한다.*/
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
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if ( SUCCEEDED( hResult = D3DX11CompileFromFile( pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL ) ) )
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 정점-쉐이더를 생성한다. 
		pd3dDevice->CreateVertexShader( pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dVertexShader );
		//컴파일된 쉐이더 코드의 메모리 주소와 입력 레이아웃에서 정점 레이아웃을 생성한다. 
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
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if ( SUCCEEDED( hResult = D3DX11CompileFromFile( pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL ) ) )
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 픽셀-쉐이더를 생성한다. 
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
			//카메라의 절두체에 포함되는 객체들만을 렌더링한다. 
			if ( m_ppObjects[j]->IsVisible( pCamera ) )
			{
				m_ppObjects[j]->Render( pd3dDeviceContext );
			}
		}
	}
}
void CShader::CreateShaderVariables( ID3D11Device *pd3dDevice )
{
	//월드 변환 행렬을 위한 상수 버퍼를 생성한다.
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
	//월드 변환 행렬을 위한 상수 버퍼를 반환한다.
	if ( m_pd3dcbWorldMatrix ) m_pd3dcbWorldMatrix->Release();
}
void CShader::UpdateShaderVariable( ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld )
{
	//월드 변환 행렬을 상수 버퍼에 복사한다.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map( m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource );
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose( &pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld );
	pd3dDeviceContext->Unmap( m_pd3dcbWorldMatrix, 0 );

	//상수 버퍼를 디바이스의 슬롯(VS_SLOT_WORLD_MATRIX)에 연결한다.
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
	//3인칭 카메라일 때 플레이어를 렌더링한다.
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
	/*버퍼의 초기화 데이터가 없으면 동적 버퍼로 생성한다. 즉, 나중에 매핑을 하여 내용을 채우거나 변경한다.*/
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
	Nowscene = 1;


	ifstream inFile( "data.txt" );   //읽을 파일을 연다.
	int num, i = 0, j = 0;
	while ( inFile >> num ) {
		mi_map[i][j] = num;
		++j;
		if ( j >= maze_Data ) {
			i++;
			j = 0;
		}
	}


	m_nInstanceBufferStride = sizeof( VS_VB_INSTANCE );
	m_nInstanceBufferOffset = 0;

	default_random_engine dre;    // 기본 엔진 사용
	dre.seed( (unsigned int)time( NULL ) );
	//uniform_int_distribution<>color( 0, 255 );    // 색상에 대한 분포
	uniform_real_distribution<float>mX( -500, 500 );    //+-
	uniform_real_distribution<float>mZ( 30, 1000 );    // 위치값
	uniform_real_distribution<float>mD( -0.0005, 0.0100000 );    // 움직이는 방향
	uniform_int_distribution<> ui;

	m_nObjects = 10;
	int nCubeObjects = m_nObjects;
	m_ppObjects = new CGameObject*[m_nObjects];

	CCubeMesh *pCubeMesh = new CCubeMesh( pd3dDevice, 10.0f, 10.0f, 10.0f, D3DCOLOR_XRGB( 0, 0, 0 ) );
	CRotatingObject *pRotatingObject = NULL;

	for ( int i = 0; i < m_nObjects; ++i ) {
		pRotatingObject = new CRotatingObject();
		pRotatingObject->SetMesh( pCubeMesh );
		pRotatingObject->SetPosition( 0, 100, 0 );
		pRotatingObject->SetRotationSpeed( 45 );
		m_ppObjects[i] = pRotatingObject;
	}


	m_pd3dCubeInstanceBuffer = CreateInstanceBuffer( pd3dDevice, nCubeObjects, m_nInstanceBufferStride, NULL );
	pCubeMesh->AssembleToVertexBuffer( 1, &m_pd3dCubeInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset );

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 큰박스를 그리기 위하여 추가하였다. (추가)
	m_bnObjects = 1;
	int nBCubeObjects = m_bnObjects;
	m_bppObjects = new CGameObject*[m_bnObjects];

	CBCubeMesh *pBCubeMesh = new CBCubeMesh( pd3dDevice, 10000, 1, 10000, D3DCOLOR_XRGB( 255, 255, 255 ) );
	CRotatingObject *pBRotatingObject = NULL;

	for ( int i = 0; i < m_bnObjects; ++i ) {
		pBRotatingObject = new CRotatingObject();
		pBRotatingObject->SetMesh( pBCubeMesh );
		pBRotatingObject->SetPosition( 0, -28, 500 );
		//pRotatingObject->SetRotationAxis( D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
		pBRotatingObject->SetRotationSpeed( 45 );
		m_bppObjects[i] = pBRotatingObject;
	}


	m_pd3dSphereInstanceBuffer = CreateInstanceBuffer( pd3dDevice, nBCubeObjects, m_nInstanceBufferStride, NULL );
	pBCubeMesh->AssembleToVertexBuffer( 1, &m_pd3dSphereInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset );
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 컴퓨터 객체
	m_cnObjects = 1000;
	c_Value = m_cnObjects;
	int ncCubeInstances = m_cnObjects;
	m_cppObjects = new CGameObject*[m_cnObjects];

	CMCubeMesh *pcCubeMesh = new CMCubeMesh( pd3dDevice, 3.0f, 3.0f, 3.0f, D3DCOLOR_XRGB( 0, 0, 0 ) );
	CRotatingObject *pCRotatingObject = NULL;


	for ( int i = 0; i < m_cnObjects; ++i ) {
		pCRotatingObject = new CRotatingObject();
		pCRotatingObject->SetMesh( pcCubeMesh );
		c_X[i] = mX( dre ), c_Y[i] = -24, c_Z[i] = mZ( dre );
		pCRotatingObject->SetPosition( c_X[i], c_Y[i], c_Z[i] );
		c_View[i] = true;
		pCRotatingObject->SetRotationSpeed( 45 );
		m_cppObjects[i] = pCRotatingObject;
	}


	m_pd3dCCubeInstanceBuffer = CreateInstanceBuffer( pd3dDevice, ncCubeInstances, m_nInstanceBufferStride, NULL );
	pcCubeMesh->AssembleToVertexBuffer( 1, &m_pd3dCCubeInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset );
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 미로 벽을 만들어보자.
	m_MazenObjects = maze_Data*maze_Data;
	int nMazeCubeInstances = m_MazenObjects;
	m_MazeppObjects = new CGameObject*[m_MazenObjects];

	CMazeCubeMesh *pMazeCubeMesh = new CMazeCubeMesh( pd3dDevice, 50, 100, 50, D3DCOLOR_XRGB( 0, 0, 20 ) );
	CRotatingObject *pMazeObject = NULL;
	int x = -40, y = -7, z = 50;
	int k = 0, t = 0;

	for ( int i = 0; i < m_MazenObjects; ++i ) {
		pMazeObject = new CRotatingObject();
		pMazeObject->SetMesh( pMazeCubeMesh );
		
		if ( mi_map[k][t] == 2 ) {
			pMazeObject->SetPosition( x, y, z );
		}
		else {
			pMazeObject->SetPosition( x, -300, z );
		}
			x += 50;
			++t;
			if ( t >= maze_Data ) {
				++k;
				t = 0;
				x = -40;
				z += 50;
			}
		
		pMazeObject->SetRotationSpeed( 0 );
		m_MazeppObjects[i] = pMazeObject;
	}
	m_pd3dMazeInstanceBuffer = CreateInstanceBuffer( pd3dDevice, nMazeCubeInstances, m_nInstanceBufferStride, NULL );
	pMazeCubeMesh->AssembleToVertexBuffer( 1, &m_pd3dMazeInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset );
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 천장을 만들어보자
	m_CellnObjects = maze_Data*maze_Data;
	int nCellCubeInstances = m_CellnObjects;
	m_CellppObjects = new CGameObject*[m_CellnObjects];

	CCellCubeMesh *pCellCubeMesh = new CCellCubeMesh( pd3dDevice, 50, 10, 50, D3DCOLOR_XRGB( 30, 0, 0 ) );
	CRotatingObject *pCellObject = NULL;
	x = -40, y = 48, z = 50;
	k = 0, t = 0;

	for ( int i = 0; i < m_CellnObjects; ++i ) {
		pCellObject = new CRotatingObject();
		pCellObject->SetMesh( pCellCubeMesh );
		pCellObject->SetPosition( x, y, z );
		x += 50;
		++t;
		if ( t >= maze_Data ) {
			++k;
			t = 0;
			x = -40;
			z += 50;
		}
		pCellObject->SetRotationSpeed( 0 );
		m_CellppObjects[i] = pCellObject;
	}
	cout << x << ", " << z << endl;
	m_pd3dCellInstanceBuffer = CreateInstanceBuffer( pd3dDevice, nCellCubeInstances, m_nInstanceBufferStride, NULL );
	pCellCubeMesh->AssembleToVertexBuffer( 1, &m_pd3dCellInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset );

}


void CInstancingShader::Render( ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera )
{
	/*
	쉐이더로 만든후 랜더링은 여기서 한다.
	여기서 일반 큐브와, 빅 큐브를 만들어서 랜더링을 하였다.
	자꾸 보다보니 큐브 영화 보고 싶어진다... (추가)
	*/

	if ( Nowscene == 1 ) {
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
				/*직육면체 객체가 카메라에 보일 때(직육면체 객체의 바운딩 박스가 절두체와 교차할 때) 인스턴싱 데이터를 인스턴스 버퍼로 쓴다*/
				if ( m_ppObjects[j]->IsVisible( pCamera ) )
				{
					D3DXMatrixTranspose( &pCubeInstances[nCubeInstances].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld );
					pCubeInstances[nCubeInstances++].m_d3dxColor = (j % 2) ? D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) : RANDOM_COLOR;
				}
			}
		}
		pd3dDeviceContext->Unmap( m_pd3dCubeInstanceBuffer, 0 );

		//카메라에 보이는 직육면체들만 인스턴싱을 한다.
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
				/*구 객체가 카메라에 보일 때(구 객체의 바운딩 박스가 절두체와 교차할 때) 인스턴싱 데이터를 인스턴스 버퍼로 쓴다*/
				if ( m_bppObjects[j]->IsVisible( pCamera ) )
				{
					D3DXMatrixTranspose( &pSphereInstances[nSphereInstances].m_d3dxTransform, &m_bppObjects[j]->m_d3dxmtxWorld );
					pSphereInstances[nSphereInstances++].m_d3dxColor = RANDOM_COLOR;
				}
			}
		}
		pd3dDeviceContext->Unmap( m_pd3dSphereInstanceBuffer, 0 );

		//카메라에 보이는 구들만 인스턴싱을 한다.
		pBCubeMesh->RenderInstanced( pd3dDeviceContext, nSphereInstances, 0 );
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		CMesh *pCCubeMesh = m_cppObjects[0]->GetMesh();

		int ncCubeInstances = 0;
		pd3dDeviceContext->Map( m_pd3dCCubeInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource );
		VS_VB_INSTANCE *pCCubeInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
		for ( int j = 0; j < m_cnObjects; j++ )
		{
			default_random_engine dre;    // 기본 엔진 사용
			dre.seed( (unsigned int)time( NULL )+ j );
			uniform_real_distribution<float>mD( 0.010, 0.500 );    // 움직이는 방향
			uniform_int_distribution<>mZ( 1, 4 );    // 따라오는 방향
			uniform_int_distribution<> ui;

			switch ( mZ(dre) ) {
			case 1: 
				if ( c_X[j] > p_X )
					c_X[j] -= mD( dre );
				if ( c_Z[j] > p_Z )
					c_Z[j] -= mD( dre );
				break;
			case 2:
				if ( c_Z[j] > p_Z )
					c_Z[j] -= mD( dre );
				if ( c_X[j] < p_X )
					c_X[j] += mD( dre );
				break;
			case 3:
				if ( c_X[j] < p_X )
					c_X[j] += mD( dre );
				if ( c_Z[j] < p_Z )
					c_Z[j] += mD( dre );
				break;
			case 4:
				if ( c_X[j] > p_X )
					c_X[j] -= mD( dre );
				if ( c_Z[j] < p_Z )
					c_Z[j] += mD( dre );
				break;
			}
			m_cppObjects[j]->SetPosition( c_X[j], c_Y[j], c_Z[j] );

			if ( c_X[j] >= p_X - 1.0 && c_X[j] <= p_X + 1.0 ) {
				if ( c_Z[j] >= p_Z - 1.0 && c_Z[j] <= p_Z + 1.0 ) {
					c_View[j] = false;
				}
			}


			if ( m_cppObjects[j] )
			{
				/*구 객체가 카메라에 보일 때(구 객체의 바운딩 박스가 절두체와 교차할 때) 인스턴싱 데이터를 인스턴스 버퍼로 쓴다*/
				if ( m_cppObjects[j]->IsVisible( pCamera ) && c_View[j] == true )
				{
					D3DXMatrixTranspose( &pCCubeInstances[ncCubeInstances].m_d3dxTransform, &m_cppObjects[j]->m_d3dxmtxWorld );
					pCCubeInstances[ncCubeInstances++].m_d3dxColor = RANDOM_COLOR;
				}
			}
		}
		pd3dDeviceContext->Unmap( m_pd3dCCubeInstanceBuffer, 0 );

		//카메라에 보이는 구들만 인스턴싱을 한다.
		pCCubeMesh->RenderInstanced( pd3dDeviceContext, ncCubeInstances, 0 );
	}


	if ( Nowscene == 3 ) {
		OnPrepareRender( pd3dDeviceContext );

		CMesh *pCubeMesh = m_ppObjects[0]->GetMesh();

		int nCubeInstances = 0;
		D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
		pd3dDeviceContext->Map( m_pd3dCubeInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource );
		VS_VB_INSTANCE *pCubeInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
		for ( int j = 0; j < m_nObjects; j++ )
		{
			m_ppObjects[j]->SetPosition( 40, 0, 1560 );
			if ( m_ppObjects[j] )
			{
				/*직육면체 객체가 카메라에 보일 때(직육면체 객체의 바운딩 박스가 절두체와 교차할 때) 인스턴싱 데이터를 인스턴스 버퍼로 쓴다*/
				if ( m_ppObjects[j]->IsVisible( pCamera ) )
				{
					D3DXMatrixTranspose( &pCubeInstances[nCubeInstances].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld );
					pCubeInstances[nCubeInstances++].m_d3dxColor = (j % 2) ? D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) : RANDOM_COLOR;
				}
			}
		}
		pd3dDeviceContext->Unmap( m_pd3dCubeInstanceBuffer, 0 );

		//카메라에 보이는 직육면체들만 인스턴싱을 한다.
		pCubeMesh->RenderInstanced( pd3dDeviceContext, nCubeInstances, 0 );
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		CMesh *pBCubeMesh = m_bppObjects[0]->GetMesh();

		int nSphereInstances = 0;
		pd3dDeviceContext->Map( m_pd3dSphereInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource );
		VS_VB_INSTANCE *pSphereInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
		for ( int j = 0; j < m_bnObjects; j++ )
		{
			if ( m_bppObjects[j] )
			{
				/*구 객체가 카메라에 보일 때(구 객체의 바운딩 박스가 절두체와 교차할 때) 인스턴싱 데이터를 인스턴스 버퍼로 쓴다*/
				if ( m_bppObjects[j]->IsVisible( pCamera ) )
				{
					D3DXMatrixTranspose( &pSphereInstances[nSphereInstances].m_d3dxTransform, &m_bppObjects[j]->m_d3dxmtxWorld );
					pSphereInstances[nSphereInstances++].m_d3dxColor = RANDOM_COLOR;
				}
			}
		}
		pd3dDeviceContext->Unmap( m_pd3dSphereInstanceBuffer, 0 );

		//카메라에 보이는 구들만 인스턴싱을 한다.
		pBCubeMesh->RenderInstanced( pd3dDeviceContext, nSphereInstances, 0 );
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		CMesh *pMazeMesh = m_MazeppObjects[0]->GetMesh();

		int nMazeInstances = 0;
		pd3dDeviceContext->Map( m_pd3dMazeInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource );
		VS_VB_INSTANCE *pMazeInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
		for ( int j = 0; j < m_MazenObjects; j++ )
		{
			if ( m_MazeppObjects[j] )
			{
				/*구 객체가 카메라에 보일 때(구 객체의 바운딩 박스가 절두체와 교차할 때) 인스턴싱 데이터를 인스턴스 버퍼로 쓴다*/
				if ( m_MazeppObjects[j]->IsVisible( pCamera ) )
				{
					D3DXMatrixTranspose( &pMazeInstances[nMazeInstances].m_d3dxTransform, &m_MazeppObjects[j]->m_d3dxmtxWorld );
					pMazeInstances[nMazeInstances++].m_d3dxColor = D3DXCOLOR( 0.0f, 0.0f, 1.0f, 1.0f );
				}
			}
		}
		pd3dDeviceContext->Unmap( m_pd3dMazeInstanceBuffer, 0 );

		//카메라에 보이는 구들만 인스턴싱을 한다.
		pMazeMesh->RenderInstanced( pd3dDeviceContext, nMazeInstances, 0 );
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		CMesh *pCellMesh = m_CellppObjects[0]->GetMesh();

		int nCellInstances = 0;
		pd3dDeviceContext->Map( m_pd3dCellInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource );
		VS_VB_INSTANCE *pCellInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
		for ( int j = 0; j < m_CellnObjects; j++ )
		{
			if ( m_CellppObjects[j] )
			{
				/*구 객체가 카메라에 보일 때(구 객체의 바운딩 박스가 절두체와 교차할 때) 인스턴싱 데이터를 인스턴스 버퍼로 쓴다*/
				if ( m_CellppObjects[j]->IsVisible( pCamera ) )
				{
					D3DXMatrixTranspose( &pCellInstances[nCellInstances].m_d3dxTransform, &m_CellppObjects[j]->m_d3dxmtxWorld );
					pCellInstances[nCellInstances++].m_d3dxColor = RANDOM_COLOR;
				}
			}
		}
		pd3dDeviceContext->Unmap( m_pd3dCellInstanceBuffer, 0 );

		//카메라에 보이는 구들만 인스턴싱을 한다.
		pCellMesh->RenderInstanced( pd3dDeviceContext, nCellInstances, 0 );
	}







}















