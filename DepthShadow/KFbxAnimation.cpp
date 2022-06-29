#include "KFbxObj.h"
#include "KTimer.h"
#include <algorithm>

bool Compare(const pair<int, int>& a, const pair<int, int>& b)
{
	if (a.first == b.first)
		return a.second < b.second;
	return a.first < b.first;
}
KMesh* KFbxObj::GetFindIndex(FbxNode* pNode)
{
	for (int iNode = 0; iNode < m_pMeshList.size(); iNode++)
	{
		KMesh* pMesh = m_pMeshList[iNode];
		if (pMesh->m_pFbxNode == pNode)
		{
			return pMesh;
		}
	}
	return nullptr;
}
bool KFbxObj::ParseMeshSkinning(FbxMesh* pFbxMesh, KMesh* pMesh, KSkinData* pSkindata)
{
	int iNumDeformer = pFbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	if (iNumDeformer == 0)
	{
		return false;
	}
	int iNumVertexCount = pFbxMesh->GetControlPointsCount();
	pSkindata->m_VertexList.resize(iNumVertexCount);

	for (int iDeformer = 0; iDeformer < iNumDeformer; iDeformer++)
	{
		FbxDeformer* pFbxDeformer = pFbxMesh->GetDeformer(iDeformer, FbxDeformer::eSkin);
		FbxSkin* pSkin = (FbxSkin*)pFbxDeformer;
		int iNumCluster = pSkin->GetClusterCount();
		pMesh->m_matBindPoseList.resize(iNumCluster);
		//������ ��ġ�� ����� iNumCluster�� �ִ�. �׷��Ƿ� iNumCluster ������� ���Ƽ� ���� ��´�.
		for (int iCluster = 0; iCluster < iNumCluster; iCluster++)
		{
			FbxCluster* pCluster = pSkin->GetCluster(iCluster);
			//������ ��ġ�� ����� iClusterSize ������ ������ ��ħ
			int iNumVertex = pCluster->GetControlPointIndicesCount();

			FbxAMatrix matXBindPose, matInitPostion;
			pCluster->GetTransformLinkMatrix(matXBindPose);
			pCluster->GetTransformMatrix(matInitPostion);
			FbxAMatrix matBoneBindPos = matInitPostion.Inverse() * matXBindPose;
			KMatrix matBinePos = DxConvertMatrix(ConvertAMatrix(matBoneBindPos));
			D3DXMatrixInverse(&matBinePos, NULL, &matBinePos);
			pMesh->m_matBindPoseList[iCluster] = matBinePos;

			FbxNode* pLinkNode = pCluster->GetLink();
			pMesh->m_pFbxNodeList.push_back(pLinkNode);
			//ControlPoint(������) ��������Ʈ
			int* iIndex = pCluster->GetControlPointIndices();

			//����ġ ����Ʈ
			double* pWeight = pCluster->GetControlPointWeights();
			for (int i = 0; i < iNumVertex; i++)
			{
				pSkindata->m_VertexList[iIndex[i]].m_IndexList.push_back(iCluster);
				pSkindata->m_VertexList[iIndex[i]].m_WeightList.push_back(pWeight[i]);
				//iIndex[i] ������ iMatrixIndex ����� pWeight[i] = 1 ����ġ�� ������ ��ģ��.
			}
		}
	}
	return true;
}
void KFbxObj::ParseAnimStack(FbxString* szData)
{
	// 1Frame = 160 Tick
	// �׷��� ASE ���� ���� Animation �� Tick ������ ��µ� �� Ȯ���� �� �ִ�.
	// frameSpeed = 1 Sec per 30 Frame
	// 1Sec Tick = 30 * 160 = 4800 Tick
	// 0~100 Frame ( 16000 / 4800) = 3.3333 Sec

	m_pFbxScene->GetAnimationEvaluator()->Reset();

	FbxTakeInfo* pTakeInfo = m_pFbxScene->GetTakeInfo(*szData);
	FbxTime FrameTime;
	FrameTime.SetTime(0, 0, 0, 1, 0, m_pFbxScene->GetGlobalSettings().GetTimeMode());
	float fFrameTime = FrameTime.GetSecondDouble();
	float fFrameStep = 1;
	m_fSampleTime = fFrameTime * fFrameStep;

	//MAX�� �ð�(tick)�������� �츮�� ����ϴ� start, end time�������� �������ִ� �۾�.
	if (pTakeInfo)
	{
		m_fStartTime = (float)pTakeInfo->mLocalTimeSpan.GetStart().GetSecondDouble();
		m_fEndTime = (float)pTakeInfo->mLocalTimeSpan.GetStop().GetSecondDouble();
	}
	else
	{
		FbxTimeSpan tlTimeSpan;
		m_pFbxScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(tlTimeSpan);
		m_fStartTime = (float)tlTimeSpan.GetStart().GetSecondDouble();
		m_fEndTime = (float)tlTimeSpan.GetStop().GetSecondDouble();
	}
}
void KFbxObj::ParseAnimation()
{
	FbxArray<FbxString*> AnimStackNameArray;
	m_pFbxScene->FillAnimStackNameArray(AnimStackNameArray);
	int iAnimStackCount = AnimStackNameArray.GetCount();
	for (int iStack = 0; iStack < iAnimStackCount; iStack++)
	{
		ParseAnimStack(AnimStackNameArray.GetAt(iStack));
	}
}
void KFbxObj::ParseAnimationNode()
{
	//�ִϸ��̼� ������ ����

	FbxTime::SetGlobalTimeMode(FbxTime::eFrames30);
	FbxAnimStack* currAnimStack = m_pFbxScene->GetSrcObject<FbxAnimStack>(0);

	m_pFbxScene->SetCurrentAnimationStack(currAnimStack);
	FbxString Name = currAnimStack->GetNameOnly();
	FbxString TakeName = currAnimStack->GetName();
	FbxTakeInfo* TakeInfo = m_pFbxScene->GetTakeInfo(TakeName);
	FbxTimeSpan LocalTimeSpan = TakeInfo->mLocalTimeSpan;
	FbxTime Start = LocalTimeSpan.GetStart();
	FbxTime Stop = LocalTimeSpan.GetStop();
	FbxTime Duration = LocalTimeSpan.GetDuration();

	FbxTime::EMode TimeMode = FbxTime::GetGlobalTimeMode();
	FbxLongLong FrameCount = Duration.GetFrameCount(TimeMode);
	double FrameRate = FbxTime::GetFrameRate(TimeMode);

	FbxAnimEvaluator* pAnim = m_pFbxScene->GetAnimationEvaluator();
	FbxLongLong a = Start.GetFrameCount(TimeMode);
	FbxLongLong b = Stop.GetFrameCount(TimeMode);

	FbxTime time;
	for (FbxLongLong f = a; f <= b; ++f)
	{
		time.SetFrame(f, TimeMode);
		for (int iMesh = 0; iMesh < m_pMeshList.size(); iMesh++)
		{
			KMesh* pMesh = m_pMeshList[iMesh];
			FbxAMatrix matGlobal = pMesh->m_pFbxNode->EvaluateGlobalTransform(time);
			KMatrix matGlobaDX = DxConvertMatrix(ConvertAMatrix(matGlobal));
			pMesh->m_AnimationTrack.push_back(matGlobaDX);
		}
	}
}


