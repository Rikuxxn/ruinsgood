//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PARTICLE_H_// ���̃}�N����`������Ă��Ȃ�������
#define _PARTICLE_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"
#include "effect.h"
#include <unordered_map>
#include <functional>

class CParticle;

using ParticleCreateFunc = std::function<CParticle* ()>;

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
		TYPE_FLAMETHROWER,
		TYPE_WATER,
		TYPE_AURA,
		TYPE_AURA2,
		TYPE_MOVE,
		TYPE_MAX
	}TYPE;

	static CParticle* Create(D3DXVECTOR3 dir, D3DXVECTOR3 pos, D3DXCOLOR col, int nLife, TYPE type, int nMaxParticle);
	static void InitFactory(void);
	HRESULT Init(void);
	void Update(void);

	void SetParticle(TYPE type) { m_nType = type; }
	void SetLife(int nLife) { m_nLife = nLife; }
	void SetDir(D3DXVECTOR3 dir) { m_Dir = dir; }

	int GetLife(void) { return m_nLife; }
	int GetMaxParticle(void) { return m_nMaxParticle; }
	D3DXVECTOR3 GetDir(void) { return m_Dir; }

private:
	D3DXVECTOR3 m_Dir;	// ����
	TYPE m_nType;		// ���
	int m_nLife;		// ����
	int m_nMaxParticle;	// ���q�̐�
	static std::unordered_map<TYPE, ParticleCreateFunc> m_ParticleFactoryMap;
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
// �Ή����˃p�[�e�B�N���N���X
//*****************************************************************************
class CFlamethrowerParticle : public CParticle
{
public:
	CFlamethrowerParticle();
	~CFlamethrowerParticle();

	HRESULT Init(void);
	void Update(void);

	void SetSpeedScale(float scale) { m_speedScale = scale; }
	float GetSpeedScale() const { return m_speedScale; }

private:
	float m_speedScale;  // ���x�{��
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

//*****************************************************************************
// �ړ����p�[�e�B�N���N���X
//*****************************************************************************
class CMoveParticle : public CParticle
{
public:
	CMoveParticle();
	~CMoveParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

#endif