//=============================================================================
//
// �����蔻�菈�� [collisionUtils.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "collisionUtils.h"


//=============================================================================
// �V�����_�[��AABB�̓����蔻��(���̋�����Ԃ�)
//=============================================================================
bool CCollision::CheckCylinderAABBCollisionWithHitDistance(
	const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight, const D3DXVECTOR3& cylinderDir,
	const D3DXVECTOR3& aabbMin, const D3DXVECTOR3& aabbMax,
	float* outHitDistance)
{
	// �V�����_�[�������̃x�N�g��
	D3DXVECTOR3 axis = cylinderDir;

	// AABB��8���_���擾
	D3DXVECTOR3 verts[8] =
	{
		{aabbMin.x, aabbMin.y, aabbMin.z},
		{aabbMax.x, aabbMin.y, aabbMin.z},
		{aabbMin.x, aabbMax.y, aabbMin.z},
		{aabbMax.x, aabbMax.y, aabbMin.z},
		{aabbMin.x, aabbMin.y, aabbMax.z},
		{aabbMax.x, aabbMin.y, aabbMax.z},
		{aabbMin.x, aabbMax.y, aabbMax.z},
		{aabbMax.x, aabbMax.y, aabbMax.z},
	};

	// ���_�Q���������Ɏˉe���čŏ��ő�����߂�i�������͈̔́j
	float minProj = FLT_MAX;
	float maxProj = -FLT_MAX;
	for (int i = 0; i < 8; i++)
	{
		D3DXVECTOR3 v = verts[i] - cylinderCenter;
		float proj = D3DXVec3Dot(&v, &axis);
		if (proj < minProj) minProj = proj;
		if (proj > maxProj) maxProj = proj;
	}

	// �������̋�������
	float halfHeight = cylinderHeight * 0.5f;

	if (maxProj < -halfHeight || minProj > halfHeight)
	{
		// �������ŏd�Ȃ�Ȃ�
		return false;
	}

	// �������ɔ���Ă��镔���̋�����clamp���Ĕ��莲��̓_���擾
	float clampProj = std::max(-halfHeight, std::min(0.5f * (minProj + maxProj), halfHeight));
	D3DXVECTOR3 closestPointOnAxis = cylinderCenter + axis * clampProj;

	// ���ɐ�����2�̃x�N�g�����v�Z�i�V�����_�[���ɐ����j
	D3DXVECTOR3 up = axis;
	D3DXVECTOR3 right, forward;

	// axis�ɒ�������x�N�g����1���߂�
	if (fabs(up.x) < 0.9f)
	{
		right = D3DXVECTOR3(1, 0, 0);
	}
	else
	{
		right = D3DXVECTOR3(0, 1, 0);
	}

	D3DXVec3Cross(&right, &right, &up);
	D3DXVec3Normalize(&right, &right);

	D3DXVec3Cross(&forward, &up, &right);
	D3DXVec3Normalize(&forward, &forward);

	// AABB�̒��_��f�ʍ��W�n�ɕϊ����āA�ŏ��ő�����߂�i2D�����`�̋��E�j
	float minX = FLT_MAX, maxX = -FLT_MAX;
	float minY = FLT_MAX, maxY = -FLT_MAX;
	for (int i = 0; i < 8; i++)
	{
		D3DXVECTOR3 v = verts[i] - closestPointOnAxis;
		float x = D3DXVec3Dot(&v, &right);
		float y = D3DXVec3Dot(&v, &forward);
		if (x < minX) minX = x;
		if (x > maxX) maxX = x;
		if (y < minY) minY = y;
		if (y > maxY) maxY = y;
	}

	// �~�i�V�����_�[���a�j�Ƌ�`�iAABB�f�ʁj�Ƃ�2D����
	// �~�̒��S��(0,0)�A���a��cylinderRadius

	// ��`�̍ŋߐړ_���~�̒��S�ɓ��e�i�N�����v�j
	float closestX = std::max(minX, std::min(0.0f, maxX));
	float closestY = std::max(minY, std::min(0.0f, maxY));

	// ���S����̋���
	float distX = closestX;
	float distY = closestY;

	float distSq = distX * distX + distY * distY;

	if (distSq <= cylinderRadius * cylinderRadius)
	{
		if (outHitDistance)
		{
			// �����͒��S����clampProj�ւ̋����𐳂̐��ɕϊ����ēn��
			*outHitDistance = halfHeight - fabs(clampProj);
		}

		// �����蔻�肠��
		return true;
	}

	return false;
}
