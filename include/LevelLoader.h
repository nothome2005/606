#pragma once
#include <vector>
#include <string>
#include <filesystem>

class LevelLoader {
public:
    LevelLoader() { Reload(); }
    void Reload() {
        userLevels.clear();
        std::filesystem::create_directory("maps");
        for (const auto& entry : std::filesystem::directory_iterator("maps")) {
            if (entry.is_regular_file() && entry.path().extension() == ".lvl") {
                userLevels.push_back(entry.path().stem().string());
            }
        }
        levelScroll = 0;
        selectedLevel = -1;
        lastLoadedLevel = -1;
    }

    // --- Getters/Setters ---
    const std::vector<std::string>& GetUserLevels() const { return userLevels; }
    int GetScroll() const { return levelScroll; }
    void SetScroll(int s) { levelScroll = s; }
    int GetSelectedLevel() const { return selectedLevel; }
    void SetSelectedLevel(int idx) { selectedLevel = idx; }
    int GetLastLoadedLevel() const { return lastLoadedLevel; }
    void SetLastLoadedLevel(int idx) { lastLoadedLevel = idx; }

private:
    std::vector<std::string> userLevels;
    int levelScroll = 0;
    int selectedLevel = -1;
    int lastLoadedLevel = -1;
};
