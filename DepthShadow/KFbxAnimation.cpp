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
		//영향을 미치는 행렬이 iNumCluster에 있다. 그러므로 iNumCluster 갯수대로 돌아서 값을 얻는다.
		for (int iCluster = 0; iCluster < iNumCluster; iCluster++)
		{
			FbxCluster* pCluster = pSkin->GetCluster(iCluster);
			//영향을 미치는 행렬이 iClusterSize 정점에 영향을 미침
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
			//ControlPoint(제어점) 정점리스트
			int* iIndex = pCluster->GetControlPointIndices();

			//가중치 리스트
			double* pWeight = pCluster->GetControlPointWeights();
			for (int i = 0; i < iNumVertex; i++)
			{
				pSkindata->m_VertexList[iIndex[i]].m_IndexList.push_back(iCluster);
				pSkindata->m_VertexList[iIndex[i]].m_WeightList.push_back(pWeight[i]);
				//iIndex[i] 정점은 iMatrixIndex 행렬이 pWeight[i] = 1 가중치로 영향을 미친다.
			}
		}
	}
	return true;
}
void KFbxObj::ParseAnimStack(FbxString* szData)
{
	// 1Frame = 160 Tick
	// 그래서 ASE 파일 보면 Animation 이 Tick 단위로 출력된 걸 확인할 수 있다.
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

	//MAX의 시간(tick)기준으로 우리가 사용하는 start, end time기준으로 변경해주는 작업.
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
	//애니메이션 데이터 저장

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


