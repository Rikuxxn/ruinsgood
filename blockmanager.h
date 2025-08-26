//=============================================================================
//
// ブロックマネージャー処理 [blockmanager.h]
// Author : RIKU TANEKAWA
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
    void Update(void);
    void Draw(void);
    void UpdateInfo(void); // ImGuiでの操作関数をここで呼ぶ用
    void SaveToJson(const char* filename);
    void LoadFromJson(const char* filename);
    static bool CheckAllTorches(void);

     //*****************************************************************************
    // ImGuiでの操作関数
    //*****************************************************************************
    void UpdateTransform(CBlock* selectedBlock);
    void UpdateDraggingBlock(void);
    void PickBlockFromMouseClick(void);
    void UpdateCollider(CBlock* selectedBlock);

    //*****************************************************************************
    // getter関数
    //*****************************************************************************
    bool GetUpdateCollider(void) { return m_autoUpdateColliderSize; }
    static const char* GetFilePathFromType(CBlock::TYPE type);
    static const std::vector<CBlock*>& GetAllBlocks(void);
    static CBlock* GetSelectedBlock(void) { return m_selectedBlock; }

private:
    static std::vector<CBlock*> m_blocks;   // ブロック情報
    static CBlock* m_draggingBlock;         // ドラッグ中のブロック情報
    static int m_nNumAll;                   // 総数
    static int m_selectedIdx;               // 選択中のインデックス
    int m_prevSelectedIdx;                  // 前回の選択中のインデックス
    bool m_hasConsumedPayload ;             // ペイロード生成済みフラグ
    CDebugProc3D* m_pDebug3D;			    // 3Dデバッグ表示へのポインタ
    bool m_autoUpdateColliderSize;
    static const std::unordered_map<CBlock::TYPE, const char*> s_FilePathMap;
    static CBlock* m_selectedBlock;                  // 選択中のブロック

};

#endif
