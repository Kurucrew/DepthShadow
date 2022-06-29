#pragma once
#include "KMesh.h"

class KFbxObj : public KModel
{
	FbxManager* m_pFbxManager;
	FbxImporter* m_pFbxImporter;
	FbxScene* m_pFbxScene;
public:
	KMatrix					m_matWorld;
	KAnimMatrix				m_matAnimMatrix;
	bool					m_bAnimPlay = false;
	float					m_fElpaseTime = 0.0f;
	int						m_iAnimIndex = 0;
	float					m_fStartTime;
	float					m_fEndTime;
	float					m_fSampleTime;
	std::vector<FbxNode*>	m_pFbxNodeList;
	std::vector<KMtrl*>		m_pFbxMaterialList;
	std::vector<KMesh*>		m_pMeshList;
	CB_DATA					m_cbData;

public:
	KMesh*		GetFindIndex(FbxNode* pNode);
	bool		LoadObject(std::string filename, std::string shaderName);

	bool		Frame();
	bool		Release();
	KMatrix		DxConvertMatrix(KMatrix m);
	KMatrix		ConvertMatrix(FbxMatrix& m);
	KMatrix		ConvertAMatrix(FbxAMatrix& m);

	void	SetMatrix(KMatrix* pMatWorld, KMatrix* pMatView, KMatrix* pMatProj);
	bool	Render(ID3D11DeviceContext* pContext);
	int		GetRootMtrl(FbxSurfaceMaterial* pFbxMaterial);
	void	LoadMaterial(KMtrl* pMtrl);

	void	PreProcess(FbxNode* pNode);
	void	ParseNode(FbxNode* pNode, KMesh* pMesh);
	void	ParseMesh(FbxNode* pNode, KMesh* pMesh);
	KMatrix ParseTransform(FbxNode* pNode, KMatrix& matParent);
public:
	void	ParseAnimationNode();
	void	ParseAnimation();
	void	ParseAnimStack(FbxString* szData);
	bool	ParseMeshSkinning(FbxMesh* pFbxMesh, KMesh* pMesh, KSkinData* pSkindata);

	FbxVector2	ReadTextureCoord(FbxMesh* pFbxMesh, DWORD dwVertexTextureCount, FbxLayerElementUV* pUVSet, int vertexIndex, int uvIndex);
	FbxVector4	ReadNormal(const FbxMesh* mesh, DWORD dwVertexNormalCount, FbxLayerElementNormal* VertexNormalSets, int controlPointIndex, int dwVertexIndex);
	FbxColor	ReadColor(const FbxMesh* mesh, DWORD dwVertexColorCount, FbxLayerElementVertexColor* VertexColorSets, DWORD dwDDCIndex, DWORD dwVertexIndex);
	FbxVector4	ReadTangent(const FbxMesh* pFbxMesh, DWORD dwVertexTangentCount, FbxGeometryElementTangent* VertexTangentSets, DWORD dwDCCIndex, DWORD dwVertexIndex);

	void		SetPixelShader(ID3D11PixelShader* ps = nullptr);
};

