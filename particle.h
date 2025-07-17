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
		TYPE_AURA2,
		TYPE_MAX
	}TYPE;

	static CParticle*Create(D3DXVECTOR3 pos, D3DXCOLOR col, int nLife, int nType,int nMaxParticle);
	HRESULT Init(void);
	void Update(void);

	void SetParticle(int nType) { m_nType = nType; }
	void SetLife(int nLife) { m_nLife = nLife; }

	int GetLife(void) { return m_nLife; }
	int GetMaxParticle(void) { return m_nMaxParticle; }

private:
	int m_nType;		// ���
	int m_nLife;		// ����
	int m_nMaxParticle;	// ���q�̐�
};

//*****************************************************************************
// ���p�[�e�B�N���N���X
//*****************************************************************************
class CFireParticle : public CParticle
{
public:
	CFireParticle();
	~CFireParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// �����Ԃ��p�[�e�B�N���N���X
//*****************************************************************************
class CWaterParticle : public CParticle
{
public:
	CWaterParticle();
	~CWaterParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// �I�[���p�[�e�B�N���N���X
//*****************************************************************************
class CAuraParticle : public CParticle
{
public:
	CAuraParticle();
	~CAuraParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// �I�[��(���ʗp)�p�[�e�B�N���N���X
//*****************************************************************************
class CAura2Particle : public CParticle
{
public:
	CAura2Particle();
	~CAura2Particle();

	HRESULT Init(void);
	void Update(void);

private:

};

#endif