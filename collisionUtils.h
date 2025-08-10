//=============================================================================
//
// �����蔻�菈�� [collisionUtils.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _COLLISIONUTILS_H_// ���̃}�N����`������Ă��Ȃ�������
#define _COLLISIONUTILS_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************

//*****************************************************************************
// �����蔻��N���X
//*****************************************************************************
class CCollision
{
public:

	// �V�����_�[��AABB�̓����蔻��(���̋�����Ԃ�)
	static bool CheckCylinderAABBCollisionWithHitDistance(
		const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight, const D3DXVECTOR3& cylinderDir,
		const D3DXVECTOR3& aabbMin, const D3DXVECTOR3& aabbMax,
		float* outHitDistance);

};


#endif