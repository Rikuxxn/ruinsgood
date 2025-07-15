//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _PARTICLE_H_// ���̃}�N����`������Ă��Ȃ�������
#define _PARTICLE_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"
#include "effect.h"

//*****************************************************************************
// �p�[�e�B�N���N���X
//*****************************************************************************
class CParticle : public CEffect
{
public:
	CParticle();
	~CParticle();

	// �p�[�e�B�N���̎��
	typedef enum
	{
		TYPE_FIRE = 0,
		TYPE_WATER,
		TYPE_AURA,
		TYPE_MAX
	}TYPE;

	static CParticle*Create(D3DXVECTOR3 pos, D3DXCOLOR col, int nLife, int nType,int nMaxParticle);
	HRESULT Init(void);
	virtual void Update(void);
	void SetParticle(int nType);
	void SetLife(int nLife);
private:
	int m_nType;		// ���
	int m_nLife;		// ����
	int m_nMaxParticle;	// ���q�̐�
	CEffect* m_pEffect;	// �G�t�F�N�g�ւ̃|�C���^
};

//*****************************************************************************
// ���p�[�e�B�N���N���X
//*****************************************************************************
class CFireParticle : public CParticle
{
public:
	CFireParticle();
	~CFireParticle();

	static CFireParticle* Create(D3DXVECTOR3 pos, D3DXCOLOR col, int nLife, int nType, int nMaxParticle);
	void Update(void) override;

private:
	D3DXVECTOR3 m_move;
	int m_nLife;
};


//*****************************************************************************
// �����Ԃ��p�[�e�B�N���N���X
//*****************************************************************************

#endif