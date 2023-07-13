#pragma once
#include <raylib.h>
#include <vector>
#include <cmath>

extern const int screen_width, screen_height;

struct Function {
    virtual float evaluate_at(float x) = 0;
    void plot(Color color) {
        float prev = evaluate_at(0);
        for (int x = 1; x < screen_width; ++x) {
            float cur = evaluate_at(x);
            if (0 < cur && cur < screen_height || 0 < prev && prev < screen_height) {
                DrawLine(x - 1, screen_height - prev, x, screen_height - cur, color);
            }
            prev = cur;

        }
    }
    float current_error(std::vector<Vector2> &data) {
        float e = 0.0f;
        for (auto [x, y] : data) {
            e += std::pow(evaluate_at(x) - y, 2);
        }
        return data.size() > 0 ? e / data.size(): e;
    }
};

struct LinearFunction : Function {

    //draws y = ax + b through the entire screen
    static void draw_line(float a, float b, Color color) {
        DrawLine(0, screen_height - (a * 0 + b), screen_width, screen_height - (a * screen_width + b), color);
    }
    //small optimization
    void plot(Color color) {
        for (int x = 0; x < screen_width; ++x) {
            DrawPixel(x, screen_height - evaluate_at(x), color);
        }
    }
    float evaluate_at(float x) {
        return a * x + b;
    }
    float a = 0.0f, b = 0.0f;
};

struct QuadraticFunction : Function {
    float evaluate_at(float x) {
        return a * std::pow(x, 2) + b * x + c;
    }
    float a = 0.0f, b = 0.0f, c = 0.0f;
};

struct PowerFunction : Function {
    float evaluate_at(float x) {
        return a * std::pow(x, b);
    }
    float a = 0.0f, b = 0.0f;
};

struct ExponentialFunction : Function {
    float evaluate_at(float x) {
        return a * std::pow(b, x);
    }
    float a = 0.0f, b = 0.0f;
};
