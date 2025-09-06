//=============================================================================
//
// 当たり判定処理 [collisionUtils.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "collisionUtils.h"


//=============================================================================
// シリンダーとAABBの当たり判定(その距離を返す)
//=============================================================================
bool CCollision::CheckCylinderAABBCollisionWithHitDistance(
	const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight, const D3DXVECTOR3& cylinderDir,
	const D3DXVECTOR3& aabbMin, const D3DXVECTOR3& aabbMax,
	float* outHitDistance)
{
	// シリンダー軸方向のベクトル
	D3DXVECTOR3 axis = cylinderDir;

	// AABBの8頂点を取得
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

	// 頂点群を軸方向に射影して最小最大を求める（軸方向の範囲）
	float minProj = FLT_MAX;
	float maxProj = -FLT_MAX;
	for (int i = 0; i < 8; i++)
	{
		D3DXVECTOR3 v = verts[i] - cylinderCenter;
		float proj = D3DXVec3Dot(&v, &axis);
		if (proj < minProj) minProj = proj;
		if (proj > maxProj) maxProj = proj;
	}

	// 軸方向の距離判定
	float halfHeight = cylinderHeight * 0.5f;

	if (maxProj < -halfHeight || minProj > halfHeight)
	{
		// 軸方向で重なりなし
		return false;
	}

	// 軸方向に被っている部分の距離をclampして判定軸上の点を取得
	float clampProj = std::max(-halfHeight, std::min(0.5f * (minProj + maxProj), halfHeight));
	D3DXVECTOR3 closestPointOnAxis = cylinderCenter + axis * clampProj;

	// 軸に垂直な2つのベクトルを計算（シリンダー軸に垂直）
	D3DXVECTOR3 up = axis;
	D3DXVECTOR3 right, forward;

	// axisに直交するベクトルを1つ決める
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

	// AABBの頂点を断面座標系に変換して、最小最大を求める（2D長方形の境界）
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

	// 矩形の最近接点を円の中心に投影（クランプ）
	float closestX = std::max(minX, std::min(0.0f, maxX));
	float closestY = std::max(minY, std::min(0.0f, maxY));

	// 中心からの距離
	float distX = closestX;
	float distY = closestY;

	float distSq = distX * distX + distY * distY;

	if (distSq <= cylinderRadius * cylinderRadius)
	{
		if (outHitDistance)
		{
			// 発射位置から求める
			D3DXVECTOR3 startPos = cylinderCenter + axis * halfHeight;	// 後端
			D3DXVECTOR3 hitPos = cylinderCenter + axis * clampProj;		// 当たった位置
			D3DXVECTOR3 disPos = hitPos - startPos;
			*outHitDistance = D3DXVec3Length(&disPos);
		}

		// 当たり判定あり
		return true;
	}

	return false;
}
//=============================================================================
// シリンダーとカプセルの当たり判定
//=============================================================================
bool CCollision::CheckCapsuleCylinderCollision_Dir(
	const D3DXVECTOR3& capsuleCenter, float capsuleRadius, float capsuleHeight,
	const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight,
	const D3DXVECTOR3& cylinderDir, bool flag)
{

	float collisionCylinderRadius;
	float collisionCapsuleRadius;

	// flagは使わなければfalseでいい
	if (flag)
	{
		// ==== 半径を縮小 ====
		collisionCylinderRadius = cylinderRadius * 0.40f; // 円柱は50%
		collisionCapsuleRadius = capsuleRadius * 0.85f;   // カプセルは90%
	}
	else
	{
		collisionCylinderRadius = cylinderRadius;
		collisionCapsuleRadius = capsuleRadius;
	}

	// ベクトル差
	D3DXVECTOR3 delta = capsuleCenter - cylinderCenter;

	// 軸方向に射影
	float proj = D3DXVec3Dot(&delta, &cylinderDir);

	// 軸上距離判定
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
		axisDist = 0.0f; // 軸上は重なっている
	}

	// 軸直交面の距離
	D3DXVECTOR3 closestPointOnAxis = cylinderCenter + cylinderDir * proj;
	D3DXVECTOR3 diff = capsuleCenter - closestPointOnAxis;
	float radialDist = D3DXVec3Length(&diff);

	// ==== 判定 ====
	if (axisDist <= collisionCapsuleRadius + collisionCylinderRadius &&
		radialDist <= collisionCapsuleRadius + collisionCylinderRadius)
	{
		return true;
	}

	return false;
}
//=============================================================================
// AABBとAABBの当たり判定
//=============================================================================
bool CCollision::CheckCollisionAABB(D3DXVECTOR3 pos1, D3DXVECTOR3 modelSize1, D3DXVECTOR3 scale1, 
	D3DXVECTOR3 pos2, D3DXVECTOR3 modelSize2, D3DXVECTOR3 scale2)
{
	//=========================================================================
	// ブロック1の情報
	//=========================================================================

	// AABBを取得
	D3DXVECTOR3 Pos_1 = pos1;				// ブロックの位置
	D3DXVECTOR3 ModelSize_1 = modelSize1;	// ブロックの元のサイズ（中心原点）
	D3DXVECTOR3 Scale_1 = scale1;			// ブロックの拡大率

	// 元サイズに拡大率を適用する
	D3DXVECTOR3 Size_1;
	Size_1.x = ModelSize_1.x * Scale_1.x;
	Size_1.y = ModelSize_1.y * Scale_1.y;
	Size_1.z = ModelSize_1.z * Scale_1.z;

	// 最小値と最大値を求める
	D3DXVECTOR3 Min_1 = Pos_1 - Size_1 * 0.5f;
	D3DXVECTOR3 Max_1 = Pos_1 + Size_1 * 0.5f;


	//=========================================================================
	// ブロック2の情報
	//=========================================================================

	// AABBを取得
	D3DXVECTOR3 Pos_2 = pos2;				// ブロックの位置
	D3DXVECTOR3 ModelSize_2 = modelSize2;	// ブロックの元のサイズ（中心原点）
	D3DXVECTOR3 Scale_2 = scale2;			// ブロックの拡大率

	// 元サイズに拡大率を適用する
	D3DXVECTOR3 Size_2;
	Size_2.x = ModelSize_2.x * Scale_2.x;
	Size_2.y = ModelSize_2.y * Scale_2.y;
	Size_2.z = ModelSize_2.z * Scale_2.z;

	// 最小値と最大値を求める
	D3DXVECTOR3 Min_2 = Pos_2 - Size_2 * 0.5f;
	D3DXVECTOR3 Max_2 = Pos_2 + Size_2 * 0.5f;

	//=========================================================================
	// AABB同士の交差チェック
	//=========================================================================
	bool isOverlap =
		Min_1.x <= Max_2.x && Max_1.x >= Min_2.x &&
		Min_1.y <= Max_2.y && Max_1.y >= Min_2.y &&
		Min_1.z <= Max_2.z && Max_1.z >= Min_2.z;

	if (isOverlap)
	{// 交差している
		return true;
	}

	return false;
}
