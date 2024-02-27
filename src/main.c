#include "game.h"
#include "raylib.h"

int main() {

    Game game = CreateGame("Scaffold");

    InitGame(&game);

    while (IsRunning(&game)) {

        // handle events

        // Update
        UpdateGame(&game);

        // Render
        RenderGame(&game);

    }

    ShutDown(&game);

    CloseWindow();

    return 0;
}
