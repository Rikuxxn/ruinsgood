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

	// 円（シリンダー半径）と矩形（AABB断面）との2D判定
	// 円の中心は(0,0)、半径はcylinderRadius

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
			// 距離は中心からclampProjへの距離を正の数に変換して渡す
			*outHitDistance = halfHeight - fabs(clampProj);
		}

		// 当たり判定あり
		return true;
	}

	return false;
}
