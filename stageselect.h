//=============================================================================
//
// ステージセレクト処理 [stageselecct.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _STAGESELECT_H_// このマクロ定義がされていなかったら
#define _STAGESELECT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "stage.h"

class CStageSelect
{
public:
    CStageSelect();
    ~CStageSelect();

    void Init(void);                     // 初期化
    void Uninit(void);
    void Update(void);                  // 更新（入力処理）
    void Draw(void);                    // 描画
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