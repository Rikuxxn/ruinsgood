//=============================================================================
//
// 2D�I�u�W�F�N�g���� [object2D.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _OBJECT2D_H_// ���̃}�N����`������Ă��Ȃ�������
#define _OBJECT2D_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"
#include "object.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define OBJECT_WIDTH (55)
#define OBJECT_HEIGHT (35)


//*****************************************************************************
// 2D�I�u�W�F�N�g�N���X
//*****************************************************************************
class CObject2D : public CObject
{
public:
	CObject2D(int nPriority = 3);
	~CObject2D();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static CObject2D* Create(void);
	void SetPos(D3DXVECTOR3 pos);
	void SetRot(D3DXVECTOR3 rot);
	void SetCol(D3DCOLOR color);
	void SetSize(float fWidth,float fHeight);
	void SetScrollSpeed(float fUSpeed);
	void SetUV(int nTexU, int nTexV);
	D3DXVECTOR3 GetPos(void);
	D3DXVECTOR3 GetMove(void);
	D3DXVECTOR3 GetRot(void);
	D3DCOLOR GetCol(void);
	void TextureAnimExp(int nTexPosX,int nTexPosY, int nAnimSpeed);
	void TextureAnim(int nTexPosX, int nTexPosY, int nAnimSpeed);
	void ScrollTexture(void);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;						// �ʒu
	D3DXVECTOR3 m_rot;						// ����
	D3DXVECTOR3 m_move;						// �ړ���
	D3DCOLOR m_col;							// �F
	float m_fTexU;							// �e�N�X�`��UV
	float m_fTexV;							// �e�N�X�`��UV
	int m_nCounterAnim;						// �A�j���[�V�����J�E���^�[
	int m_nPatternAnim;						// �A�j���[�V�����p�^�[��No.
	float m_fWidth;							// ��
	float m_fHeight;						// ����
	float m_fLength;						// ����
	float m_fAngle;							// �p�x
	float m_fUSpeed;						// ���̃X�N���[���X�s�[�h
	float m_fVSpeed;						// �c�̃X�N���[���X�s�[�h
	float m_aPosTexU;						// ���̃X�N���[���p�ϐ�
};

#endif