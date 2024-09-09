#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"


// 三次元の回転
Matrix4x4 MakeRotateXMatrix(float radian);

Matrix4x4 MakeRotateYMatrix(float radian);

Matrix4x4 MakeRotateZMatrix(float radian);

// 4x4行列の和
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

// 4x4行列の差
Matrix4x4 Sub(const Matrix4x4& m1, const Matrix4x4& m2);

// 4x4行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

// ビューポート行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);


// アフィン返還
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

//座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m);

//線形補間
Vector3 Lerp(const Vector3& a, const Vector3& b, float t);

//ベクトル変換
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

//長さ
float Length(const Vector3& v);

//ベクトルの正規化
Vector3 Normalize(const Vector3& v);

//スカラー倍
Vector3 Multiply(float scalar, const Vector3& v);

// 行列とベクトルの掛け算関数
Vector3 Multiply(const Matrix4x4& mat, const Vector3& vec);

Vector3 Cross(const Vector3& v1, const Vector3& v2);

