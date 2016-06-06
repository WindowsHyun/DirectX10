#include "stdafx.h"
#include "Object.h"


CGameObject::CGameObject()
{
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
	m_pMesh = NULL;
	m_pShader = NULL;

	m_nReferences = 0;
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader) m_pShader->Release();
}

void CGameObject::AddRef()
{
	m_nReferences++;
}

void CGameObject::Release()
{
	if (m_nReferences > 0) m_nReferences--;
	if (m_nReferences <= 0) delete this;
}

void CGameObject::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CGameObject::Animate(float fTimeElapsed)
{
}

void CGameObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if (m_pShader)
	{
		m_pShader->UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);
		m_pShader->Render(pd3dDeviceContext);
	}
	if (m_pMesh) m_pMesh->Render(pd3dDeviceContext);
}


CRotatingObject::CRotatingObject()
{
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float fTimeElapsed)
{
	D3DXMATRIX mtxRotate;
	//y-축 회전 행렬을 생성하고 월드 변환 행렬에 곱한다. 
	D3DXMatrixRotationY(&mtxRotate, (float)D3DXToRadian(45.0f*fTimeElapsed));
	m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
	D3DXMatrixRotationZ( &mtxRotate, (float)D3DXToRadian( 45.0f*fTimeElapsed ) );
	m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
	D3DXMatrixRotationX( &mtxRotate, (float)D3DXToRadian( 45.0f*fTimeElapsed ) );
	m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;

	m_d3dxmtxWorld._41 += moveDirection.x;
	m_d3dxmtxWorld._42 += moveDirection.y;
	m_d3dxmtxWorld._43 += moveDirection.z;

	//moveDirection._41 *= moveSetDirection.x;
	//cout << moveDirection._41 << " * " << moveSetDirection.x << " = " << moveDirection._41 * moveSetDirection.x << endl;
	//m_d3dxmtxWorld += moveDirection;

	//m_d3dxmtxWorld = mtxDirection + m_d3dxmtxWorld;

	//cout << moveSetDirection.x << ", " << moveSetDirection.y << ", " << moveSetDirection.z << endl;
	////cout << m_d3dxmtxWorld._41 << ", " << m_d3dxmtxWorld._42 << ", " << m_d3dxmtxWorld._43 << endl;
	RectangleCD0();
	RectangleCD1();
	RectangleCD2();
	RectangleCD3();
	RectangleCD4();
	RectangleCD5();
}

void CRotatingObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CGameObject::Render(pd3dDeviceContext);
}

void CGameObject::SetPosition( float x, float y, float z )
{
	m_d3dxmtxWorld._41 = x;
	m_d3dxmtxWorld._42 = y;
	m_d3dxmtxWorld._43 = z;
}

void CGameObject::setDirection( float x, float y, float z )
{
	moveDirection.x = x;
	moveDirection.y = y;
	moveDirection.z = z;
}
void CGameObject::SetPosition( D3DXVECTOR3 d3dxvPosition )
{
	m_d3dxmtxWorld._41 = d3dxvPosition.x;
	m_d3dxmtxWorld._42 = d3dxvPosition.y;
	m_d3dxmtxWorld._43 = d3dxvPosition.z;
}

D3DXVECTOR3 CGameObject::GetPosition()
{
	return(D3DXVECTOR3( m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43 ));
}
