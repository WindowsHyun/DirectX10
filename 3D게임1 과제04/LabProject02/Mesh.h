#pragma once
#define RANDOM_COLOR D3DXCOLOR((rand() * 0xFFFFFF) / RAND_MAX)
struct MESHINTERSECTINFO {
	DWORD m_dwFaceIndex;
	float m_fU;
	float m_fV;
	float m_fDistance;
};
class CVertex
{
	//������ ��ġ ����(3���� ����)�� �����ϱ� ���� ��� ������ �����Ѵ�. 
	D3DXVECTOR3 m_d3dxvPosition;
public:
	//CVertex Ŭ������ �����ڿ� �Ҹ��ڸ� ������ ���� �����Ѵ�. 
	CVertex() { m_d3dxvPosition = D3DXVECTOR3(0, 0, 0); }
	CVertex(D3DXVECTOR3 d3dxvPosition) { m_d3dxvPosition = d3dxvPosition; }
	~CVertex() { }
};
class AABB
{
public:
	//�ٿ�� �ڽ��� �ּ����� �ִ����� ��Ÿ���� �����̴�.
	D3DXVECTOR3 m_d3dxvMinimum;
	D3DXVECTOR3 m_d3dxvMaximum;

public:
	AABB() { m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX); m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX); }
	AABB(D3DXVECTOR3 d3dxvMinimum, D3DXVECTOR3 d3dxvMaximum) { m_d3dxvMinimum = d3dxvMinimum; m_d3dxvMaximum = d3dxvMaximum; }

	//�� ���� �ٿ�� �ڽ��� ���Ѵ�.
	void Merge(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	void Merge(AABB *pAABB);
	//�ٿ�� �ڽ��� 8���� �������� ��ķ� ��ȯ�ϰ� �ּ����� �ִ����� �ٽ� ����Ѵ�.
	void Update(D3DXMATRIX *pd3dxmtxTransform);
};
class CMesh
{
public:
	//CMesh Ŭ������ �����ڿ� �Ҹ��ڸ� �����Ѵ�. 
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();

	//CMesh Ŭ���� ��ü�� ����(Reference)�� ���õ� ��� ������ �Լ��� �����Ѵ�.
private:
	int m_nReferences;
protected:
	AABB m_bcBoundingCube;
	
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	AABB GetBoundingCube() { return(m_bcBoundingCube); }
	/*�� ������ ��ġ ���͸� ��ŷ�� ���Ͽ� �����Ѵ�(���� ���۸� DYNAMIC���� �����ϰ� Map()�� ���� �ʾƵ� �ǵ���).*/
	D3DXVECTOR3 *m_pd3dxvPositions;
	/*�޽��� �ε����� �����Ѵ�(�ε��� ���۸� DYNAMIC���� �����ϰ� Map()�� ���� �ʾƵ� �ǵ���).*/
	UINT *m_pnIndices;
protected:
	//������ ��ġ ���Ϳ� ������ �����ϱ� ���� ���ۿ� ���� �������̽� �������̴�. 
	ID3D11Buffer *m_pd3dPositionBuffer;
	ID3D11Buffer *m_pd3dColorBuffer;

	//���۵��� �Է������⿡ �����ϱ� ���� ���� ���� ��ȣ�̴�. 
	UINT m_nSlot;
	/*�ν��Ͻ��� ���� ���� ���۴� �޽��� ���� �����Ϳ� �ν��Ͻ� ������(��ü�� ��ġ�� ����)�� ���´�. �׷��Ƿ� �ν��Ͻ��� ���� ���� ���۴� �ϳ��� �ƴ϶� ���۵��� �迭�̴�. ������ ��ҵ��� ��Ÿ���� ���۵��� �Է������⿡ �����ϱ� ���� �����̴�.*/
	ID3D11Buffer **m_ppd3dVertexBuffers;
	//������ �����ϱ� ���� �ʿ��� ������ �����̴�. 
	int m_nBuffers;

	//������ �����̴�. 
	
	UINT m_nStartVertex;
	//������ ��ҵ��� ��Ÿ���� ���۵��� ������ ����Ʈ ���� ��Ÿ���� �迭�̴�. 
	UINT *m_pnVertexStrides;
	//������ ��ҵ��� ��Ÿ���� ���۵��� ���� ��ġ(����Ʈ ��)�� ��Ÿ���� �迭�̴�. 
	UINT *m_pnVertexOffsets;
	//���� �����͸� �����ϱ� ���� ���� ���� �������̽� �����͸� �����Ѵ�.
	ID3D11Buffer *m_pd3dVertexBuffer;
	ID3D11RasterizerState *m_pd3dRasterizerState;
	/*���� ������ ���� ����, ������ ����Ʈ ��, ���� �����Ͱ� ���� ������ ��𿡼����� �����ϴ� ���� ��Ÿ���� ������ �����Ѵ�.*/
	UINT m_nVertices;
	UINT m_nStride;
	UINT m_nOffset;
	/*�ε��� ����(�ε����� �迭)�� ���� �������̽� �������̴�. �ε��� ���۴� ���� ����(�迭)�� ���� �ε����� ������.*/
	ID3D11Buffer *m_pd3dIndexBuffer;
	//�ε��� ���۰� �����ϴ� �ε����� �����̴�. 
	UINT m_nIndices;
	//�ε��� ���ۿ��� �޽��� ǥ���ϱ� ���� ���Ǵ� ���� �ε����̴�. 
	UINT m_nStartIndex;
	//�� �ε����� ������ �ε����̴�. 
	int m_nBaseVertex;
	UINT m_nIndexOffset;
	//�� �ε����� ����(DXGI_FORMAT_R32_UINT �Ǵ� DXGI_FORMAT_R16_UINT)�̴�. 
	DXGI_FORMAT	m_dxgiIndexFormat;

	//���� �����Ͱ� � ������Ƽ�긦 ǥ���ϰ� �ִ� ���� ��Ÿ���� ��� ������ �����Ѵ�.
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	//���� �����͸� �������ϴ� ��� �Լ��� �����Ѵ�.
public:
	void AssembleToVertexBuffer(int nBuffers = 0, ID3D11Buffer **m_pd3dBuffers = NULL, UINT *pnBufferStrides = NULL, UINT *pnBufferOffsets = NULL);

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice) { }
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	//�ν��Ͻ��� ����Ͽ� �������Ѵ�. 
	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances = 0, int nStartInstance = 0);
	int CheckRayIntersection(D3DXVECTOR3 *pd3dxvRayPosition, D3DXVECTOR3 *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo);
};
class CTriangleMesh : public CMesh
{
public:	
	CTriangleMesh(ID3D11Device *pd3dDevice);
	virtual ~CTriangleMesh();

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
};

class CDiffusedVertex
{
	D3DXVECTOR3 m_d3dxvPosition;
	//������ ������ ��Ÿ���� ��� ����(D3DXCOLOR ����ü)�� �����Ѵ�. 
	D3DXCOLOR m_d3dxcDiffuse;
public:
	//�����ڿ� �Ҹ��ڸ� �����Ѵ�.
	CDiffusedVertex(float x, float y, float z, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex(D3DXVECTOR3 d3dxvPosition, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = d3dxvPosition; m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f); }
	~CDiffusedVertex() { }
};

class CCubeMesh : public CMesh
{
public:
	//������ü�� ����, ����, ������ ũ�⸦ �����Ͽ� ������ü �޽��� �����Ѵ�.
	CCubeMesh(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CCubeMesh();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CBCubeMesh : public CMesh
{
public:
	//������ü�� ����, ����, ������ ũ�⸦ �����Ͽ� ������ü �޽��� �����Ѵ�.
	CBCubeMesh( ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, D3DXCOLOR d3dxColor = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f ) );
	virtual ~CBCubeMesh();

	virtual void CreateRasterizerState( ID3D11Device *pd3dDevice );
	virtual void Render( ID3D11DeviceContext *pd3dDeviceContext );
};

class CMCubeMesh : public CMesh
{
public:
	//������ü�� ����, ����, ������ ũ�⸦ �����Ͽ� ������ü �޽��� �����Ѵ�.
	CMCubeMesh( ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, D3DXCOLOR d3dxColor = D3DXCOLOR( 0.0f, 1.0f, 1.0f, 1.0f ) );
	virtual ~CMCubeMesh();

	virtual void CreateRasterizerState( ID3D11Device *pd3dDevice );
	virtual void Render( ID3D11DeviceContext *pd3dDeviceContext );
};

class CMazeCubeMesh : public CMesh
{
public:
	//������ü�� ����, ����, ������ ũ�⸦ �����Ͽ� ������ü �޽��� �����Ѵ�.
	CMazeCubeMesh( ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, D3DXCOLOR d3dxColor = D3DXCOLOR( 0.0f, 1.0f, 1.0f, 1.0f ) );
	virtual ~CMazeCubeMesh();

	virtual void CreateRasterizerState( ID3D11Device *pd3dDevice );
	virtual void Render( ID3D11DeviceContext *pd3dDeviceContext );
};

class CCellCubeMesh : public CMesh
{
public:
	//������ü�� ����, ����, ������ ũ�⸦ �����Ͽ� ������ü �޽��� �����Ѵ�.
	CCellCubeMesh( ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, D3DXCOLOR d3dxColor = D3DXCOLOR( 0.0f, 1.0f, 1.0f, 1.0f ) );
	virtual ~CCellCubeMesh();

	virtual void CreateRasterizerState( ID3D11Device *pd3dDevice );
	virtual void Render( ID3D11DeviceContext *pd3dDeviceContext );
};

class CAirplaneMesh : public CMesh
{
public:
	CAirplaneMesh(ID3D11Device *pd3dDevice, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f, D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirplaneMesh();
};

class CSphereMesh : public CMesh
{
public:
	CSphereMesh(ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20, D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CSphereMesh();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
};


class CMeshIlluminated : public CMesh
{
public:
	CMeshIlluminated( ID3D11Device *pd3dDevice );
	virtual ~CMeshIlluminated();

protected:
	//������ ������ ����ϱ� ���Ͽ� �������Ͱ� �ʿ��ϴ�.
	ID3D11Buffer *m_pd3dNormalBuffer;

public:
	//������ ���Ե� �ﰢ���� �������͸� ����ϴ� �Լ��̴�.
	D3DXVECTOR3 CalculateTriAngleNormal( UINT nIndex0, UINT nIndex1, UINT nIndex2 );
	void SetTriAngleListVertexNormal( D3DXVECTOR3 *pd3dxvNormals );
	//������ ���������� ����� ����ϴ� �Լ��̴�.
	void SetAverageVertexNormal( D3DXVECTOR3 *pd3dxvNormals, int nPrimitives, int nOffset, bool bStrip );
	void CalculateVertexNormal( D3DXVECTOR3 *pd3dxvNormals );
};

//������ ����Ͽ� ������ ������ �����ϱ� ���Ͽ� ������ �������͸� ���� ������ü �޽� Ŭ�����̴�.
class CCubeMeshIlluminated : public CMeshIlluminated
{
public:
	CCubeMeshIlluminated( ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f );
	virtual ~CCubeMeshIlluminated();
};

//������ ����Ͽ� ������ ������ �����ϱ� ���Ͽ� ������ �������͸� ���� �� �޽� Ŭ�����̴�.
class CSphereMeshIlluminated : public CMeshIlluminated
{
public:
	CSphereMeshIlluminated( ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20 );
	virtual ~CSphereMeshIlluminated();
};
