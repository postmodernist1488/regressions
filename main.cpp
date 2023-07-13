#include <raylib.h>
#include <vector>
#include <cmath>
#include <iostream>
#include "functions.hpp"
#include "regressions.hpp"

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

#define display(r) {\
    r.calculated.plot(GRAY); \
    r.descent_step(data);\
    r.descent.plot(BLACK);\
    r.draw_description(30, 30, 30, GRAY);\
    float error = r.descent.current_error(data);\
    DrawText(TextFormat("Error: %.02f", error), 30, 90, 30, GRAY);\
    for (auto [x, y] : data) {\
        DrawCircle(x, screen_height - y, 3.0f, RED);\
        draw_dash_dotted_line(x, screen_height - y, x, screen_height - r.descent.evaluate_at(x), 4, BLUE);\
    } \
}

enum REGRESSION_TYPE { LINEAR, QUADRATIC, POWER };

int main() {
    InitWindow(screen_width, screen_height, "Regressions");
    SetTargetFPS(60);

    std::vector<Vector2> data;
    REGRESSION_TYPE current_regression = POWER;
    LinearRegression lr;
    QuadraticRegression qr;
    PowerRegression pr;

    Regression* regressions[] { &lr, &qr, &pr };

    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 point = {
                .x = (float) GetMouseX(),
                .y = (float) (screen_height - GetMouseY())
            };
            data.push_back(point);
            for (auto regression: regressions) {
                regression->add_point(point);
            }
        }

        if (IsKeyPressed(KEY_R)) {
            data.clear();
            for (auto regression: regressions) {
                regression->reset();
            }
        }

        if (IsKeyPressed(KEY_F)) {
            SetTargetFPS(0);
        }
        if (IsKeyReleased(KEY_F)) {
            SetTargetFPS(60);
        }

        if (IsKeyPressed(KEY_ONE)) current_regression = LINEAR;
        if (IsKeyPressed(KEY_TWO)) current_regression = QUADRATIC;
        if (IsKeyPressed(KEY_THREE)) current_regression = POWER;

        BeginDrawing();
            ClearBackground(RAYWHITE);


            switch(current_regression) {
                case LINEAR: 
                    {
                        display(lr);
                    }
                    break;
                case QUADRATIC: 
                    {   
                        display(qr);
                    }
                    break;
                case POWER:
                    {
                        display(pr);
                    }
            }

        EndDrawing();
    }

    return 0;
}
