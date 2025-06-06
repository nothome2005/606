#pragma once
#include "raylib.h"
#include <functional>

class ScrollBar {
public:
    ScrollBar(float x, float y, float width, float height, int totalItems, int visibleItems);

    void Draw() const;
    void Update();
    
    // Set the callback function that will be called when the scroll value changes
    void SetOnScroll(std::function<void(int)> callback) { onScroll = std::move(callback); }
    
    // Set the current scroll position (0-based index)
    void SetScrollPosition(int position);
    
    // Get the current scroll position
    int GetScrollPosition() const { return scrollPosition; }
    
    // Update the total number of items
    void SetTotalItems(int count);
    
    // Set the number of visible items
    void SetVisibleItems(int count);

private:
    Rectangle track; // The track/background of the scrollbar
    Rectangle thumb; // The draggable part of the scrollbar
    
    int totalItems;   // Total number of items to scroll through
    int visibleItems; // Number of items visible at once
    int scrollPosition = 0; // Current scroll position (0-based index)
    
    bool isDragging = false;
    float dragOffset = 0;
    
    std::function<void(int)> onScroll;
    
    void UpdateThumbPosition();
};