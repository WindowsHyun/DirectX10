//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	m_hInstance = NULL;
	m_hWnd = NULL;

	m_hDCFrameBuffer = NULL;
	m_hBitmapFrameBuffer = NULL;

	m_bActive = true;

	m_nObjects = 0;
	m_bObjects = 0;
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	srand(timeGetTime());

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

#ifdef _DEBUG
	AllocConsole();
	freopen("CONOUT$", "wt", stdout);
#endif

	RECT rc;
	GetClientRect(m_hWnd, &rc);

	m_pCamera = new CCamera();
	m_pCamera->SetViewport(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,0,0);

	BuildFrameBuffer();

	BuildObjects();

	SetupGameState();

	return(true);
}

void CGameFramework::BuildFrameBuffer()
{
	HDC hDC = ::GetDC(m_hWnd);

	m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, m_pCamera->m_Viewport.m_nWidth, m_pCamera->m_Viewport.m_nHeight);
	::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);

	::ReleaseDC(m_hWnd, hDC);
	::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
	HBRUSH hBrush = ::CreateSolidBrush(dwColor);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, m_pCamera->m_Viewport.m_xTopLeft, m_pCamera->m_Viewport.m_yTopLeft, m_pCamera->m_Viewport.m_nWidth, m_pCamera->m_Viewport.m_nHeight);
	::SelectObject(m_hDCFrameBuffer, hOldBrush);
	::DeleteObject(hBrush);
}

void CGameFramework::PresentFrameBuffer()
{
	HDC hDC = ::GetDC(m_hWnd);
	::BitBlt(hDC, m_pCamera->m_Viewport.m_xTopLeft, m_pCamera->m_Viewport.m_yTopLeft, m_pCamera->m_Viewport.m_nWidth, m_pCamera->m_Viewport.m_nHeight, m_hDCFrameBuffer, m_pCamera->m_Viewport.m_xTopLeft, m_pCamera->m_Viewport.m_yTopLeft, SRCCOPY);
	::ReleaseDC(m_hWnd, hDC);
}

void CGameFramework::SetupGameState() {
	// 처음 시작시에 한번만 작업을 한다.
	CCubeMesh *pCubeMesh = new CCubeMesh();
	BigCubeMesh *pCubeMesh2 = new BigCubeMesh();

	m_nObjects = 60;
	m_pObjects = new CGameObject[m_nObjects];

	m_bObjects = 1;
	m_bpObjects = new CGameObject[m_bObjects];


	default_random_engine dre;    // 기본 엔진 사용
	dre.seed((unsigned int)time(NULL));
	uniform_int_distribution<>color(0, 255);    // 색상에 대한 분포
	uniform_int_distribution<>p4(-m_nObjects, m_nObjects);    //+-
	uniform_real_distribution<float>mD(-0.0000001, 0.0100000);    // 움직이는 방향
	uniform_int_distribution<> ui;


	m_bpObjects[0].SetMesh(pCubeMesh2);
	m_bpObjects[0].SetColor(RGB(255, 0, 0));
	m_bpObjects[0].SetPosition(0, 0, 0);

	for (int i = 0; i < m_nObjects; ++i) {
		m_pObjects[i].SetMesh(pCubeMesh);
		m_pObjects[i].SetColor(RGB(color(dre), color(dre), color(dre)));
		m_pObjects[i].SetPosition(0, 0, 0);
		m_pObjects[i].SetMovingDirection(p4(dre), p4(dre), p4(dre));
		m_pObjects[i].m_fMovingSpeed = 0.001 + ui(dre) % 16 * 0.000005;
	}
}

void CGameFramework::SetupRenderStates() {

}

void CGameFramework::BuildObjects() {

}

void CGameFramework::ReleaseObjects()
{
	if (m_pObjects) delete[] m_pObjects;
	if (m_bpObjects) delete[] m_bpObjects;
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_hBitmapFrameBuffer) ::DeleteObject(m_hBitmapFrameBuffer);
	if (m_hDCFrameBuffer) ::DeleteDC(m_hDCFrameBuffer);

	if (m_hWnd) DestroyWindow(m_hWnd);
}

void CGameFramework::ProcessInput() {
	// 키보드 입력에 따라서 화면이 움직인다.
	if (GetKeyState(VK_LEFT) & 0xFF00) m_pCamera->m_cPosition.x += 25.0f * 0.0005f;
	if (GetKeyState(VK_RIGHT) & 0xFF00) m_pCamera->m_cPosition.x -= 25.0f * 0.0005f;
	if (GetKeyState(VK_UP) & 0xFF00) m_pCamera->m_cPosition.z -= 25.0f * 0.0005f;
	if (GetKeyState(VK_DOWN) & 0xFF00) m_pCamera->m_cPosition.z += 25.0f * 0.0005f;
}

void CGameFramework::AnimateObjects()
{
	float fYaw = 0.0f, fPitch = 0.0f, fRoll = 0.0f;

	for (int i = 0; i < m_nObjects; i++){
		fYaw = 0.02f * (i + 1);
		fPitch = 0.02f * (i + 1);
		fRoll = 0.01f * (i + 1);
		m_pObjects[i].Rotate(fPitch, fYaw, fRoll);
	}

	for (int i = 0; i < m_nObjects; i++) {
		// 움직여 보자.
		m_pObjects[i].CubeMove(m_pObjects[i].m_vMovingDirection.x,
			m_pObjects[i].m_vMovingDirection.y, m_pObjects[i].m_vMovingDirection.z);
	}
}

void CGameFramework::FrameAdvance()
{
	if (!m_bActive) return;

	ProcessInput();

	AnimateObjects();

	ClearFrameBuffer(RGB(0, 0, 0)); // 배경화면

	for (int i = 0; i < m_nObjects; i++) {
		m_pObjects[i].Render(m_hDCFrameBuffer, m_pCamera);
	}

	for (int i = 0; i < m_bObjects; i++) {
		m_bpObjects[i].Render(m_hDCFrameBuffer, m_pCamera);
	}
	//CubeMove(m_vPosition, m_vMovingDirection);


	PresentFrameBuffer();
}


