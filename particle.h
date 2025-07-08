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
// �}�N����`
//*****************************************************************************
#define MAX_PARTICLE (20)	// ���������������q�̐�


//*****************************************************************************
// �p�[�e�B�N���N���X
//*****************************************************************************
class CParticle : public CEffect
{
public:
	CParticle(int nPriority = 5);
	~CParticle();

	// �p�[�e�B�N���̎��
	typedef enum
	{
		PARTICLE_BACKFIRE = 0,
		PARTICLE_HORMING,
		PARTICLE_PLAYERBULLET,
		PARTICLE_ENEMYBULLET,
		PARTICLE_PLAYER_EXP,
		PARTICLE_ENEMY_EXP,
		PARTICLE_FLASH,
		PARTICLE_MAX
	}PARTICLETYPE;

	static CParticle*Create(D3DXVECTOR3 pos, D3DXCOLOR col, int nLife, int nType,int nMaxParticle);
	HRESULT Init(void);
	void Update(void);
	void SetParticle(int nType);
	void SetLifeParticle(int nLife);
private:
	int m_nType;		// ���
	int m_nLife;		// ����
	int m_nMaxParticle;	// ���q�̐�
};
#endif