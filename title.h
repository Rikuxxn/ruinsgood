//=============================================================================
//
// �^�C�g������ [title.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _TITLE_H_// ���̃}�N����`������Ă��Ȃ�������
#define _TITLE_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "scene.h"
#include "blockmanager.h"

//*****************************************************************************
// �^�C�g���N���X
//*****************************************************************************
class CTitle : public CScene
{
public:
	//�^�C�g���̎��
	typedef enum
	{
		TYPE_FIRST = 0,	// �^�C�g��
		TYPE_SECOND,	// PRESS
		TYPE_MAX
	}TYPE;

	CTitle();
	~CTitle();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

	// �e�摜�̍\����
	struct ImageInfo
	{
		D3DXVECTOR3 pos;
		float width;
		float height;
	};

	// ���_�͈̔͂��`����\����
	typedef struct
	{
		int start;
		int end;
	}VertexRange;

	// �^�C�v���Ƃɒ��_�͈͂�ݒ�
	VertexRange m_vertexRanges[TYPE_MAX];

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	int m_nIdxTextureTitle;
	int m_nIdxTexturePress;
	float m_alphaPress;          // ���݂̃��l�i0.0f �` 1.0f�j
	bool  m_isAlphaDown;         // �_�ŗp�t���O�i�グ��/������j
	bool  m_isEnterPressed;      // �G���^�[�L�[�����ꂽ

	CBlockManager* m_pBlockManager;		// �u���b�N�}�l�[�W���[�ւ̃|�C���^

};

#endif
