//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "pausemanager.h"
#include "manager.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CPauseManager::CPauseManager()
{
    m_SelectedIndex = 0;
    m_inputLock = false;
}
//=============================================================================
// デストラクタ
//=============================================================================
CPauseManager::~CPauseManager()
{
    // なし
}
//=============================================================================
// 初期化処理
//=============================================================================
void CPauseManager::Init(void)
{
    // 空にする
    m_Items.clear();

    // ポーズの生成
    m_Items.push_back(CPause::Create(CPause::MENU_CONTINUE, D3DXVECTOR3(860.0f, 310.0f, 0.0f), 200.0f, 60.0f));
    m_Items.push_back(CPause::Create(CPause::MENU_RETRY, D3DXVECTOR3(860.0f, 510.0f, 0.0f), 200.0f, 60.0f));
    m_Items.push_back(CPause::Create(CPause::MENU_QUIT, D3DXVECTOR3(860.0f, 710.0f, 0.0f), 200.0f, 60.0f));

    for (auto item : m_Items)
    {
        // ポーズの初期化処理
        item->Init();

        item->SetSelected(false);
    }

    m_SelectedIndex = 0;
    m_Items[m_SelectedIndex]->SetSelected(true);

    m_inputLock = false;
}
//=============================================================================
// 終了処理
//=============================================================================
void CPauseManager::Uninit(void)
{
    // 空にする
    m_Items.clear();
}
//=============================================================================
// 更新処理
//=============================================================================
void CPauseManager::Update(void)
{
    if (!CManager::GetisPaused())
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

    // キーボード/ゲームパッドでの上下移動
    bool up = CManager::GetInputKeyboard()->GetTrigger(DIK_UP) || CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_UP);
    bool down = CManager::GetInputKeyboard()->GetTrigger(DIK_DOWN) || CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_DOWN);

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
            m_SelectedIndex = (int)m_Items.size() - 1;
        }
        if (m_SelectedIndex >= (int)m_Items.size())
        {
            m_SelectedIndex = 0;
        }

        m_inputLock = true;
    }

    if (!up && !down)
    {
        m_inputLock = false;
    }

    // Enterまたはゲームパッドボタンで実行
    if (CManager::GetFade()->GetFade() == CFade::FADE_NONE)
    {
        bool confirm = false;

        // マウスクリック
        if (mouseOver != -1 && CManager::GetInputMouse()->GetTrigger(0))
        {
            confirm = true;
        }

        // キーボード or ゲームパッド
        if (CManager::GetInputKeyboard()->GetTrigger(DIK_RETURN) || CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_A))
        {
            confirm = true;
        }

        if (confirm)
        {
            // 決定SE
            CManager::GetSound()->Play(CSound::SOUND_LABEL_ENTER);

            // 各項目の選択時処理
            m_Items[m_SelectedIndex]->Execute();
        }
    }

    // 選択状態更新
    for (size_t i = 0; i < m_Items.size(); i++)
    {
        m_Items[i]->SetSelected(i == m_SelectedIndex);

        // ポーズの更新処理
        m_Items[i]->Update();
    }
}
//=============================================================================
// 描画処理
//=============================================================================
void CPauseManager::Draw(void)
{
    for (auto item : m_Items)
    {
        if (item)
        {
            item->Draw();
        }
    }
}
//=============================================================================
// マウス選択処理
//=============================================================================
int CPauseManager::GetMouseOverIndex(void) const
{
    for (size_t i = 0; i < m_Items.size(); i++)
    {
        if (m_Items[i]->IsMouseOver())
        {
            return (int)i;
        }
    }

    return -1;
}


