#pragma once
#include "Timer.h"
#include "Player.h"
#include "Scene.h"

class CGameFramework
{
private:
	//������ ���� �����ӿ�ũ���� ����� Ÿ�̸��̴�.
	CGameTimer m_GameTimer;//Ÿ�̸ӿ��ְ�? ��
	//������ ������ ���(Scene)�� �����ϴ� ��ü�� ���� �����͸� ��Ÿ����.
	CScene *m_pScene;
	CPlayer *m_pPlayer;
	CCamera *m_pCamera;
	//������ ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�.
	_TCHAR m_pszBuffer[50];
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	//����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D11Device *m_pd3dDevice;
	//���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	IDXGISwapChain *m_pDXGISwapChain;
	//����̽� ���ؽ�Ʈ�� ���� �������̴�. �ַ� ���������� ������ �ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D11DeviceContext *m_pd3dDeviceContext;
	//���� Ÿ�� �� �������̽��� ���� �������̴�. 
	ID3D11RenderTargetView *m_pd3dRenderTargetView;
	
	POINT	m_ptOldCursorPos;
public:
	CPlayerShader *m_pPlayerShader;
private:
	//���� ����(Depth Buffer) ��ü �������̽��� �信 ���� �������̴�.
	ID3D11Texture2D *m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView *m_pd3dDepthStencilView;

public:

	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	//����̽�, ���� ü��, ����̽� ���ؽ�Ʈ, ����̽��� ���õ� �並 �����ϴ� �Լ��̴�. 
	bool CreateRenderTargetDepthStencilView();
	bool CreateDirect3DDisplay();

	//�������� �޽�, ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�. 
	void BuildObjects();
	void ReleaseObjects();

	//�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�. 
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};
