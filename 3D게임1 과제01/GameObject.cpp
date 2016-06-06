#include "StdAfx.h"
#include "GameObject.h"

CPolygon::CPolygon(int nVertices)
{
	if (nVertices > 0)
	{
		m_nVertices = nVertices;
		m_pVertices = new CVertex[nVertices];
	}
}

CPolygon::~CPolygon()
{
	if (m_pVertices) delete[] m_pVertices;
}

void CPolygon::SetVertex(int nIndex, CVertex vertex)
{
	if ((0 <= nIndex) && (nIndex < m_nVertices) && m_pVertices)
	{
		m_pVertices[nIndex] = vertex;
	}
}

void CPolygon::Draw(HDC hDCFrameBuffer, CGameObject *pObject, CCamera *pCamera)
{
	CVertex vertex;
	VIEWPORT viewport;
	D3DXVECTOR3 vPrevious, vCurrent, vRotated;
	D3DXMATRIX mtxPrevious, mtxRotate, mtxTransform, mtxPerspective, mtxScreen, mtxWorld;
	D3DXMatrixIdentity(&mtxWorld);
	D3DXMatrixIdentity(&mtxRotate);
	D3DXMatrixIdentity(&mtxTransform);
	D3DXMatrixIdentity(&mtxPerspective);
	D3DXMatrixIdentity(&mtxScreen);
	for (int i = 0; i <= m_nVertices; i++) {
		vertex = m_pVertices[i % m_nVertices];
		vRotated = vCurrent = vertex.m_vPosition;
		//Scale Transformation
	/*	vCurrent.x *= pObject->m_fScale.x;
		vCurrent.y *= pObject->m_fScale.y;
		vCurrent.z *= pObject->m_fScale.z;*/
		D3DXMatrixScaling(&mtxWorld, pObject->m_fScale.x, pObject->m_fScale.y, pObject->m_fScale.z);
		//Rotation Transformation
		if (pObject->m_fRotation.x != 0.0f)
		{
			/*vRotated.y = float(vCurrent.y * cos(DegreeToRadian(pObject->m_fRotation.x)) - vCurrent.z * sin(DegreeToRadian(pObject->m_fRotation.x)));
			vRotated.z = float(vCurrent.y * sin(DegreeToRadian(pObject->m_fRotation.x)) + vCurrent.z * cos(DegreeToRadian(pObject->m_fRotation.x)));
			vCurrent = vRotated;*/
			mtxRotate._22 = cos(DegreeToRadian(pObject->m_fRotation.x));
			mtxRotate._23 = sin(DegreeToRadian(pObject->m_fRotation.x));
			mtxRotate._32 = -sin(DegreeToRadian(pObject->m_fRotation.x));
			mtxRotate._33 = cos(DegreeToRadian(pObject->m_fRotation.x));
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotate);
			D3DXMatrixIdentity(&mtxRotate);
		}
		if (pObject->m_fRotation.y != 0.0f)
		{
			/*vRotated.x = float(vCurrent.x * cos(DegreeToRadian(pObject->m_fRotation.y)) + vCurrent.z * sin(DegreeToRadian(pObject->m_fRotation.y)));
			vRotated.z = float(-vCurrent.x * sin(DegreeToRadian(pObject->m_fRotation.y)) + vCurrent.z * cos(DegreeToRadian(pObject->m_fRotation.y)));
			vCurrent = vRotated;*/
			mtxRotate._11 = cos(DegreeToRadian(pObject->m_fRotation.y));
			mtxRotate._13 = -sin(DegreeToRadian(pObject->m_fRotation.y));
			mtxRotate._31 = sin(DegreeToRadian(pObject->m_fRotation.y));
			mtxRotate._33 = cos(DegreeToRadian(pObject->m_fRotation.y));
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotate);

			D3DXMatrixIdentity(&mtxRotate);
		}
		if (pObject->m_fRotation.z != 0.0f)
		{
			/*vRotated.x = float(vCurrent.x * cos(DegreeToRadian(pObject->m_fRotation.z)) - vCurrent.y * sin(DegreeToRadian(pObject->m_fRotation.z)));
			vRotated.y = float(vCurrent.x * sin(DegreeToRadian(pObject->m_fRotation.z)) + vCurrent.y * cos(DegreeToRadian(pObject->m_fRotation.z)));
			vCurrent = vRotated;*/
			mtxRotate._11 = cos(DegreeToRadian(pObject->m_fRotation.z));
			mtxRotate._12 = sin(DegreeToRadian(pObject->m_fRotation.z));
			mtxRotate._21 = -sin(DegreeToRadian(pObject->m_fRotation.z));
			mtxRotate._22 = cos(DegreeToRadian(pObject->m_fRotation.z));
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotate);

			D3DXMatrixIdentity(&mtxRotate);
		}
		//Translation Transformation
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		/*지워야할부분↓*/
		/*vCurrent.x += pObject->m_fPosition.x;
		vCurrent.y += pObject->m_fPosition.y;
		vCurrent.z += pObject->m_fPosition.z;*/
		/*
		vCurrent.x += pObject->m_fxPosition;
		vCurrent.y += pObject->m_fyPosition;
		vCurrent.z += pObject->m_fzPosition;
		*/
		D3DXMatrixTranslation(&mtxTransform, pObject->m_fPosition.x, pObject->m_fPosition.y, pObject->m_fPosition.z);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTransform);

		D3DXMatrixIdentity(&mtxTransform);

		//View Transformation
		/*지워야할부분↓*/
		/*vCurrent.x -= pCamera->m_cPosition.x;
		vCurrent.y -= pCamera->m_cPosition.y;
		vCurrent.z -= pCamera->m_cPosition.z;*/

		D3DXMatrixTranslation(&mtxTransform, -pCamera->m_cPosition.x, -pCamera->m_cPosition.y, -pCamera->m_cPosition.z);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTransform);

		D3DXMatrixIdentity(&mtxTransform);


		if (pCamera->m_cRotation.x != 0.0f)
		{
			/*vRotated.y = float(vCurrent.y * cos(DegreeToRadian(-pCamera->m_cRotation.x)) - vCurrent.z * sin(DegreeToRadian(-pCamera->m_cRotation.x)));
			vRotated.z = float(vCurrent.y * sin(DegreeToRadian(-pCamera->m_cRotation.x)) + vCurrent.z * cos(DegreeToRadian(-pCamera->m_cRotation.x)));
			vCurrent.x = vRotated.x;
			vCurrent.y = vRotated.y;*/

			mtxRotate._22 = cos(DegreeToRadian(pCamera->m_cRotation.x));
			mtxRotate._23 = sin(DegreeToRadian(pCamera->m_cRotation.x));
			mtxRotate._32 = -sin(DegreeToRadian(pCamera->m_cRotation.x));
			mtxRotate._33 = cos(DegreeToRadian(pCamera->m_cRotation.x));
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotate);
			D3DXMatrixIdentity(&mtxRotate);

		}
		if (pCamera->m_cRotation.y != 0.0f)
		{
			/*vRotated.x = float(vCurrent.x * cos(DegreeToRadian(-pCamera->m_cRotation.y)) + vCurrent.z * sin(DegreeToRadian(-pCamera->m_cRotation.y)));
			vRotated.z = float(-vCurrent.x * sin(DegreeToRadian(-pCamera->m_cRotation.y)) + vCurrent.z * cos(DegreeToRadian(-pCamera->m_cRotation.y)));
			vCurrent.x = vRotated.x;
			vCurrent.y = vRotated.y;*/

			mtxRotate._11 = cos(DegreeToRadian(pCamera->m_cRotation.y));
			mtxRotate._13 = -sin(DegreeToRadian(pCamera->m_cRotation.y));
			mtxRotate._31 = sin(DegreeToRadian(pCamera->m_cRotation.y));
			mtxRotate._33 = cos(DegreeToRadian(pCamera->m_cRotation.y));
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotate);
			D3DXMatrixIdentity(&mtxRotate);
		}
		if (pCamera->m_cRotation.z != 0.0f)
		{
			/*vRotated.x = float(vCurrent.x * cos(DegreeToRadian(-pCamera->m_cRotation.z)) - vCurrent.y * sin(DegreeToRadian(-pCamera->m_cRotation.z)));
			vRotated.y = float(vCurrent.x * sin(DegreeToRadian(-pCamera->m_cRotation.z)) + vCurrent.y * cos(DegreeToRadian(-pCamera->m_cRotation.z)));
			vCurrent.x = vRotated.x;
			vCurrent.y = vRotated.y;*/

			mtxRotate._11 = cos(DegreeToRadian(pCamera->m_cRotation.z));
			mtxRotate._12 = sin(DegreeToRadian(pCamera->m_cRotation.z));
			mtxRotate._21 = -sin(DegreeToRadian(pCamera->m_cRotation.z));
			mtxRotate._22 = cos(DegreeToRadian(pCamera->m_cRotation.z));
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotate);
			D3DXMatrixIdentity(&mtxRotate);

		}
		//Perspective Transformation
		/*float fAspect = (float)pCamera->m_Viewport.m_nWidth / (float)pCamera->m_Viewport.m_nHeight;
		if (vCurrent.z != 0.0f) vCurrent.x /= (fAspect * vCurrent.z);
		if (vCurrent.z != 0.0f) vCurrent.y /= vCurrent.z;*/
		D3DXMatrixPerspectiveLH(&mtxPerspective, (float)pCamera->m_Viewport.m_nWidth, (float)pCamera->m_Viewport.m_nHeight, 400, 1000);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxPerspective);
		D3DXMatrixIdentity(&mtxPerspective);
		//Screen Transformation
		/*vCurrent.x = +vCurrent.x * (pCamera->m_Viewport.m_nWidth * 0.5f) + pCamera->m_Viewport.m_xTopLeft + (pCamera->m_Viewport.m_nWidth * 0.5f);
		vCurrent.y = -vCurrent.y * (pCamera->m_Viewport.m_nHeight * 0.5f) + pCamera->m_Viewport.m_yTopLeft + (pCamera->m_Viewport.m_nHeight * 0.5f);*/

		mtxScreen._11 = pCamera->m_Viewport.m_nWidth / 2;
		mtxScreen._22 = -(pCamera->m_Viewport.m_nHeight / 2);
		mtxScreen._33 = pCamera->m_Viewport.m_vMaxDeep - pCamera->m_Viewport.m_vMinDeep;
		mtxScreen._41 = pCamera->m_Viewport.m_xTopLeft + pCamera->m_Viewport.m_nWidth / 2;
		mtxScreen._42 = pCamera->m_Viewport.m_yTopLeft + pCamera->m_Viewport.m_nHeight / 2;
		mtxScreen._43 = pCamera->m_Viewport.m_vMaxDeep;
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScreen);
		D3DXMatrixIdentity(&mtxScreen);

		D3DXVec3TransformCoord(&vCurrent, &vCurrent, &mtxWorld);
		if ((i != 0) && (vCurrent.z > 0.0f))
		{
			::MoveToEx(hDCFrameBuffer, (long)vPrevious.x, (long)vPrevious.y, NULL);
			::LineTo(hDCFrameBuffer, (long)vCurrent.x, (long)vCurrent.y);
		}
		vPrevious = vCurrent;
		mtxPrevious = mtxWorld;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMesh::CMesh(int nPolygons)
{
	if (nPolygons > 0)
	{
		m_nPolygons = nPolygons;
		m_ppPolygons = new CPolygon *[nPolygons];
		m_nReferences = 0;
	}
}

CMesh::~CMesh(void)
{
	if (m_ppPolygons)
	{
		for (int i = 0; i < m_nPolygons; i++) if (m_ppPolygons[i]) delete m_ppPolygons[i];
		delete[] m_ppPolygons;
	}
}

void CMesh::SetPolygon(int nIndex, CPolygon *pPolygon)
{
	if ((0 <= nIndex) && (nIndex < m_nPolygons) && m_ppPolygons)
	{
		m_ppPolygons[nIndex] = pPolygon;
	}
}

void CMesh::Render(HDC hDCFrameBuffer, CGameObject *pObject, CCamera *pCamera)
{
	for (int j = 0; j < m_nPolygons; j++)
	{
		m_ppPolygons[j]->Draw(hDCFrameBuffer, pObject, pCamera);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMesh::CCubeMesh() : CMesh(6)
{
	CPolygon *pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-0.5, +0.5, -0.5));
	pFrontFace->SetVertex(1, CVertex(+0.5, +0.5, -0.5));
	pFrontFace->SetVertex(2, CVertex(+0.5, -0.5, -0.5));
	pFrontFace->SetVertex(3, CVertex(-0.5, -0.5, -0.5));
	SetPolygon(0, pFrontFace);

	CPolygon *pTopFace = new CPolygon(4);
	pTopFace->SetVertex(0, CVertex(-0.5, +0.5, +0.5));
	pTopFace->SetVertex(1, CVertex(+0.5, +0.5, +0.5));
	pTopFace->SetVertex(2, CVertex(+0.5, +0.5, -0.5));
	pTopFace->SetVertex(3, CVertex(-0.5, +0.5, -0.5));
	SetPolygon(1, pTopFace);

	CPolygon *pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-0.5, -0.5, +0.5));
	pBackFace->SetVertex(1, CVertex(+0.5, -0.5, +0.5));
	pBackFace->SetVertex(2, CVertex(+0.5, +0.5, +0.5));
	pBackFace->SetVertex(3, CVertex(-0.5, +0.5, +0.5));
	SetPolygon(2, pBackFace);

	CPolygon *pBottomFace = new CPolygon(4);
	pBottomFace->SetVertex(0, CVertex(-0.5, -0.5, -0.5));
	pBottomFace->SetVertex(1, CVertex(+0.5, -0.5, -0.5));
	pBottomFace->SetVertex(2, CVertex(+0.5, -0.5, +0.5));
	pBottomFace->SetVertex(3, CVertex(-0.5, -0.5, +0.5));
	SetPolygon(3, pBottomFace);

	CPolygon *pLeftFace = new CPolygon(4);
	pLeftFace->SetVertex(0, CVertex(-0.5, +0.5, +0.5));
	pLeftFace->SetVertex(1, CVertex(-0.5, +0.5, -0.5));
	pLeftFace->SetVertex(2, CVertex(-0.5, -0.5, -0.5));
	pLeftFace->SetVertex(3, CVertex(-0.5, -0.5, +0.5));
	SetPolygon(4, pLeftFace);

	CPolygon *pRightFace = new CPolygon(4);
	pRightFace->SetVertex(0, CVertex(+0.5, +0.5, -0.5));
	pRightFace->SetVertex(1, CVertex(+0.5, +0.5, +0.5));
	pRightFace->SetVertex(2, CVertex(+0.5, -0.5, +0.5));
	pRightFace->SetVertex(3, CVertex(+0.5, -0.5, -0.5));
	SetPolygon(5, pRightFace);
}

BigCubeMesh::BigCubeMesh() : CMesh(6)
{
	CPolygon *pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-10, +10, -10));
	pFrontFace->SetVertex(1, CVertex(+10, +10, -10));
	pFrontFace->SetVertex(2, CVertex(+10, -10, -10));
	pFrontFace->SetVertex(3, CVertex(-10, -10, -10));
	SetPolygon(0, pFrontFace);

	CPolygon *pTopFace = new CPolygon(4);
	pTopFace->SetVertex(0, CVertex(-10, +10, +10));
	pTopFace->SetVertex(1, CVertex(+10, +10, +10));
	pTopFace->SetVertex(2, CVertex(+10, +10, -10));
	pTopFace->SetVertex(3, CVertex(-10, +10, -10));
	SetPolygon(1, pTopFace);

	CPolygon *pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-10, -10, +10));
	pBackFace->SetVertex(1, CVertex(+10, -10, +10));
	pBackFace->SetVertex(2, CVertex(+10, +10, +10));
	pBackFace->SetVertex(3, CVertex(-10, +10, +10));
	SetPolygon(2, pBackFace);

	CPolygon *pBottomFace = new CPolygon(4);
	pBottomFace->SetVertex(0, CVertex(-10, -10, -10));
	pBottomFace->SetVertex(1, CVertex(+10, -10, -10));
	pBottomFace->SetVertex(2, CVertex(+10, -10, +10));
	pBottomFace->SetVertex(3, CVertex(-10, -10, +10));
	SetPolygon(3, pBottomFace);

	CPolygon *pLeftFace = new CPolygon(4);
	pLeftFace->SetVertex(0, CVertex(-10, +10, +10));
	pLeftFace->SetVertex(1, CVertex(-10, +10, -10));
	pLeftFace->SetVertex(2, CVertex(-10, -10, -10));
	pLeftFace->SetVertex(3, CVertex(-10, -10, +10));
	SetPolygon(4, pLeftFace);

	CPolygon *pRightFace = new CPolygon(4);
	pRightFace->SetVertex(0, CVertex(+10, +10, -10));
	pRightFace->SetVertex(1, CVertex(+10, +10, +10));
	pRightFace->SetVertex(2, CVertex(+10, -10, +10));
	pRightFace->SetVertex(3, CVertex(+10, -10, -10));
	SetPolygon(5, pRightFace);
}

CCubeMesh::~CCubeMesh(void)
{
}

BigCubeMesh::~BigCubeMesh(void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject()
{
	m_pMesh = NULL;
	m_dwColor = 0x00000000;
	m_fScale.x = m_fScale.y = m_fScale.z = 1.0f;
	m_fRotation.x = m_fRotation.y = m_fRotation.z = 0.0f;
	m_fPosition.x = m_fPosition.y = m_fPosition.z = 0.0f;
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
}

void CGameObject::Render(HDC hDCFrameBuffer, CCamera *pCamera)
{
	HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
	HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

	if (m_pMesh) m_pMesh->Render(hDCFrameBuffer, this, pCamera);

	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCamera::CCamera()
{
	m_cRotation.x = m_cRotation.y = m_cRotation.z = 0.0f;
	m_cPosition.x = m_cPosition.y = 0.0f;
	m_cPosition.z = 30.0; // z포트 뒤로 옮긴다. (수정)
	m_Viewport.m_xTopLeft = 0;
	m_Viewport.m_yTopLeft = 0;
	m_Viewport.m_nWidth = 640;
	m_Viewport.m_nHeight = 480;
	m_Viewport.m_vMaxDeep = 1000;
	m_Viewport.m_vMinDeep = 100;
}

CCamera::~CCamera()
{
}

void CCamera::SetViewport(float xTopLeft, float yTopLeft, float nWidth, float nHeight,float zn,float zf)
{
	m_Viewport.m_xTopLeft = xTopLeft;
	m_Viewport.m_yTopLeft = yTopLeft;
	m_Viewport.m_nWidth = nWidth;
	m_Viewport.m_nHeight = nHeight;
	m_Viewport.m_vMinDeep = 10;
	m_Viewport.m_vMaxDeep = 1000;
}
