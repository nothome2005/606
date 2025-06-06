#pragma once
#include "raylib.h"
#include <functional>
#include <string>

class Button {
public:
    Button(float x, float y, float w, float h, const std::string& text = "", int fontSize = 38);

    void Draw() const;
    void Update(); // вызывает onClick при клике
    void SetOnClick(std::function<void()> cb) { onClick = std::move(cb); }
    bool IsHovered() const;
    bool IsClicked() const;

    Rectangle rect;
    std::string text;
    int fontSize;

private:
    std::function<void()> onClick;
};