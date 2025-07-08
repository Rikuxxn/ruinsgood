//=============================================================================
//
// ���b�V���t�B�[���h���� [meshfield.h]
// Author : TANEKAWA RIKU
//
//=============================================================================
#ifndef _MESHFIELD_H_// ���̃}�N����`������Ă��Ȃ�������
#define _MESHFIELD_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"
#include "object.h"


//*****************************************************************************
// ���b�V���t�B�[���h�N���X
//*****************************************************************************
class CMeshfield : public CObject
{
public:
	CMeshfield();
	~CMeshfield();

	static CMeshfield* Create(D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);

private:
	int m_nIdxTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;		// �C���f�b�N�X�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;						// �ʒu
	D3DXVECTOR3 m_rot;						// ����
	D3DXMATRIX m_mtxWorld;					// ���[���h�}�g���b�N�X
	float m_fWidth;							// ��
	float m_fHeight;						// ����
	int m_nPrimitive;						// �v���~�e�B�u��
	int m_nVertex;							// ���_��

};

#endif