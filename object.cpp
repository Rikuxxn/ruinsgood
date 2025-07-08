//=============================================================================
//
// �I�u�W�F�N�g���� [object.cpp]
// Author : TANEKAWA RIKU
//
//=============================================================================
// 
//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"
#include "camera.h"
#include "manager.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
CObject* CObject::m_apTop[MAX_OBJ_PRIORITY] = {};
CObject* CObject::m_apCur[MAX_OBJ_PRIORITY] = {};
int CObject::m_nNumAll = 0;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CObject::CObject(int nPriority)
{
	m_nPriority = 0;
	m_type = TYPE_NONE;

	// �I�u�W�F�N�g(����)�����X�g�ɒǉ�

	m_pPrev = m_apCur[nPriority];

	if (m_pPrev != NULL)
	{
		m_pPrev->m_pNext = this;
	}

	m_pNext = NULL;

	if (m_apTop[nPriority] == NULL)
	{
		m_apTop[nPriority] = this;
	}

	m_apCur[nPriority] = this;

	m_nNumAll++;
	m_nPriority = nPriority;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CObject::~CObject()
{
	// ���͂Ȃ�
}
//=============================================================================
// ���ׂẴI�u�W�F�N�g�̔j��
//=============================================================================
void CObject::ReleaseAll(void)
{
	for (int nPriority = 0; nPriority < MAX_OBJ_PRIORITY; nPriority++)
	{
		CObject* pObject = m_apTop[nPriority];

		while (pObject != NULL)
		{
			CObject* pObjectNext = pObject->m_pNext;// ���̃I�u�W�F�N�g��ۑ�

			// �I������
			pObject->Uninit();

			pObject = pObjectNext;// ���̃I�u�W�F�N�g����
		}
	}
}
//=============================================================================
// ���ׂẴI�u�W�F�N�g�̍X�V����
//=============================================================================
void CObject::UpdateAll(void)
{
	for (int nPriority = 0; nPriority < MAX_OBJ_PRIORITY; nPriority++)
	{
		CObject* pObject = m_apTop[nPriority];

		while (pObject != NULL)
		{
			CObject* pObjectNext = pObject->m_pNext;// ���̃I�u�W�F�N�g��ۑ�

			// �X�V����
			pObject->Update();

			pObject = pObjectNext;// ���̃I�u�W�F�N�g����
		}
	}
}
//=============================================================================
// ���ׂẴI�u�W�F�N�g�̕`�揈��
//=============================================================================
void CObject::DrawAll(void)
{
	//�J�����̎擾
	CCamera* pCamera = CManager::GetCamera();

	// �J�����̐ݒ�
	pCamera->SetCamera();

	for (int nPriority = 0; nPriority < MAX_OBJ_PRIORITY; nPriority++)
	{
		CObject* pObject = m_apTop[nPriority];

		while (pObject != NULL)
		{
			CObject* pObjectNext = pObject->m_pNext;// ���̃I�u�W�F�N�g��ۑ�

			// �`�揈��
			pObject->Draw();

			pObject = pObjectNext;// ���̃I�u�W�F�N�g����
		}
	}
}
//=============================================================================
// �I�u�W�F�N�g�̔j��
//=============================================================================
void CObject::Release(void)
{
	int nPriority = this->m_nPriority;

	if (m_pPrev != NULL && m_pNext != NULL)
	{// �Ԃ��������Ƃ�
		m_pNext->m_pPrev = m_pPrev;
		m_pPrev->m_pNext = m_pNext;
	}
	else if (m_pPrev == NULL && m_pNext != NULL)
	{// �擪���������Ƃ�
		m_pNext->m_pPrev = NULL;
		m_apTop[nPriority] = m_pNext;
	}
	else if (m_pPrev != NULL && m_pNext == NULL)
	{// �Ō�����������Ƃ�
		m_pPrev->m_pNext = NULL;
		m_apCur[nPriority] = m_pPrev;
	}

	m_nNumAll--;

	delete this;
}
//=============================================================================
// �I�u�W�F�N�g�̑����̎擾
//=============================================================================
int CObject::GetNumObject(void)
{
	return m_nNumAll;
}
////=============================================================================
//// �I�u�W�F�N�g�̎擾
////=============================================================================
//CObject* CObject::GetObject(int nPriority,int nIdx)
//{
//	return m_apObject[nPriority][nIdx];
//}
//=============================================================================
// �^�C�v�̐ݒ菈��
//=============================================================================
void CObject::SetType(TYPE type)
{
	m_type = type;
}
//=============================================================================
// �^�C�v�̎擾
//=============================================================================
CObject::TYPE CObject::GetType(void)
{
	return m_type;
}
