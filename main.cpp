#include <raylib.h>
#include <vector>
#include <cmath>
#include <iostream>

const int screen_width = 1000;
const int screen_height = 800;

void draw_dash_dotted_line(float start_x, float start_y, float end_x, float end_y, float dash_len, Color color) {
    const float len_x = end_x - start_x;
    const float len_y = end_y - start_y;
    const float step = dash_len / std::sqrt(std::pow(len_x, 2) + std::pow(len_y, 2));
    const float step_x = step * len_x;
    const float step_y = step * len_y;
    float cur_x = start_x,
          cur_y = start_y;

    // compare independently of direction
    while (len_x * (cur_x - end_x) < 0 || len_y * (cur_y - end_y) < 0) {
        float next_x = cur_x + step_x;
        if (len_x * (next_x - end_x) > 0) {
            next_x = end_x;
        }
        float next_y = cur_y + step_y;
        if (len_y * (next_y - end_y) > 0) {
            next_y = end_y;
        }
        DrawLine(cur_x, cur_y, next_x, next_y, color);
        cur_x += 2 * step_x;
        cur_y += 2 * step_y;
    }
}

/* abstract interface for a regression that is able to 
 * draw current gradient descent state and the final (perfect) regression
 */
struct Regression {
    virtual void draw_description(int x, int y, int font_size, Color color) = 0; // draw the title and the function
    virtual void add_point(Vector2 point) = 0;                 // update the calculated (final) regression
    virtual void descent_step(std::vector<Vector2> &data) = 0; // do one gradient descent iteration
    virtual void reset() = 0;
};

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
        DrawText(TextFormat("y = %.2fx + %.2f", descent.a, descent.b), x, font_size + y, font_size, color);
    }

    void reset() override {
        n = sx = sy = sxy = sx2 = 0;
    }

    LinearFunction descent, calculated;

    private:
        float sx = 0.0f,
              sy = 0.0f,
              sxy = 0.0f,
              sx2 = 0.0f;
        std::size_t n = 0;
};

float sqrt_signed(float x) {
    return x > 0 ? std::sqrt(x) : -std::sqrt(-x);
}

struct QuadraticRegression : Regression {
    void draw_description(int x, int y, int font_size, Color color) override {
        DrawText("Linear regression", x, y, font_size, color);
        DrawText(TextFormat("y = %.2fx^2 + %.2fx + %.2f", descent.a, descent.b, descent.c), x, font_size + y, font_size, color);
    }
    void add_point(Vector2 point) override {
    }

    virtual void descent_step(std::vector<Vector2> &data) {
        const float a_weight = 0.00000001f;
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
        descent.a = -0.1f;
        descent.b = 0.0f;
        descent.c = 500.0f;
    }

    QuadraticFunction descent, calculated;
};

enum REGRESSION_TYPE { LINEAR, QUADRATIC };

int main() {
    InitWindow(screen_width, screen_height, "Regressions");
    SetTargetFPS(60);

    std::vector<Vector2> data;
    REGRESSION_TYPE current_regression = QUADRATIC;
    LinearRegression lr;
    QuadraticRegression qr;
    qr.reset();

    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 point = {
                .x = (float) GetMouseX(),
                .y = (float) (screen_height - GetMouseY())
            };
            data.push_back(point);
            lr.add_point(point);
            qr.add_point(point);
        }

        if (IsKeyPressed(KEY_R)) {
            data.clear();
            lr.reset();
            qr.reset();
        }

        if (IsKeyPressed(KEY_F)) {
            SetTargetFPS(0);
        }
        if (IsKeyReleased(KEY_F)) {
            SetTargetFPS(60);
        }

        if (IsKeyPressed(KEY_ONE)) current_regression = LINEAR;
        if (IsKeyPressed(KEY_TWO)) current_regression = QUADRATIC;

        BeginDrawing();
            ClearBackground(RAYWHITE);


            switch(current_regression) {
                case LINEAR: 
                    {
                        lr.calculated.plot(GRAY);

                        lr.descent_step(data);

                        lr.descent.plot(BLACK);
                        lr.draw_description(30, 30, 30, GRAY);
                        float error = lr.descent.current_error(data);
                        DrawText(TextFormat("Error: %.02f", error), 30, 90, 30, GRAY);
                        for (auto [x, y] : data) {
                            DrawCircle(x, screen_height - y, 3.0f, RED);
                            draw_dash_dotted_line(x, screen_height - y, x, screen_height - lr.descent.evaluate_at(x), 4, BLUE);
                        }
                    }
                    break;
                case QUADRATIC: 
                    {   
                        qr.descent_step(data);
                        qr.descent.plot(BLACK);
                        for (auto [x, y] : data) {
                            DrawCircle(x, screen_height - y, 3.0f, RED);
                            //draw_dash_dotted_line(x, screen_height - y, x, screen_height - lr.descent.evaluate_at(x), 4, BLUE);
                        }

                    }
                    break;
            }

        EndDrawing();
    }

    return 0;
}
