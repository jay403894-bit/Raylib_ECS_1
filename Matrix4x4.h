#pragma once
#include <cmath>

struct Matrix4x4 {
    float m[4][4];  // 4x4 matrix array

    Matrix4x4() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = (i == j) ? 1.0f : 0.0f; // Identity matrix
            }
        }
    }

    // Matrix multiplication
    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 4; k++) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    // Matrix translation
    static Matrix4x4 Translate(float x, float y) {
        Matrix4x4 result;
        result.m[3][0] = x;
        result.m[3][1] = y;
        return result;
    }

    // Matrix scaling
    static Matrix4x4 Scale(float scaleX, float scaleY) {
        Matrix4x4 result;
        result.m[0][0] = scaleX;
        result.m[1][1] = scaleY;
        return result;
    }

    // Matrix rotation (counter-clockwise in radians)
    static Matrix4x4 Rotate(float radians) {
        Matrix4x4 result;
        result.m[0][0] = cosf(radians);
        result.m[0][1] = -sinf(radians);
        result.m[1][0] = sinf(radians);
        result.m[1][1] = cosf(radians);
        return result;
    }

    // Convert matrix to float array for Raylib (if needed)
    operator float* () {
        return &m[0][0];
    }
};