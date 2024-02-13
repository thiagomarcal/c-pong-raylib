#include "raylib.h"
#include "game.h"

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
