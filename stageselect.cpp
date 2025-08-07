//=============================================================================
//
// ステージセレクト処理 [stageselect.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "stageselect.h"
#include "manager.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
bool CStageSelect::m_isVisible = false;
int CStageSelect::m_SelectedIndex = 0;

//=============================================================================
// コンストラクタ
//=============================================================================
CStageSelect::CStageSelect()
{
    m_inputLock = false;
    m_isInputAllowed = false;
    m_isReturning = false;
}
//=============================================================================
// デストラクタ
//=============================================================================
CStageSelect::~CStageSelect()
{
    // なし
}
//=============================================================================
// 初期化処理
//=============================================================================
void CStageSelect::Init(void)
{
    // 空にする
    m_Stage.clear();

    // スライドイン開始位置
    std::vector<D3DXVECTOR3> startPosX =
    {
        D3DXVECTOR3(2000.0f, 310.0f, 0.0f),
        D3DXVECTOR3(2200.0f, 460.0f, 0.0f),
        D3DXVECTOR3(2400.0f, 610.0f, 0.0f),
        D3DXVECTOR3(2600.0f, 760.0f, 0.0f)
    };

    // スライドイン目標位置
    std::vector<D3DXVECTOR3> targetPositions =
    {
        D3DXVECTOR3(1000.0f, 310.0f, 0.0f),
        D3DXVECTOR3(1000.0f, 460.0f, 0.0f),
        D3DXVECTOR3(1000.0f, 610.0f, 0.0f),
        D3DXVECTOR3(1000.0f, 760.0f, 0.0f)
    };

    for (int i = 0; i < (int)targetPositions.size(); i++)
    {
        // 開始位置は画面右外（startPosX）＋高さは目標位置と同じ
        D3DXVECTOR3 startPos(startPosX[i].x, targetPositions[i].y, targetPositions[i].z);

        CStage* stage = nullptr;
        switch (i)
        {
        case 0:
            stage = CStage::Create(CStage::STAGE_ID_1, startPos, 220.0f, 50.0f);
            break;
        case 1:
            stage = CStage::Create(CStage::STAGE_ID_2, startPos, 220.0f, 50.0f);
            break;
        case 2:
            stage = CStage::Create(CStage::STAGE_ID_3, startPos, 220.0f, 50.0f);
            break;
        case 3:
            stage = CStage::Create(CStage::STAGE_ID_NONE, startPos, 220.0f, 50.0f);
            break;
        }

        // 目標位置をセット
        stage->SetTargetPos(targetPositions[i]);

        stage->Init();
        stage->SetSelected(false);

        m_Stage.push_back(stage);
    }

    m_SelectedIndex = 0;
    m_Stage[m_SelectedIndex]->SetSelected(true);

    m_inputLock = false;
    m_isVisible = false;
}
//=============================================================================
// 終了処理
//=============================================================================
void CStageSelect::Uninit(void)
{
    // 空にする
    m_Stage.clear();
}
//=============================================================================
// 更新処理
//=============================================================================
void CStageSelect::Update(void)
{
    if (!IsVisible())
    {
        return;
    }

    int mouseOver = GetMouseOverIndex();

    // マウスオーバー時の選択変更＆SE
    if (mouseOver != -1 && mouseOver != m_SelectedIndex)
    {
        m_SelectedIndex = mouseOver;

        // 選択SE
        CManager::GetSound()->Play(CSound::SOUND_LABEL_SELECT);
    }

    // ゲームパッドでの上下移動
    bool up = CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_UP);
    bool down = CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_DOWN);

    if ((up || down) && !m_inputLock)
    {
        // 選択SE
        CManager::GetSound()->Play(CSound::SOUND_LABEL_SELECT);

        if (up)
        {
            m_SelectedIndex--;
        }
        else
        {
            m_SelectedIndex++;
        }

        if (m_SelectedIndex < 0)
        {
            m_SelectedIndex = (int)m_Stage.size() - 1;
        }
        if (m_SelectedIndex >= (int)m_Stage.size())
        {
            m_SelectedIndex = 0;
        }

        m_inputLock = true;
    }

    if (!up && !down)
    {
        m_inputLock = false;
    }

    // 全ステージ項目のスライドイン完了をチェック
    if (!m_isInputAllowed)
    {
        bool allDone = true;

        for (auto& stage : m_Stage)
        {
            if (!stage->IsSlideIn())
            {
                allDone = false;
                break;
            }
        }

        if (allDone)
        {
            m_isInputAllowed = true;
        }
    }

    // クリックまたはゲームパッドボタンで実行
    if (m_isInputAllowed && CManager::GetFade()->GetFade() == CFade::FADE_NONE)
    {
        bool confirm = false;

        // マウスクリック
        if (mouseOver != -1 && CManager::GetInputMouse()->GetTrigger(0))
        {
            confirm = true;
        }

        // ゲームパッド
        if (CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_A))
        {
            confirm = true;
        }

        if (confirm)
        {
            // 決定SE
            CManager::GetSound()->Play(CSound::SOUND_LABEL_ENTER);

            if (m_SelectedIndex == CStage::STAGE_ID_NONE)
            {
                SetReturn(true);
            }
            else
            {
                // 選ばれたステージIDを保存
                SetSelectedStage(m_Stage[m_SelectedIndex]->GetStageId());

                // 各項目の選択時処理
                m_Stage[m_SelectedIndex]->Execute();
            }
        }
    }

    // 選択状態更新
    for (size_t i = 0; i < m_Stage.size(); i++)
    {
        m_Stage[i]->SetSelected(i == m_SelectedIndex);

        // ポーズの更新処理
        m_Stage[i]->Update();
    }
}
//=============================================================================
// 描画処理
//=============================================================================
void CStageSelect::Draw(void)
{
    for (auto stage : m_Stage)
    {
        if (stage)
        {
            stage->Draw();
        }
    }
}
//=============================================================================
// 表示状態の設定処理
//=============================================================================
void CStageSelect::SetVisible(bool isVisible)
{
    m_isVisible = isVisible;

    if (isVisible)
    {
        StartSlideIn();
    }
}
//=============================================================================
// マウス選択処理
//=============================================================================
int CStageSelect::GetMouseOverIndex(void) const
{
    for (size_t i = 0; i < m_Stage.size(); i++)
    {
        if (m_Stage[i]->IsMouseOver())
        {
            return (int)i;
        }
    }

    return -1;
}
//=============================================================================
// スライドイン処理
//=============================================================================
void CStageSelect::StartSlideIn(void)
{
    for (auto& stage : m_Stage)
    {
        stage->StartSlideIn(true);
    }
}
//=============================================================================
// スライドアウト処理
//=============================================================================
void CStageSelect::StartSlideOut(void)
{
    for (auto& stage : m_Stage)
    {
        stage->StartSlideOut(true); // 各項目が移動開始
    }
}
//=============================================================================
// スライドアウト終了判定処理
//=============================================================================
bool CStageSelect::IsSlideOutFinished(void) const
{
    for (const auto& stage : m_Stage)
    {
        if (!stage->IsSlideOutFinished())
        {
            return false;
        }
    }

    return true;
}


