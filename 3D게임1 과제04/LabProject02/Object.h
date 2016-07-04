#pragma once
#include "Mesh.h"
#include "Camera.h"

#define DIR_FORWARD	0x01
#define DIR_BACKWARD	0x02
#define DIR_LEFT	0x04
#define DIR_RIGHT	0x08
#define DIR_UP		0x10
#define DIR_DOWN	0x20

//재질의 색상을 나타내는 구조체이다.
struct MATERIAL
{
	D3DXCOLOR m_d3dxcAmbient;
	D3DXCOLOR m_d3dxcDiffuse;
	D3DXCOLOR m_d3dxcSpecular; //(r,g,b,a=power)
	D3DXCOLOR m_d3dxcEmissive;
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if ( --m_nReferences <= 0 ) delete this; }

	MATERIAL m_Material;
};


class CGameObject
{
public:
	//게임 객체는 하나의 재질을 가질 수 있다.
	CMaterial *m_pMaterial;
	void SetMaterial( CMaterial *pMaterial );

	CGameObject();
	virtual ~CGameObject();
	CMesh *GetMesh() { return(m_pMesh); }

	//로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe( float fDistance = 1.0f );
	void MoveUp( float fDistance = 1.0f );
	void MoveForward( float fDistance = 1.0f );

	//로컬 x-축, y-축, z-축 방향으로 회전한다.
	void Rotate( float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f );
	void Rotate( D3DXVECTOR3 *pd3dxvAxis, float fAngle );

	//객체의 위치, 로컬 x-축, y-축, z-축 방향 벡터를 반환한다.
	D3DXVECTOR3 GetLookAt();
	D3DXVECTOR3 GetUp();
	D3DXVECTOR3 GetRight();

	// 방향등을 기억하기 위하여 (추가)
	D3DXVECTOR3 moveDirection;

	//객체를 렌더링하기 전에 호출되는 함수이다.
	virtual void OnPrepareRender() { }

private:
	int m_nReferences;
	bool m_bActive;
public:
	void AddRef();
	void Release();

	D3DXMATRIX m_d3dxmtxWorld;
	CMesh *m_pMesh;

	bool IsVisible( CCamera *pCamera = NULL );
	virtual void SetMesh( CMesh *pMesh );

	virtual void Animate( float fTimeElapsed );
	virtual void Render( ID3D11DeviceContext *pd3dDeviceContext );
	virtual void SetPosition( float x, float y, float z );
	virtual void SetPosition( D3DXVECTOR3 d3dxvPosition );

	D3DXVECTOR3 GetPosition();

	void SetActive( bool bActive = false ) { m_bActive = bActive; }

	//월드 좌표계의 픽킹 광선을 생성한다.
	void GenerateRayForPicking( D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition, D3DXVECTOR3 *pd3dxvPickRayDirection );
	//월드 좌표계의 픽킹 광선을 생성한다.
	int PickObjectByRayIntersection( D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo );
};
class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();

	virtual void Animate( float fTimeElapsed );
	virtual void Render( ID3D11DeviceContext *pd3dDeviceContext );
protected:
	//자전 속도와 회전축 벡터를 나타내는 멤버 변수를 선언한다.
	float m_fRotationSpeed;
	D3DXVECTOR3 m_d3dxvRotationAxis;

public:
	//자전 속도와 회전축 벡터를 설정하는 함수이다.
	void SetRotationSpeed( float fRotationSpeed ) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis( D3DXVECTOR3 d3dxvRotationAxis ) { m_d3dxvRotationAxis = d3dxvRotationAxis; }


	bool RectangleCD0() {
		//왼쪽
		D3DXPLANE m_Plane;
		D3DXVECTOR3 m_fVector;
		D3DXVECTOR3 m_vPos1( -17.5, 17.5, 17.5 ), m_vPos2( -17.5, -17.5, 17.5 ), m_vPos3( -17.5, -17.5, -17.5 );
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

	bool RectangleCD1() {
		// 오른쪽
		D3DXPLANE m_Plane;
		D3DXVECTOR3 m_fVector;
		D3DXVECTOR3 m_vPos1( 17.5, -17.5, -17.5 ), m_vPos2( 17.5, -17.5, 17.5 ), m_vPos3( 17.5, 17.5, -17.5 );
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
		D3DXVECTOR3 m_vPos1( -25.0, 25.0, 25.0 ), m_vPos2( 25.0, 25.0, 25.0 ), m_vPos3( 25.0, 25.0, -25.0 );
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
		D3DXVECTOR3 m_vPos1( -8.0, -8.0, 8.0 ), m_vPos2( 8.0, -8.0, 8.0 ), m_vPos3( 8.0, -8.0, -8.0 );
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
		D3DXVECTOR3 m_vPos1( 10, -10, -10 ), m_vPos2( 10, 10, -10 ), m_vPos3( -10, 10, -10 );
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
		D3DXVECTOR3 m_vPos1( -10, 10, 10 ), m_vPos2( -10, -10, 10 ), m_vPos3( 10, -10, 10 );
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

class CRevolvingObject : public CGameObject
{
public:
	CRevolvingObject();
	virtual ~CRevolvingObject();

	virtual void Animate( float fTimeElapsed );

private:
	// 공전 회전축과 회전 속력을 나타낸다.
	D3DXVECTOR3 m_d3dxvRevolutionAxis;
	float m_fRevolutionSpeed;

public:
	// 공전 속력을 설정한다.
	void SetRevolutionSpeed( float fRevolutionSpeed ) { m_fRevolutionSpeed = fRevolutionSpeed; }
	// 공전을 위한 회전축을 설정한다.
	void SetRevolutionAxis( D3DXVECTOR3 d3dxvRevolutionAxis ) { m_d3dxvRevolutionAxis = d3dxvRevolutionAxis; }
};