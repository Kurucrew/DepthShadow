#pragma once
#include "KModel.h"
#include <fbxsdk.h>
#pragma comment	(lib, "libfbxsdk.lib")
#pragma comment	(lib, "libxml2-md.lib")
#pragma comment	(lib, "zlib-md.lib")

const enum OBJCTCLASSTYPE
{
	CLASS_GEOM = 0,
	CLASS_BONE,
	CLASS_DUMMY,
	CLASS_BIPED,
};
struct KMtrl
{
	FbxNode*				m_pFbxNode;
	FbxSurfaceMaterial*		m_pFbxSurfaceMtrl;
	KTexture				m_Texture;
	std::vector<KMtrl*>		m_pSubMtrl;
	KMtrl() {}
	KMtrl(FbxNode* pFbxNode, FbxSurfaceMaterial* pFbxMtrl)
	{
		m_pFbxNode = pFbxNode;
		m_pFbxSurfaceMtrl = pFbxMtrl;
	}
	void Release()
	{
		m_Texture.Release();
		for (auto data : m_pSubMtrl)
		{
			data->Release();
			delete data;
		}
	}
};
struct KLayer
{
	FbxLayerElementUV*			pUV;
	FbxLayerElementVertexColor* pColor;
	FbxLayerElementNormal*		pNormal;
	FbxLayerElementMaterial*	pMaterial;
};
struct KWeight
{
	std::vector<int>			m_IndexList;
	std::vector<float>			m_WeightList;
};
struct KSkinData
{
	std::vector<FbxNode*>		m_MatrixList;
	std::vector<KWeight>		m_VertexList;
};
struct PNCTIW_VERTEX
{
	float		index[4];
	float		weight[4];
};
struct KAnimMatrix
{
	KMatrix	matAnimation[255];
};

class KMesh : public KModel
{
public:
	std::vector<FbxNode*>		m_pFbxNodeList;
	std::vector<KMesh*>			m_pMeshLinkList;
	std::vector<KMatrix>		m_matBindPoseList;

	KAnimMatrix					m_matAnimMatrix;
	ID3D11Buffer*				m_pAnimCB = nullptr;
	ID3D11Buffer*				m_pIWVertexBuffer = nullptr;

	std::vector<PNCTIW_VERTEX>	m_WeightList;
	FbxNode*					m_pFbxNode = nullptr;
	OBJCTCLASSTYPE				m_ClassType;
	std::wstring				m_szName;
	std::wstring				m_szParentName;
	int							m_iNumLayer = 0;
	std::vector<KLayer>			m_LayerList;
	int							m_iMtrlRef;
	KMatrix						m_matWorld;
	KMesh*						m_pParent = nullptr;
	std::vector<KMatrix>		m_AnimationTrack;
	std::vector<KMesh*>			m_pSubMesh;

public:
	bool			Release() override;
	virtual HRESULT	CreateConstantBuffer() override;
	virtual HRESULT CreateVertexLayout() override;
	virtual HRESULT CreateVertexBuffer() override;
	virtual bool	PreRender(ID3D11DeviceContext* pContext) override;

	KMesh();
	virtual ~KMesh();
};
