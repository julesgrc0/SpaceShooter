#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include "src/texture.h"


typedef struct Player
{
    double x, y;
} Player;

struct GameData
{
    Player main_player;
};

static struct GameData *current_data;
void update(void *data);

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(500, 500, 0, &window, &renderer);

    pthread_t update_thread;
    pthread_create(&update_thread, NULL, (void *)update, (void *)current_data);
    pthread_detach(update_thread);


    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            }
        }


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);


        SDL_RenderPresent(renderer);
    }

    pthread_join(update_thread, NULL);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


void update(void *data)
{

    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            double delta = (double)(SDL_GetTicks() / 1000);

            switch (event.type)
            {
            case SDL_KEYDOWN:

                if (event.key.keysym.sym == SDLK_LEFT)
                {
                    current_data->main_player.x -= 5 * delta;
                }
                else if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    current_data->main_player.x += 5 * delta;
                }
                printf("%f %f\n", delta, current_data->main_player.x);
                break;
            }
        }

    }
    pthread_exit(NULL);
}
