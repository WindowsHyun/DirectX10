#pragma once

#define DegreeToRadian(x)	((x)*3.1415926535/180.0)

struct VIEWPORT
{
	float						m_xTopLeft;
	float						m_yTopLeft;
	float						m_nWidth;
	float	 				m_nHeight;
	float					m_vMinDeep;
	float					m_vMaxDeep;
};

class CCamera
{
public:
	CCamera();
	~CCamera();

	//float					m_fxRotation;
	//float					m_fyRotation;
	//float					m_fzRotation;
	D3DXVECTOR3				m_cRotation;		// 회전

	//float					m_fxPosition;
	//float					m_fyPosition;
	//float					m_fzPosition;
	D3DXVECTOR3				m_cPosition;		// 포지션

	VIEWPORT				m_Viewport;

	void Rotate(float fxRotation, float fyRotation, float fzRotation) {
		m_cRotation.x = fxRotation;
		m_cRotation.y = fyRotation;
		m_cRotation.z = fzRotation;
	}
	void SetPosition(float fxPosition, float fyPosition, float fzPosition) {
		m_cPosition.x = fxPosition;
		m_cPosition.y = fyPosition;
		m_cPosition.z = fzPosition;
	}

	void SetViewport(float xTopLeft, float yTopLeft, float nWidth, float nHeight, float zn, float zf);
};

class CGameObject;

class CVertex
{
public:
	CVertex() { }
	CVertex(float x, float y, float z) { 
		m_vPosition = D3DXVECTOR3(x, y, z); 
	}

	D3DXVECTOR3				m_vPosition;
};

class CPolygon
{
public:
	CPolygon(int nVertices);
	virtual ~CPolygon();

	int						m_nVertices;
	CVertex					*m_pVertices;

	void SetVertex(int nIndex, CVertex vertex);
	void Draw(HDC hDCFrameBuffer, CGameObject *pObject, CCamera *pCamera);
};

class CMesh
{
public:
	CMesh(int nPolygons);
	virtual ~CMesh();

private:
	int						m_nReferences;

	int						m_nPolygons;
	CPolygon				**m_ppPolygons;

public:
	void AddRef() { m_nReferences++; }
	void Release() { m_nReferences--; if (m_nReferences <= 0) delete this; }

public:
	void SetPolygon(int nIndex, CPolygon *pPolygon);
	virtual void Render(HDC hDCFrameBuffer, CGameObject *pObject, CCamera *pCamera);
};

class CCubeMesh : public CMesh
{
public:
	CCubeMesh();
	virtual ~CCubeMesh();
};

class BigCubeMesh : public CMesh
{
public:
	BigCubeMesh();
	virtual ~BigCubeMesh();
};

class CGameObject
{
public:
	CGameObject();
	~CGameObject();

public:
	CMesh					*m_pMesh;
	DWORD					m_dwColor;

	D3DXVECTOR3				m_fScale;		// 스케일
	D3DXVECTOR3				m_fRotation;		// 회전
	D3DXVECTOR3				m_fPosition;		// 포지션
	D3DXVECTOR3				m_fVector;
	D3DXVECTOR3				m_vMovingDirection;		// 움직이는 방향

	float					m_fMovingSpeed;
	float					m_fMovingRange;
	float					m_fRotationSpeed;

public:
	void SetMesh(CMesh *pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }
	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }

	void SetScale(float fxScale, float fyScale, float fzScale) {
		//-------------------------------
		m_fScale.x = fxScale;
		m_fScale.y = fyScale;
		m_fScale.z = fzScale;
		//-------------------------------
	}
	void SetRotation(float fxRotation, float fyRotation, float fzRotation) {
		//-------------------------------
		m_fRotation.x = fxRotation;
		m_fRotation.y = fyRotation;
		m_fRotation.z = fzRotation;
		//-------------------------------
		cout << "도형 회전 : " << fxRotation << " | " << fyRotation << " | " << fzRotation << endl;
	}
	void SetPosition(float fxPosition, float fyPosition, float fzPosition) {
		//m_fxPosition = fxPosition;
		//m_fyPosition = fyPosition;
		//m_fzPosition = fzPosition;
		m_fPosition.x = fxPosition;
		m_fPosition.y = fyPosition;
		m_fPosition.z = fzPosition;
		//D3DXMatrixIdentity(&m_fmPosition);	// 단위행렬로 바꾼다.
		//D3DXMatrixTranslation(&m_vNowPosition, fxPosition, fyPosition, fzPosition);
		//cout << "도형 좌표 : " << m_fPosition.x << " | " << m_fPosition.y << " | " << m_fPosition.z << endl;
	}
	void SetMovingDirection(float fxMDirection, float fyMDirection, float fzMDirection) {
		m_vMovingDirection = { fxMDirection, fyMDirection, fzMDirection };
	}
	void Rotate(float fxRotation, float fyRotation, float fzRotation) {
		m_fRotation.x += fxRotation; m_fRotation.y += fyRotation; m_fRotation.z += fzRotation;
	}

	void CubeMove(float m_x, float m_y, float m_z) {

		//D3DXMATRIX  matTranslate;
		//D3DXMatrixTranslation(&matTranslate, m_x, m_y, m_z);
		//if (RectangleCD1()) {

		//}
		//else {
		m_fPosition.x += m_x*m_fMovingSpeed;
		m_fPosition.y += m_y*m_fMovingSpeed;
		m_fPosition.z += m_z*m_fMovingSpeed;
		RectangleCD0();
		RectangleCD1();
		RectangleCD2();
		RectangleCD3();
		RectangleCD4();
		RectangleCD5();
		//}
	}
	/*
	┌-------------┐
	│             │
	│             │
	└-------------┘
	*/
	bool RectangleCD0() {
		D3DXPLANE m_Plane;
		D3DXVECTOR3 m_vPos1(-10, 10, 10), m_vPos2(-10, -10, 10), m_vPos3(-10, -10, -10);
		D3DXPlaneFromPoints(&m_Plane, &m_vPos1, &m_vPos2, &m_vPos3);

		float fResult;
		//D3DXVECTOR3 vPos(m_fPosition.x, m_fPosition.y, m_fPosition.z);
		fResult = D3DXPlaneDotCoord(&m_Plane, &m_fPosition);
		if (fResult <= 0) {
			m_fVector.x = m_Plane.a;
			m_fVector.y = m_Plane.b;
			m_fVector.z = m_Plane.c;
			m_vMovingDirection = m_vMovingDirection - 2 * (D3DXVec3Dot(&m_vMovingDirection, &m_fVector)* m_fVector);
			return true;
		}
		return false;
	}

	bool RectangleCD1() {
		D3DXPLANE m_Plane;
		D3DXVECTOR3 m_vPos1(10, 10, 10), m_vPos2(10, -10, 10), m_vPos3(10, -10, -10);
		D3DXPlaneFromPoints(&m_Plane, &m_vPos1, &m_vPos2, &m_vPos3);

		float fResult;
		fResult = D3DXPlaneDotCoord(&m_Plane, &m_fPosition);

		if (fResult >= 0) {
			/*	D3DXPLANE m_Plane;
				D3DXVECTOR3 m_vPos1(10, 10, 10), m_vPos2(10, -10, 10), m_vPos3(10, -10, -10);
				D3DXPlaneFromPoints(&m_Plane, &m_vPos1, &m_vPos2, &m_vPos3);*/
			m_fVector.x = m_Plane.a;
			m_fVector.y = m_Plane.b;
			m_fVector.z = m_Plane.c;
			m_vMovingDirection = m_vMovingDirection - 2 * (D3DXVec3Dot(&m_vMovingDirection, &m_fVector)* m_fVector);
			return true;
		}
		return false;
	}

	bool RectangleCD2() {
		D3DXPLANE m_Plane;
		D3DXVECTOR3 m_vPos1(-10, 10, 10), m_vPos2(10, 10, 10), m_vPos3(10, 10, -10);
		D3DXPlaneFromPoints(&m_Plane, &m_vPos1, &m_vPos2, &m_vPos3);

		float fResult;
		fResult = D3DXPlaneDotCoord(&m_Plane, &m_fPosition);
		if (fResult >= 0) {
			m_fVector.x = m_Plane.a;
			m_fVector.y = m_Plane.b;
			m_fVector.z = m_Plane.c;
			m_vMovingDirection = m_vMovingDirection - 2 * (D3DXVec3Dot(&m_vMovingDirection, &m_fVector)* m_fVector);
			return true;
		}
		return false;
	}

	bool RectangleCD3() {
		D3DXPLANE m_Plane;
		D3DXVECTOR3 m_vPos1(-10, -10, 10), m_vPos2(10, -10, 10), m_vPos3(10, -10, -10);
		D3DXPlaneFromPoints(&m_Plane, &m_vPos1, &m_vPos2, &m_vPos3);

		float fResult;
		fResult = D3DXPlaneDotCoord(&m_Plane, &m_fPosition);
		if (fResult <= 0) {
			m_fVector.x = m_Plane.a;
			m_fVector.y = m_Plane.b;
			m_fVector.z = m_Plane.c;
			m_vMovingDirection = m_vMovingDirection - 2 * (D3DXVec3Dot(&m_vMovingDirection, &m_fVector)* m_fVector);
			return true;
		}
		return false;
	}

	bool RectangleCD4() {
		D3DXPLANE m_Plane;
		D3DXVECTOR3 m_vPos1(-10, 10, 10), m_vPos2(-10, -10, 10), m_vPos3(10, -10, 10);
		D3DXPlaneFromPoints(&m_Plane, &m_vPos1, &m_vPos2, &m_vPos3);

		float fResult;
		fResult = D3DXPlaneDotCoord(&m_Plane, &m_fPosition);
		if (fResult >= 0) {
			m_fVector.x = m_Plane.a;
			m_fVector.y = m_Plane.b;
			m_fVector.z = m_Plane.c;
			m_vMovingDirection = m_vMovingDirection - 2 * (D3DXVec3Dot(&m_vMovingDirection, &m_fVector)* m_fVector);
			return true;
		}
		return false;
	}

	bool RectangleCD5() {
		D3DXPLANE m_Plane;
		D3DXVECTOR3 m_vPos1(10, -10, -10), m_vPos2(10, 10, -10), m_vPos3(-10, 10, -10);
		D3DXPlaneFromPoints(&m_Plane, &m_vPos1, &m_vPos2, &m_vPos3);

		float fResult;
		fResult = D3DXPlaneDotCoord(&m_Plane, &m_fPosition);
		//cout << fResult << endl;
		if (fResult <= 0) {
			m_fVector.x = m_Plane.a;
			m_fVector.y = m_Plane.b;
			m_fVector.z = m_Plane.c;
			m_vMovingDirection = m_vMovingDirection - 2 * (D3DXVec3Dot(&m_vMovingDirection, &m_fVector)* m_fVector);
			return true;
		}
		return false;
	}

	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);
};

