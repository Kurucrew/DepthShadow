#pragma once
#include "KModel.h"
struct KMapInfo
{
    int             m_iNumRow;
    int             m_iNumCol;
    int             m_iNumRowCell;
    int             m_iNumColCell;
    int             m_iNumVertex;
    float           m_fCellDistance;
    std::wstring    szDefaultTexture;
};
class KMap :public KModel
{
public:
    KMapInfo m_info;

    bool    Load(KMapInfo& info, std::wstring vs, std::wstring ps);
    bool	CreateVertexData() override;
    bool	CreateIndexData() override;
};

