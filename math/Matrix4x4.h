#pragma once
/// <summary>
/// 4x4行列
/// </summary>
struct Matrix4x4 final {
	float m[4][4];

	Matrix4x4& operator*=(const Matrix4x4& other) {
		float result[4][4] = {{0}};
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				for (int k = 0; k < 4; ++k) {
					result[i][j] += m[i][k] * other.m[k][j];
				}
			}
		}
		// 結果を現在のインスタンスに反映
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				m[i][j] = result[i][j];
			}
		}
		return *this;
	}
	Matrix4x4 operator*(const Matrix4x4& other) const {
		Matrix4x4 result;

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = 0;
				for (int k = 0; k < 4; ++k) {
					result.m[i][j] += m[i][k] * other.m[k][j];
				}
			}
		}

		return result;
	}
};
