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
			// ���ˈʒu���狁�߂�
			D3DXVECTOR3 startPos = cylinderCenter + axis * halfHeight;	// ��[
			D3DXVECTOR3 hitPos = cylinderCenter + axis * clampProj;		// ���������ʒu
			D3DXVECTOR3 disPos = hitPos - startPos;
			*outHitDistance = D3DXVec3Length(&disPos);
		}

		// �����蔻�肠��
		return true;
	}

	return false;
}
//=============================================================================
// �V�����_�[�ƃJ�v�Z���̓����蔻��
//=============================================================================
bool CCollision::CheckCapsuleCylinderCollision_Dir(
	const D3DXVECTOR3& capsuleCenter, float capsuleRadius, float capsuleHeight,
	const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight,
	const D3DXVECTOR3& cylinderDir, bool flag)
{

	float collisionCylinderRadius;
	float collisionCapsuleRadius;

	// flag�͎g��Ȃ����false�ł���
	if (flag)
	{
		// ==== ���a���k�� ====
		collisionCylinderRadius = cylinderRadius * 0.40f; // �~����50%
		collisionCapsuleRadius = capsuleRadius * 0.85f;   // �J�v�Z����90%
	}
	else
	{
		collisionCylinderRadius = cylinderRadius;
		collisionCapsuleRadius = capsuleRadius;
	}

	// �x�N�g����
	D3DXVECTOR3 delta = capsuleCenter - cylinderCenter;

	// �������Ɏˉe
	float proj = D3DXVec3Dot(&delta, &cylinderDir);

	// ���㋗������
	float halfHeightSum = (capsuleHeight * 0.5f) + (cylinderHeight * 0.5f);
	float axisDist = 0.0f;

	if (proj > halfHeightSum)
	{
		axisDist = proj - halfHeightSum;
	}
	else if (proj < -halfHeightSum)
	{
		axisDist = -halfHeightSum - proj;
	}
	else
	{
		axisDist = 0.0f; // ����͏d�Ȃ��Ă���
	}

	// ������ʂ̋���
	D3DXVECTOR3 closestPointOnAxis = cylinderCenter + cylinderDir * proj;
	D3DXVECTOR3 diff = capsuleCenter - closestPointOnAxis;
	float radialDist = D3DXVec3Length(&diff);

	// ==== ���� ====
	if (axisDist <= collisionCapsuleRadius + collisionCylinderRadius &&
		radialDist <= collisionCapsuleRadius + collisionCylinderRadius)
	{
		return true;
	}

	return false;
}
//=============================================================================
// AABB��AABB�̓����蔻��
//=============================================================================
bool CCollision::CheckCollisionAABB(D3DXVECTOR3 pos1, D3DXVECTOR3 modelSize1, D3DXVECTOR3 scale1, 
	D3DXVECTOR3 pos2, D3DXVECTOR3 modelSize2, D3DXVECTOR3 scale2)
{
	//=========================================================================
	// �u���b�N1�̏��
	//=========================================================================

	// AABB���擾
	D3DXVECTOR3 Pos_1 = pos1;				// �u���b�N�̈ʒu
	D3DXVECTOR3 ModelSize_1 = modelSize1;	// �u���b�N�̌��̃T�C�Y�i���S���_�j
	D3DXVECTOR3 Scale_1 = scale1;			// �u���b�N�̊g�嗦

	// ���T�C�Y�Ɋg�嗦��K�p����
	D3DXVECTOR3 Size_1;
	Size_1.x = ModelSize_1.x * Scale_1.x;
	Size_1.y = ModelSize_1.y * Scale_1.y;
	Size_1.z = ModelSize_1.z * Scale_1.z;

	// �ŏ��l�ƍő�l�����߂�
	D3DXVECTOR3 Min_1 = Pos_1 - Size_1 * 0.5f;
	D3DXVECTOR3 Max_1 = Pos_1 + Size_1 * 0.5f;


	//=========================================================================
	// �u���b�N2�̏��
	//=========================================================================

	// AABB���擾
	D3DXVECTOR3 Pos_2 = pos2;				// �u���b�N�̈ʒu
	D3DXVECTOR3 ModelSize_2 = modelSize2;	// �u���b�N�̌��̃T�C�Y�i���S���_�j
	D3DXVECTOR3 Scale_2 = scale2;			// �u���b�N�̊g�嗦

	// ���T�C�Y�Ɋg�嗦��K�p����
	D3DXVECTOR3 Size_2;
	Size_2.x = ModelSize_2.x * Scale_2.x;
	Size_2.y = ModelSize_2.y * Scale_2.y;
	Size_2.z = ModelSize_2.z * Scale_2.z;

	// �ŏ��l�ƍő�l�����߂�
	D3DXVECTOR3 Min_2 = Pos_2 - Size_2 * 0.5f;
	D3DXVECTOR3 Max_2 = Pos_2 + Size_2 * 0.5f;

	//=========================================================================
	// AABB���m�̌����`�F�b�N
	//=========================================================================
	bool isOverlap =
		Min_1.x <= Max_2.x && Max_1.x >= Min_2.x &&
		Min_1.y <= Max_2.y && Max_1.y >= Min_2.y &&
		Min_1.z <= Max_2.z && Max_1.z >= Min_2.z;

	if (isOverlap)
	{// �������Ă���
		return true;
	}

	return false;
}
