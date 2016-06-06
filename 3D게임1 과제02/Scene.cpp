#include "stdafx.h"
#include "Scene.h"


CScene::CScene()
{
	m_ppObjects = NULL;
	m_nObjects = 0;
	m_bObjects = 0;
}

CScene::~CScene()
{
}

void CScene::BuildObjects( ID3D11Device *pd3dDevice )
{
	CShader *pShader = new CShader();
	pShader->CreateShader( pd3dDevice );

	default_random_engine dre;    // 기본 엔진 사용
	dre.seed( (unsigned int)time( NULL ) );
	//uniform_int_distribution<>color( 0, 255 );    // 색상에 대한 분포
	uniform_real_distribution<float>p4( -3, 3 );    //+-
	uniform_real_distribution<float>mP( -10, 10 );    // 위치값
	uniform_real_distribution<float>mD( -0.05, 0.0100000 );    // 움직이는 방향
	uniform_int_distribution<> ui;

	m_nObjects = 50;
	m_ppObjects = new CGameObject*[m_nObjects];
	for ( int i = 0; i < m_nObjects; ++i ) {
		//가로x세로x높이가 15x15x15인 정육면체 메쉬를 생성한다.
		CCubeMesh *pMesh = new CCubeMesh( pd3dDevice, 3.0f, 3.0f, 3.0f );
		CRotatingObject *pObject = new CRotatingObject();
		//pObject->SetPosition( mP(dre), mP( dre ) , mP( dre ) );
		pObject->SetPosition( 0, 0, 0 );
		pObject->setDirection( mD( dre ) * p4(dre), mD( dre ) * p4( dre ), mD( dre ) * p4( dre ) );
		pObject->SetMesh( pMesh );
		pObject->SetShader( pShader );
		m_ppObjects[i] = pObject;
	}



	m_bObjects = 1;
	m_bpObjects = new CGameObject*[m_bObjects];
	for ( int i = 0; i < m_bObjects; ++i ) {
		//가로x세로x높이가 15x15x15인 정육면체 메쉬를 생성한다.
		CBigCubeMesh *pMesh = new CBigCubeMesh( pd3dDevice, 15.0f, 15.0f, 15.0f );
		CRotatingObject *pObject = new CRotatingObject();
		pObject->SetPosition( 0, 0, 0 );
		pObject->SetMesh( pMesh );
		pObject->SetShader( pShader );
		m_bpObjects[i] = pObject;
	}


}

void CScene::ReleaseObjects()
{
	//게임 객체 리스트의 각 객체를 반환(Release)하고 리스트를 소멸시킨다.
	if ( m_ppObjects )
	{
		for ( int j = 0; j < m_nObjects; j++ ) {
			m_ppObjects[j]->Release();
		}
		delete[] m_ppObjects;
	}
}

bool CScene::OnProcessingMouseMessage( HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam )
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage( HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam )
{
	return(false);
}

bool CScene::ProcessInput()
{
	return(false);
}

void CScene::AnimateObjects( float fTimeElapsed )
{
	for ( int i = 0; i < m_nObjects; i++ ) {
		m_ppObjects[i]->Animate( fTimeElapsed );
	}
}

void CScene::Render( ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera )
{
	for ( int i = 0; i < m_nObjects; i++ )
	{
		m_ppObjects[i]->Render( pd3dDeviceContext );
	}
	for ( int i = 0; i < m_bObjects; i++ )
	{
		m_bpObjects[i]->Render( pd3dDeviceContext );
	}
}