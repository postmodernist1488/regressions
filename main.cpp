#include <raylib.h>
#include <vector>
#include <cmath>
#include <iostream>

const int screen_width = 1000;
const int screen_height = 800;

/* abstract interface for a regression that is able to 
 * draw current gradient descent state and the final (perfect) regression
 */
struct Regression {
    virtual void draw_current(Color color) = 0; // draw current descent regression
    virtual void draw_final(Color color) = 0;   // draw final regression
    virtual void add_point(Vector2 point) = 0;  // update the calculated (final) regression
    virtual void descent_step(std::vector<Vector2> &data) = 0; // do one gradient descent iteration

};

// TODO: line class
struct LinearRegression : Regression {
    
    void draw_current(Color color) override {
        draw_line(descent_a, descent_b, color);
    }

    void draw_final(Color color) override {
        draw_line(calculated_a, calculated_b, color);
    }

    void add_point(Vector2 point) override {
        sx += point.x;
        sy += point.y;
        sxy += point.x * point.y;
        sx2 += std::pow(point.x, 2);
        n += 1;

        calculated_a = (sxy * n - sx * sy) / (sx2 * n - std::pow(sx, 2));
        calculated_b = (sy - calculated_a * sx) / n;

    }

    void descent_step(std::vector<Vector2> &data) {
        const float a_weight = 0.000001f;
        const float b_weight = 0.1f;
        
        float a_gradient = 0.0f;
        for (auto [x, y]: data) {
            a_gradient += 2 * (descent_a * x + descent_b - y) * x;
        }
        float b_gradient = 0.0f;
        for (auto [x, y]: data) {
            b_gradient += 2 * (descent_a * x + descent_b - y);
        }

        if (data.size() > 0) {
            a_gradient /= data.size();
            b_gradient /= data.size();
        }
        descent_a -= a_gradient * a_weight;
        descent_b -= b_gradient * b_weight;
    }

    //draws y = ax + b through the entire screen
    static void draw_line(float a, float b, Color color) {
        DrawLine(0, screen_height - (a * 0 + b), screen_width, screen_height - (a * screen_width + b), color);
    }

    private:
        float sx = 0.0f,
              sy = 0.0f,
              sxy = 0.0f,
              sx2 = 0.0f;
        float descent_a = 0.0f, descent_b = 0.0f;
        float calculated_a = 0.0f, calculated_b = 0.0f;
        std::size_t n = 0;
};

int main() {
    InitWindow(screen_width, screen_height, "Regressions");
    SetTargetFPS(60);


    std::vector<Vector2> data;
    LinearRegression lr;

    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 point = {
                .x = (float) GetMouseX(),
                .y = (float) (screen_height - GetMouseY())
            };
            data.push_back(point);
            lr.add_point(point);
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            for (auto [x, y] : data) {
                DrawCircle(x, screen_height - y, 3.0f, RED);
            }

            lr.draw_final(GRAY);
            lr.descent_step(data);
            lr.draw_current(BLACK);

        EndDrawing();
    }

    return 0;
}
