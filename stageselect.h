//=============================================================================
//
// �X�e�[�W�Z���N�g���� [stageselecct.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _STAGESELECT_H_// ���̃}�N����`������Ă��Ȃ�������
#define _STAGESELECT_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "stage.h"

class CStageSelect
{
public:
    CStageSelect();
    ~CStageSelect();

    void Init(void);                     // ������
    void Uninit(void);
    void Update(void);                  // �X�V�i���͏����j
    void Draw(void);                    // �`��
    void SetVisible(bool isVisible);
    static bool IsVisible(void) { return m_isVisible; }
    static void SetSelectedStage(int id) { m_SelectedIndex = id; }
    static int GetSelectedStage(void) { return m_SelectedIndex; }
    void StartSlideIn(void);
    void StartSlideOut(void);
    bool IsSlideInFinished(void) const;
    bool IsSlideOutFinished(void) const;
    void SetReturn(bool flag) { m_isReturning = flag; }
    bool ReturnToTitle(void) { return m_isReturning; }
private:
    std::vector<CStage*> m_Stage;
    static int m_SelectedIndex;
    bool m_inputLock;
    static bool m_isVisible;
    int GetMouseOverIndex(void) const;
    bool m_isInputAllowed;
    bool m_isReturning;
};

#endif