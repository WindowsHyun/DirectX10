#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"


class CGameObject
{
public:
	//객체의 위치를 설정한다.
	virtual void SetPosition( float x, float y, float z );
	virtual void SetPosition( D3DXVECTOR3 d3dxvPosition );
	virtual void setDirection( float x, float y, float z );
	D3DXVECTOR3 GetPosition();

	CGameObject();
	virtual ~CGameObject();

private:
	int m_nReferences;

public:
	void AddRef();
	void Release();

	float directionMove;
	D3DXVECTOR3 moveDirection;
	D3DXVECTOR3 moveSetDirection;
	D3DXMATRIX m_d3dxmtxWorld;
	CMesh *m_pMesh;
	CShader *m_pShader;

	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};


class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

	bool RectangleCD0() {
		//왼쪽
		D3DXPLANE m_Plane;
		D3DXVECTOR3 m_fVector;
		D3DXVECTOR3 m_vPos1( -19.5, 19.5, 19.5 ), m_vPos2( -19.5, -19.5, 19.5 ), m_vPos3( -19.5, -19.5, -19.5 );
		D3DXPlaneFromPoints( &m_Plane, &m_vPos1, &m_vPos2, &m_vPos3 );
		float fResult;
		//D3DXVECTOR3 vPos(m_fPosition.x, m_fPosition.y, m_fPosition.z);
		D3DXVECTOR3 vD3dmtx( m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43 );

		fResult = D3DXPlaneDotCoord( &m_Plane, &vD3dmtx );
		if ( fResult <= 0 ) {
			m_fVector.x = m_Plane.a;
			m_fVector.y = m_Plane.b;
			m_fVector.z = m_Plane.c;
			moveDirection = moveDirection - 2 * (D3DXVec3Dot( &moveDirection, &m_fVector )* m_fVector);
			//cout << moveDirection.x << ", " << moveDirection.y << ", " << moveDirection.z << endl;
			return true;
		}
		return false;
	}

	bool RectangleCD1() {
		// 오른쪽
		D3DXPLANE m_Plane;
		D3DXVECTOR3 m_fVector;
		D3DXVECTOR3 m_vPos1( 19.5, -19.5, -19.5 ), m_vPos2( 19.5, -19.5, 19.5 ), m_vPos3( 19.5, 19.5, -19.5 );
		D3DXPlaneFromPoints( &m_Plane, &m_vPos1, &m_vPos2, &m_vPos3 );
		float fResult;
		//D3DXVECTOR3 vPos(m_fPosition.x, m_fPosition.y, m_fPosition.z);
		D3DXVECTOR3 vD3dmtx( m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43 );

		fResult = D3DXPlaneDotCoord( &m_Plane, &vD3dmtx );
		//cout << fResult << endl;
		if ( fResult <= 0 ) {
			m_fVector.x = m_Plane.a;
			m_fVector.y = m_Plane.b;
			m_fVector.z = m_Plane.c;
			moveDirection = moveDirection - 2 * (D3DXVec3Dot( &moveDirection, &m_fVector )* m_fVector);
			//cout << moveDirection.x << ", " << moveDirection.y << ", " << moveDirection.z << endl;
			return true;
		}
		return false;
	}

	bool RectangleCD2() {
		// 위
		D3DXPLANE m_Plane;
		D3DXVECTOR3 m_fVector;
		//D3DXVECTOR3 m_vPos1( -19.5, -19.5, 19.5 ), m_vPos2( 19.5, -19.5, 19.5 ), m_vPos3( 19.5, -19.5, -19.5 );
		D3DXVECTOR3 m_vPos1( -19.5, 19.5, 19.5 ), m_vPos2( 19.5, 19.5, 19.5 ), m_vPos3( 19.5, 19.5, -19.5 );
		D3DXPlaneFromPoints( &m_Plane, &m_vPos1, &m_vPos2, &m_vPos3 );
		float fResult;
		//D3DXVECTOR3 vPos(m_fPosition.x, m_fPosition.y, m_fPosition.z);
		D3DXVECTOR3 vD3dmtx( m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43 );

		fResult = D3DXPlaneDotCoord( &m_Plane, &vD3dmtx );
		//cout << fResult << endl;
		if ( fResult >= 0 ) {
			m_fVector.x = m_Plane.a;
			m_fVector.y = m_Plane.b;
			m_fVector.z = m_Plane.c;
			moveDirection = moveDirection - 2 * (D3DXVec3Dot( &moveDirection, &m_fVector )* m_fVector);
			//cout << moveDirection.x << ", " << moveDirection.y << ", " << moveDirection.z << endl;
			return true;
		}
		return false;
	}

	bool RectangleCD3() {
		// 아래
		D3DXPLANE m_Plane;
		D3DXVECTOR3 m_fVector;
		D3DXVECTOR3 m_vPos1( -19.5, -19.5, 19.5 ), m_vPos2( 19.5, -19.5, 19.5 ), m_vPos3( 19.5, -19.5, -19.5 );
		D3DXPlaneFromPoints( &m_Plane, &m_vPos1, &m_vPos2, &m_vPos3 );
		float fResult;
		D3DXVECTOR3 vD3dmtx( m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43 );

		fResult = D3DXPlaneDotCoord( &m_Plane, &vD3dmtx );
		//cout << fResult << endl;
		if ( fResult <= 0 ) {
			m_fVector.x = m_Plane.a;
			m_fVector.y = m_Plane.b;
			m_fVector.z = m_Plane.c;
			moveDirection = moveDirection - 2 * (D3DXVec3Dot( &moveDirection, &m_fVector )* m_fVector);
			return true;
		}
		return false;
	}

	bool RectangleCD4() {
		// 정면
		D3DXPLANE m_Plane;
		D3DXVECTOR3 m_fVector;
		D3DXVECTOR3 m_vPos1( 13, -13, -13 ), m_vPos2( 13, 13, -13 ), m_vPos3( -13, 13, -13 );
		D3DXPlaneFromPoints( &m_Plane, &m_vPos1, &m_vPos2, &m_vPos3 );
		float fResult;
		D3DXVECTOR3 vD3dmtx( m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43 );

		fResult = D3DXPlaneDotCoord( &m_Plane, &vD3dmtx );
		if ( fResult <= 0 ) {
			m_fVector.x = m_Plane.a;
			m_fVector.y = m_Plane.b;
			m_fVector.z = m_Plane.c;
			moveDirection = moveDirection - 2 * (D3DXVec3Dot( &moveDirection, &m_fVector )* m_fVector);
			return true;
		}
		return false;
	}

	bool RectangleCD5() {
		// 후면
		D3DXPLANE m_Plane;
		D3DXVECTOR3 m_fVector;
		D3DXVECTOR3 m_vPos1( -13, 13, 13 ), m_vPos2( -13, -13, 13 ), m_vPos3( 13, -13, 13 );
		D3DXPlaneFromPoints( &m_Plane, &m_vPos1, &m_vPos2, &m_vPos3 );
		float fResult;
		D3DXVECTOR3 vD3dmtx( m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43 );

		fResult = D3DXPlaneDotCoord( &m_Plane, &vD3dmtx );
		if ( fResult >= 0 ) {
			m_fVector.x = m_Plane.a;
			m_fVector.y = m_Plane.b;
			m_fVector.z = m_Plane.c;
			moveDirection = moveDirection - 2 * (D3DXVec3Dot( &moveDirection, &m_fVector )* m_fVector);
			return true;
		}
		return false;
	}



};