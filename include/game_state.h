#pragma once
#include <memory>
#include "grid.h"
#include "player.h"
#include "wolf.h"
#include <vector>

class GameState {
public:
    virtual ~GameState() {}
    virtual void Enter() {}
    virtual void Exit() {}
    virtual void Update(float dt) = 0;
};

class WolfTurnState; // forward

class PlayerTurnState : public GameState {
public:
    PlayerTurnState(Player& player, std::vector<Wolf>& wolves, Grid& grid, GameState*& statePtr)
        : player(player), wolves(wolves), grid(grid), statePtr(statePtr), wolfState(nullptr) {}
    
    void SetWolfState(GameState* ws) { wolfState = ws; }
    Player& GetPlayer() { return player; }
    std::vector<Wolf>& GetWolves() { return wolves; }
    
    void Update(float dt) override {
        if (!player.HasMoved()) {
            player.Update(dt);
            if (player.HasMoved()) {
                for (auto& wolf : wolves) {
                    wolf.StartTurn(grid, player);
                }
                statePtr = wolfState;
            }
        }
    }
private:
    Player& player;
    std::vector<Wolf>& wolves;
    Grid& grid;
    GameState*& statePtr;
    GameState* wolfState;
};

class WolfTurnState : public GameState {
public:
    WolfTurnState(Player& player, std::vector<Wolf>& wolves, Grid& grid, GameState*& statePtr)
        : player(player), wolves(wolves), grid(grid), statePtr(statePtr), playerState(nullptr), firstRound(true) {
    }

    void SetPlayerState(GameState* ps) { playerState = ps; }
    Player& GetPlayer() { return player; }
    std::vector<Wolf>& GetWolves() { return wolves; }

    void Update(float dt) override {
        for (auto& wolf : wolves) {
            wolf.Update(dt);
        }

        bool allFinished = true;
        for (const auto& wolf : wolves) {
            if (!wolf.IsTurnFinished()) {
                allFinished = false;
                break;
            }
        }

        if (allFinished) {
            player.ResetTurn();
            if (firstRound) {
                firstRound = false;
            }
            statePtr = playerState;
        }
    }
private:
    Player& player;
    std::vector<Wolf>& wolves;
    Grid& grid;
    GameState*& statePtr;
    GameState* playerState;
    bool firstRound;
};
