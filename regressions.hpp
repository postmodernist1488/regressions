#include "functions.hpp"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <iomanip>

/* abstract interface for a regression that is able to 
 * draw current gradient descent state and the final (perfect) regression
 */
struct Regression {
    virtual void draw_description(int x, int y, int font_size, Color color) = 0; // draw the title and the function
    virtual void add_point(Vector2 point) = 0;                 // update the calculated (final) regression
    virtual void descent_step(std::vector<Vector2> &data) = 0; // do one gradient descent iteration
    virtual void reset() = 0;
};

struct LinearRegression : Regression {
    void add_point(Vector2 point) override {
        sx += point.x;
        sy += point.y;
        sxy += point.x * point.y;
        sx2 += std::pow(point.x, 2);
        n += 1;

        calculated.a = (sxy * n - sx * sy) / (sx2 * n - std::pow(sx, 2));
        calculated.b = (sy - calculated.a * sx) / n;

    }

    void descent_step(std::vector<Vector2> &data) override {
        const float a_weight = 0.000001f;
        const float b_weight = 0.1f;
        
        float a_gradient = 0.0f;
        for (auto [x, y]: data) {
            a_gradient += 2 * (descent.a * x + descent.b - y) * x;
        }
        float b_gradient = 0.0f;
        for (auto [x, y]: data) {
            b_gradient += 2 * (descent.a * x + descent.b - y);
        }

        if (data.size() > 0) {
            a_gradient /= data.size();
            b_gradient /= data.size();
        }
        descent.a -= a_gradient * a_weight;
        descent.b -= b_gradient * b_weight;
    }

    void draw_description(int x, int y, int font_size, Color color) override {
        DrawText("Linear regression", x, y, font_size, color);
        DrawText(TextFormat("y = %.4fx + %.4f", descent.a, descent.b), x, font_size + y, font_size, color);
    }

    void reset() override {
        n = sx = sy = sxy = sx2 = 0;
        descent = LinearFunction();
        calculated = LinearFunction();
    }

    LinearFunction descent, calculated;

    private:
        float sx = 0.0f,
              sy = 0.0f,
              sxy = 0.0f,
              sx2 = 0.0f;
        std::size_t n = 0;
};

inline float sqrt_signed(float x) {
    return x > 0 ? std::sqrt(x) : -std::sqrt(-x);
}

struct QuadraticRegression : Regression {
    void draw_description(int x, int y, int font_size, Color color) override {
        DrawText("Quadratic regression", x, y, font_size, color);
        DrawText(TextFormat("y = %.4f^2 + %.4fx + %.4f", descent.a, descent.b, descent.c), x, font_size + y, font_size, color);
    }
    void add_point(Vector2 point) override {
        sx4 += std::pow(point.x, 4);
        sx3 += std::pow(point.x, 3);
        sx2 += std::pow(point.x, 2);
        sx += point.x;
        sx2y += std::pow(point.x, 2) * point.y;
        sxy += point.x * point.y;
        sy += point.y;
        n += 1;

        calculated.a = (n*sx2*sx2y - n*sx3*sxy - std::pow(sx, 2)*sx2y + sx*sx2*sxy + sx*sx3*sy - std::pow(sx2, 2)*sy)
            /
            (n*sx2*sx4 - n*std::pow(sx3, 2) - std::pow(sx, 2)*sx4 + 2*sx*sx2*sx3 - std::pow(sx2, 3));
        calculated.b = (-calculated.a*n*sx3 + calculated.a*sx*sx2 + n*sxy - sx*sy)/(n*sx2 - std::pow(sx, 2));
        calculated.c = (-calculated.a*sx2 - calculated.b*sx + sy)/n;
    }

    virtual void descent_step(std::vector<Vector2> &data) override {
        const float a_weight = 0.00000002f;
        const float b_weight = 0.000001f;
        const float c_weight = 0.000001f;
        
        float a_gradient = 0.0f;
        for (auto [x, y]: data) {
            a_gradient += 2 * (descent.a * std::pow(x, 2) + descent.b * x + descent.c - y) * std::pow(x, 2);
        }
        float b_gradient = 0.0f;
        for (auto [x, y]: data) {
            b_gradient += 2 * (descent.a * std::pow(x, 2) + descent.b * x + descent.c - y) * x;
        }
        float c_gradient = 0.0f;
        for (auto [x, y]: data) {
            c_gradient += 2 * (descent.a * std::pow(x, 2) + descent.b * x + descent.c - y);
        }
        if (data.size() > 0) {
            a_gradient /= std::pow(data.size(), 2);
            b_gradient /= data.size();
            c_gradient /= data.size();
        }

        descent.a -= sqrt_signed(a_gradient) * a_weight;
        descent.b -= sqrt_signed(b_gradient) * b_weight;
        descent.c -= sqrt_signed(c_gradient) * c_weight;
    }

    void reset() override { 
        n = sx4 = sx3 = sx2 = sx = sx2y = sxy = sy;
        descent = QuadraticFunction();
        calculated = QuadraticFunction();
    }

    QuadraticFunction descent, calculated;

    private:
        float sx4 = 0.0f,
              sx3 = 0.0f,
              sx2 = 0.0f,
              sx  = 0.0f,
              sx2y = 0.0f,
              sxy = 0.0f,
              sy = 0.0f;
        std::size_t n = 0;
};

struct PowerRegression : Regression {
    void draw_description(int x, int y, int font_size, Color color) {
        DrawText("Power regression", x, y, font_size, color);
        DrawText(TextFormat("y = %.4f * %.4f^x", calculated.a, calculated.b), x, font_size + y, font_size, color);
    }
    void add_point(Vector2 point) {
        slnx += std::log(point.x);
        sln2x += std::pow(std::log(point.x), 2);
        slny += std::log(point.y);
        slnxlny += std::log(point.x) * std::log(point.y);
        n += 1;
        calculated.b = (n * slnxlny - slnx * slny) / (n * sln2x - std::pow(slnx, 2));
        calculated.a = std::exp((slny - calculated.b * slnx) / n);
    }

    void descent_step(std::vector<Vector2> &data) {
    }

    void reset() {
        n = slnx = sln2x = slny = slnxlny = 0.0f;
        descent = PowerFunction();
        calculated = PowerFunction();
    }

    PowerFunction descent, calculated;

    private:
        float slnx = 0.0f,
              sln2x = 0.0f,
              slny = 0.0f,
              slnxlny = 0.0f;
        std::size_t n = 0.0f;
};

struct ExponentialRegression : Regression {
    void draw_description(int x, int y, int font_size, Color color) {
    }
    void add_point(Vector2 point) {
    }
    void descent_step(std::vector<Vector2> &data) {
    }
    void reset() {
    }
};
