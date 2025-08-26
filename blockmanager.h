//=============================================================================
//
// �u���b�N�}�l�[�W���[���� [blockmanager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCKMANAGER_H_
#define _BLOCKMANAGER_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "block.h"
#include "debugproc3D.h"

//*****************************************************************************
// �u���b�N�}�l�[�W���[�N���X
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
    void UpdateInfo(void); // ImGui�ł̑���֐��������ŌĂԗp
    void SaveToJson(const char* filename);
    void LoadFromJson(const char* filename);
    static bool CheckAllTorches(void);

     //*****************************************************************************
    // ImGui�ł̑���֐�
    //*****************************************************************************
    void UpdateTransform(CBlock* selectedBlock);
    void UpdateDraggingBlock(void);
    void PickBlockFromMouseClick(void);
    void UpdateCollider(CBlock* selectedBlock);

    //*****************************************************************************
    // getter�֐�
    //*****************************************************************************
    bool GetUpdateCollider(void) { return m_autoUpdateColliderSize; }
    static const char* GetFilePathFromType(CBlock::TYPE type);
    static const std::vector<CBlock*>& GetAllBlocks(void);
    static CBlock* GetSelectedBlock(void) { return m_selectedBlock; }

private:
    static std::vector<CBlock*> m_blocks;   // �u���b�N���
    static CBlock* m_draggingBlock;         // �h���b�O���̃u���b�N���
    static int m_nNumAll;                   // ����
    static int m_selectedIdx;               // �I�𒆂̃C���f�b�N�X
    int m_prevSelectedIdx;                  // �O��̑I�𒆂̃C���f�b�N�X
    bool m_hasConsumedPayload ;             // �y�C���[�h�����ς݃t���O
    CDebugProc3D* m_pDebug3D;			    // 3D�f�o�b�O�\���ւ̃|�C���^
    bool m_autoUpdateColliderSize;
    static const std::unordered_map<CBlock::TYPE, const char*> s_FilePathMap;
    static CBlock* m_selectedBlock;                  // �I�𒆂̃u���b�N

};

#endif
