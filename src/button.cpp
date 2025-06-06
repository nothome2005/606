#include "button.h"
#include <string>

Button::Button(float x, float y, float w, float h, const std::string& text, int fontSize)
    : rect{ x, y, w, h }, text(text), fontSize(fontSize) {}

void Button::Draw() const {
    bool hovered = IsHovered();
    DrawRectangleRec(rect, hovered ? Fade(WHITE, 0.25f) : Fade(WHITE, 0.12f));
    DrawRectangleLinesEx(rect, 2, hovered ? WHITE : GRAY);
    int textW = MeasureText(text.c_str(), fontSize);
    DrawText(text.c_str(), rect.x + (rect.width - textW) / 2, rect.y + (rect.height - fontSize) / 2, fontSize, WHITE);
}

bool Button::IsHovered() const {
    return CheckCollisionPointRec(GetMousePosition(), rect);
}

bool Button::IsClicked() const {
    return IsHovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void Button::Update() {
    if (IsClicked() && onClick) onClick();
}