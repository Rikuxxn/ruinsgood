//=============================================================================
//
// ブロックマネージャー処理 [blockmanager.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _BLOCKMANAGER_H_
#define _BLOCKMANAGER_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "block.h"
#include "debugproc3D.h"

//*****************************************************************************
// ブロックマネージャークラス
//*****************************************************************************
class CBlockManager
{
public:
	CBlockManager();
	~CBlockManager();

    static CBlock* CreateBlock(CBlock::TYPE type, D3DXVECTOR3 pos);
    void Init(void);
    void Uninit(void);
    void UpdateInfo(void); // ImGuiでの操作関数をここで呼ぶ用
    void SaveToJson(const char* filename);
    void LoadFromJson(const char* filename);
    //static CBlock* FindNearbyHoldableBlock(float maxDistance);

     //*****************************************************************************
    // ImGuiでの操作関数
    //*****************************************************************************
    void UpdateTransform(CBlock* selectedBlock);
    void UpdateDraggingBlock(void);
    void UpdateCollider(CBlock* selectedBlock);

    //*****************************************************************************
    // getter関数
    //*****************************************************************************
    bool GetUpdateCollider(void) { return m_autoUpdateColliderSize; }
    static const char* GetFilePathFromType(CBlock::TYPE type);
    static const std::vector<CBlock*>& GetAllBlocks(void);

private:
    static std::vector<CBlock*> m_blocks;   // ブロック情報
    static CBlock* m_draggingBlock;         // ドラッグ中のブロック情報
    static int m_nNumAll;                   // 総数
    static int m_selectedIdx;               // 選択中のインデックス
    int m_prevSelectedIdx;                  // 前回の選択中のインデックス
    bool m_hasConsumedPayload ;             // ペイロード生成済みフラグ
    CDebugProc3D* m_pDebug3D;			    // 3Dデバッグ表示へのポインタ
    bool m_autoUpdateColliderSize;
};

#endif
