//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "pausemanager.h"
#include "manager.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CPauseManager::CPauseManager()
{
    m_SelectedIndex = 0;
    m_inputLock = false;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CPauseManager::~CPauseManager()
{
    // �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
void CPauseManager::Init(void)
{
    // ��ɂ���
    m_Items.clear();

    // �|�[�Y�̐���
    m_Items.push_back(CPause::Create(CPause::MENU_CONTINUE, D3DXVECTOR3(860.0f, 310.0f, 0.0f), 200.0f, 60.0f));
    m_Items.push_back(CPause::Create(CPause::MENU_RETRY, D3DXVECTOR3(860.0f, 510.0f, 0.0f), 200.0f, 60.0f));
    m_Items.push_back(CPause::Create(CPause::MENU_QUIT, D3DXVECTOR3(860.0f, 710.0f, 0.0f), 200.0f, 60.0f));

    for (auto item : m_Items)
    {
        // �|�[�Y�̏���������
        item->Init();

        item->SetSelected(false);
    }

    m_SelectedIndex = 0;
    m_Items[m_SelectedIndex]->SetSelected(true);

    m_inputLock = false;
}
//=============================================================================
// �I������
//=============================================================================
void CPauseManager::Uninit(void)
{
    // ��ɂ���
    m_Items.clear();
}
//=============================================================================
// �X�V����
//=============================================================================
void CPauseManager::Update(void)
{
    if (!CManager::GetisPaused())
    {
        return;
    }

    int mouseOver = GetMouseOverIndex();

    // �}�E�X�I�[�o�[���̑I��ύX��SE
    if (mouseOver != -1 && mouseOver != m_SelectedIndex)
    {
        m_SelectedIndex = mouseOver;

        // �I��SE
        CManager::GetSound()->Play(CSound::SOUND_LABEL_SELECT);
    }

    // �L�[�{�[�h/�Q�[���p�b�h�ł̏㉺�ړ�
    bool up = CManager::GetInputKeyboard()->GetTrigger(DIK_UP) || CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_UP);
    bool down = CManager::GetInputKeyboard()->GetTrigger(DIK_DOWN) || CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_DOWN);

    if ((up || down) && !m_inputLock)
    {
        // �I��SE
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

    // Enter�܂��̓Q�[���p�b�h�{�^���Ŏ��s
    if (CManager::GetFade()->GetFade() == CFade::FADE_NONE)
    {
        bool confirm = false;

        // �}�E�X�N���b�N
        if (mouseOver != -1 && CManager::GetInputMouse()->GetTrigger(0))
        {
            confirm = true;
        }

        // �L�[�{�[�h or �Q�[���p�b�h
        if (CManager::GetInputKeyboard()->GetTrigger(DIK_RETURN) || CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_A))
        {
            confirm = true;
        }

        if (confirm)
        {
            // ����SE
            CManager::GetSound()->Play(CSound::SOUND_LABEL_ENTER);

            // �e���ڂ̑I��������
            m_Items[m_SelectedIndex]->Execute();
        }
    }

    // �I����ԍX�V
    for (size_t i = 0; i < m_Items.size(); i++)
    {
        m_Items[i]->SetSelected(i == m_SelectedIndex);

        // �|�[�Y�̍X�V����
        m_Items[i]->Update();
    }
}
//=============================================================================
// �`�揈��
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
// �}�E�X�I������
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


