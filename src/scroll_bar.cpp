#include "scroll_bar.h"

ScrollBar::ScrollBar(float x, float y, float width, float height, int totalItems, int visibleItems)
    : track{x, y, width, height}, totalItems(totalItems), visibleItems(visibleItems)
{
    // Initialize the thumb size and position based on the current values
    UpdateThumbPosition();
}

void ScrollBar::Draw() const {
    // Draw track
    DrawRectangleRec(track, Fade(GRAY, 0.2f));
    
    // Draw thumb if there are enough items to scroll
    if (totalItems > visibleItems) {
        DrawRectangleRec(thumb, Fade(WHITE, isDragging ? 0.7f : 0.5f));
        DrawRectangleLinesEx(thumb, 1, WHITE);
    }
}

void ScrollBar::Update() {
    // Don't process if there's nothing to scroll
    if (totalItems <= visibleItems) return;
    
    Vector2 mousePos = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePos, thumb);
    
    // Handle dragging
    if (isDragging) {
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            isDragging = false;
        } else {
            // Calculate the new position based on mouse movement
            float thumbY = mousePos.y - dragOffset;
            
            // Constrain the thumb position within the track
            if (thumbY < track.y) thumbY = track.y;
            if (thumbY + thumb.height > track.y + track.height) {
                thumbY = track.y + track.height - thumb.height;
            }
            
            // Update the thumb position
            thumb.y = thumbY;
            
            // Calculate the new scroll position based on the thumb position
            float scrollRatio = (thumb.y - track.y) / (track.height - thumb.height);
            int maxScroll = totalItems - visibleItems;
            int newScrollPos = static_cast<int>(scrollRatio * maxScroll + 0.5f);
            
            // Update scroll position if it changed
            if (newScrollPos != scrollPosition) {
                scrollPosition = newScrollPos;
                if (onScroll) onScroll(scrollPosition);
            }
        }
    } else if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isDragging = true;
        dragOffset = mousePos.y - thumb.y;
    } else if (CheckCollisionPointRec(mousePos, track) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        // Click on track - jump thumb to that position
        if (mousePos.y < thumb.y) {
            // Clicked above thumb - scroll up
            SetScrollPosition(scrollPosition - visibleItems);
        } else if (mousePos.y > thumb.y + thumb.height) {
            // Clicked below thumb - scroll down
            SetScrollPosition(scrollPosition + visibleItems);
        }
    }
    
    // Handle scroll wheel
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        SetScrollPosition(scrollPosition - (int)wheel);
    }
}

void ScrollBar::SetScrollPosition(int position) {
    // Constrain the scroll position within valid range
    int maxScroll = (totalItems > visibleItems) ? (totalItems - visibleItems) : 0;
    
    if (position < 0) position = 0;
    if (position > maxScroll) position = maxScroll;
    
    // Update position if it changed
    if (position != scrollPosition) {
        scrollPosition = position;
        UpdateThumbPosition();
        if (onScroll) onScroll(scrollPosition);
    }
}

void ScrollBar::SetTotalItems(int count) {
    if (count != totalItems) {
        totalItems = count;
        // Ensure scroll position is still valid
        SetScrollPosition(scrollPosition);
        UpdateThumbPosition();
    }
}

void ScrollBar::SetVisibleItems(int count) {
    if (count != visibleItems) {
        visibleItems = count;
        // Ensure scroll position is still valid
        SetScrollPosition(scrollPosition);
        UpdateThumbPosition();
    }
}

void ScrollBar::UpdateThumbPosition() {
    // Calculate thumb height based on the visible/total ratio
    float visibleRatio = totalItems > 0 ? (float)visibleItems / totalItems : 1.0f;
    // Ensure minimum thumb size for easy grabbing
    float thumbHeight = track.height * visibleRatio;
    if (thumbHeight < 30) thumbHeight = 30;
    if (thumbHeight > track.height) thumbHeight = track.height;
    
    // Calculate thumb position based on current scroll position
    float maxScroll = totalItems - visibleItems;
    float scrollRatio = maxScroll > 0 ? (float)scrollPosition / maxScroll : 0;
    float thumbY = track.y + scrollRatio * (track.height - thumbHeight);
    
    // Update thumb rectangle
    thumb = { track.x, thumbY, track.width, thumbHeight };
}