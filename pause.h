//=============================================================================
//
// �|�[�Y���� [pause.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _PAUSE_H_// ���̃}�N����`������Ă��Ȃ�������
#define _PAUSE_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"
#include "object.h"


//*****************************************************************************
// �|�[�Y�N���X
//*****************************************************************************
class CPause : public CObject
{
public:
	static constexpr int MAX_PAUSE = 4;
	static constexpr float PAUSE_MIN_SCALE = 1.0f;
	static constexpr float PAUSE_MAX_SCALE = 1.2f;
	static constexpr float PAUSE_SCALE_SPEED = 0.02f;
	static constexpr float PAUSE_ALPHA_SPEED = 0.05f;

	// �I�����ڂ̎��
	typedef enum
	{
		MENU_CONTINUE = 0,		// �Q�[���ɖ߂�
		MENU_RETRY,				// �Q�[������蒼��
		MENU_QUIT,				// �^�C�g����ʂɖ߂�
		MENU_MAX
	}MENU;

	CPause(int nPriority = 7);
	~CPause();

	static CPause* Create(D3DXVECTOR3 pos,float fWidth,float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;						// �ʒu
	D3DCOLOR m_col;							// �F
	float m_fWidth, m_fHeight;				// �T�C�Y
	int m_fAlpha;							// ���݂̃A���t�@�l
	float m_fTime;							// ���Ԍo�ߗp
	int m_nIdxTexture;						// �e�N�X�`���C���f�b�N�X
	bool m_bPauseSelect;					// �I�΂�Ă��邩

};

#endif
