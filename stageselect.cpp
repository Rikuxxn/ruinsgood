//=============================================================================
//
// �X�e�[�W�Z���N�g���� [stageselect.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "stageselect.h"
#include "manager.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
bool CStageSelect::m_isVisible = false;
int CStageSelect::m_SelectedIndex = 0;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CStageSelect::CStageSelect()
{
    m_inputLock = false;
    m_isInputAllowed = false;
    m_isReturning = false;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CStageSelect::~CStageSelect()
{
    // �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
void CStageSelect::Init(void)
{
    // ��ɂ���
    m_Stage.clear();

    // �X���C�h�C���J�n�ʒu
    std::vector<D3DXVECTOR3> startPosX =
    {
        D3DXVECTOR3(2000.0f, 310.0f, 0.0f),
        D3DXVECTOR3(2200.0f, 460.0f, 0.0f),
        D3DXVECTOR3(2400.0f, 610.0f, 0.0f),
        D3DXVECTOR3(2600.0f, 760.0f, 0.0f)
    };

    // �X���C�h�C���ڕW�ʒu
    std::vector<D3DXVECTOR3> targetPositions =
    {
        D3DXVECTOR3(1000.0f, 310.0f, 0.0f),
        D3DXVECTOR3(1000.0f, 460.0f, 0.0f),
        D3DXVECTOR3(1000.0f, 610.0f, 0.0f),
        D3DXVECTOR3(1000.0f, 760.0f, 0.0f)
    };

    for (int i = 0; i < (int)targetPositions.size(); i++)
    {
        // �J�n�ʒu�͉�ʉE�O�istartPosX�j�{�����͖ڕW�ʒu�Ɠ���
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

        // �ڕW�ʒu���Z�b�g
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
// �I������
//=============================================================================
void CStageSelect::Uninit(void)
{
    // ��ɂ���
    m_Stage.clear();
}
//=============================================================================
// �X�V����
//=============================================================================
void CStageSelect::Update(void)
{
    if (!IsVisible())
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

    // �Q�[���p�b�h�ł̏㉺�ړ�
    bool up = CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_UP);
    bool down = CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_DOWN);

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

    // �S�X�e�[�W���ڂ̃X���C�h�C���������`�F�b�N
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

    // �N���b�N�܂��̓Q�[���p�b�h�{�^���Ŏ��s
    if (m_isInputAllowed && CManager::GetFade()->GetFade() == CFade::FADE_NONE)
    {
        bool confirm = false;

        // �}�E�X�N���b�N
        if (mouseOver != -1 && CManager::GetInputMouse()->GetTrigger(0))
        {
            confirm = true;
        }

        // �Q�[���p�b�h
        if (CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_A))
        {
            confirm = true;
        }

        if (confirm)
        {
            // ����SE
            CManager::GetSound()->Play(CSound::SOUND_LABEL_ENTER);

            if (m_SelectedIndex == CStage::STAGE_ID_NONE)
            {
                SetReturn(true);
            }
            else
            {
                // �I�΂ꂽ�X�e�[�WID��ۑ�
                SetSelectedStage(m_Stage[m_SelectedIndex]->GetStageId());

                // �e���ڂ̑I��������
                m_Stage[m_SelectedIndex]->Execute();
            }
        }
    }

    // �I����ԍX�V
    for (size_t i = 0; i < m_Stage.size(); i++)
    {
        m_Stage[i]->SetSelected(i == m_SelectedIndex);

        // �|�[�Y�̍X�V����
        m_Stage[i]->Update();
    }
}
//=============================================================================
// �`�揈��
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
// �\����Ԃ̐ݒ菈��
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
// �}�E�X�I������
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
// �X���C�h�C������
//=============================================================================
void CStageSelect::StartSlideIn(void)
{
    for (auto& stage : m_Stage)
    {
        stage->StartSlideIn(true);
    }
}
//=============================================================================
// �X���C�h�A�E�g����
//=============================================================================
void CStageSelect::StartSlideOut(void)
{
    for (auto& stage : m_Stage)
    {
        stage->StartSlideOut(true); // �e���ڂ��ړ��J�n
    }
}
//=============================================================================
// �X���C�h�A�E�g�I�����菈��
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


