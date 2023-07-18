#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <raylib.h>
#include "functions.hpp"
#include "regressions.hpp"
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_CUSTOM_ICONS
#include "ricons.h"
#include "raygui.h"

#define xstr(s) #s
#define str(s) xstr(s)
#define MAX_ITERATIONS_PER_FRAME 1000

const int screen_width = 800;
const int screen_height = 800;
const int interface_width = 250;
const int interface_height = screen_height;

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
    for (int i = 0; i < (int) iterations_per_frame; ++i) r.descent_step(data);\
    r.descent.plot(BLACK);\
    r.draw_description(30, 30, 30, GRAY);\
    float error = r.descent.current_error(data);\
    DrawText(TextFormat("Error: %.02f", error), 30, 90, 30, GRAY);\
    for (auto [x, y] : data) {\
        DrawCircle(x, screen_height - y, 3.0f, RED);\
        draw_dash_dotted_line(x, std::clamp(screen_height - y, 0.0f, (float) screen_height), x, \
                std::clamp(screen_height - r.descent.evaluate_at(x), 0.0f, (float) screen_height), 4, BLUE);\
    } \
}

enum REGRESSION_TYPE { LINEAR, QUADRATIC, POWER, EXPONENTIAL };

int main() {
    InitWindow(screen_width + interface_width, screen_height, "Regressions");
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0xf5f5f5ff); 
    SetTargetFPS(60);

    std::vector<Vector2> data;
    REGRESSION_TYPE current_regression = EXPONENTIAL;
    float iterations_per_frame = 1;
    LinearRegression lr;
    QuadraticRegression qr;
    PowerRegression pr;
    ExponentialRegression er;

    Regression* regressions[] { &lr, &qr, &pr, &er };

    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseX() < screen_width) {
            Vector2 point = {
                .x = (float) GetMouseX(),
                .y = (float) (screen_height - GetMouseY())
            };
            data.push_back(point);
            for (auto regression: regressions) {
                regression->add_point(point);
            }
        }

        if (IsKeyPressed(KEY_ONE)) current_regression = LINEAR;
        if (IsKeyPressed(KEY_TWO)) current_regression = QUADRATIC;
        if (IsKeyPressed(KEY_THREE)) current_regression = POWER;
        if (IsKeyPressed(KEY_FOUR)) current_regression = EXPONENTIAL;

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
                    break;
                case EXPONENTIAL:
                    {
                        display(er);
                    }
                    break;

            }

            // GUI
            DrawRectangle(screen_width, 0, interface_width, interface_height, LIGHTGRAY);
            const Rectangle speed_slider {screen_width + interface_width / 10, 60, interface_width * 8 / 10, 40};
            DrawText("Speed", screen_width + interface_width / 10, 40, 20, GRAY);
            GuiSliderBar(speed_slider, "1", str(MAX_ITERATIONS_PER_FRAME), &iterations_per_frame, 1.0f, MAX_ITERATIONS_PER_FRAME);
            const Rectangle restart_button {screen_width + interface_width / 5, interface_height - 100, interface_width * 3 / 5, 80};
            if (GuiButton(restart_button, "Restart")) {
                data.clear();
                for (auto regression: regressions) {
                    regression->reset();
                }
            }
            const Rectangle lp_button {screen_width + interface_width / 4, interface_height * 1 / 4, interface_width / 4, interface_width / 4};
            if (GuiButton(lp_button, "#219#")) current_regression = LINEAR;
            const Rectangle qp_button {screen_width + interface_width * 2 / 4, interface_height * 1 / 4, interface_width / 4, interface_width / 4};
            if (GuiButton(qp_button, "#220#")) current_regression = QUADRATIC;
            const Rectangle pp_button {screen_width + interface_width / 4, interface_height * 1 / 4 + interface_width / 4, interface_width / 4, interface_width / 4};
            if (GuiButton(pp_button, "#221#")) current_regression = POWER;
            const Rectangle ep_button {screen_width + interface_width * 2 / 4, interface_height * 1 / 4 + interface_width / 4, interface_width / 4, interface_width / 4};
            if (GuiButton(ep_button, "#222#")) current_regression = EXPONENTIAL;

        EndDrawing();
    }

    return 0;
}
