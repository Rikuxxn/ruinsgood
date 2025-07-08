//=============================================================================
//
// �r���{�[�h���� [objectBillboard.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _OBJECTBILLBOARD_H_// ���̃}�N����`������Ă��Ȃ�������
#define _OBJECTBILLBOARD_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"
#include "object.h"


//*****************************************************************************
// �r���{�[�h�N���X
//*****************************************************************************
class CObjectBillboard : public CObject
{
public:
	CObjectBillboard();
	~CObjectBillboard();

	//�r���{�[�h�̎��
	typedef enum
	{
		TYPE_ONE = 0,
		TYPE_LIGHT,
		TYPE_MAX
	}TYPE;

	static CObjectBillboard* Create(TYPE type, D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;
	TYPE m_nType;
	D3DXMATRIX m_mtxWorld;
	float m_fWidth;			// ��
	float m_fHeight;		// ����
	int m_nIdxTexture;

	const char* BILLBOARD_TEXTURE[TYPE_MAX] =//�r���{�[�h�̃e�N�X�`���̐ݒ�
	{
		"data/TEXTURE/selectBG.png",	// 1
		"data/TEXTURE/light.png",		// 2

	};

};
#endif