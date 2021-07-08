#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <pthread.h>
#include <SDL2/SDL.h>
#include "src/util.h"
#include "src/texture.h"

#define WINDOW_SIZE 600

typedef struct GameData
{
    Enemy *enemies;
    Player player;
    Meteor *meteor;

    size_t meteor_length;
    size_t enemies_length;
} GameData;
void update(float, UpdateInfo);
void draw(SDL_Renderer *);
void init_game(GameData *data, SDL_Renderer *render);

int main(int argc, char **argv)
{

    if (SDL_Init(SDL_INIT_VIDEO))
    {
        return 1;
    }

    SDL_Window *window;
    SDL_Renderer *renderer;

    if (SDL_CreateWindowAndRenderer(WINDOW_SIZE, WINDOW_SIZE, 0, &window, &renderer) != 0)
    {
        return 1;
    }

    GameData data;
    init_game(&data, renderer);

    char *enemiesPath[4] = {};
    size_t len = 4;
    size_t alloc_size = sizeof(SDL_Texture *) * len;

    SDL_Texture **enemiesTextures = (SDL_Texture **)malloc(alloc_size);
    memcpy(enemiesTextures, load_texture(renderer, enemiesPath, len), alloc_size);

    char *uiPaths[10] = {"./out/assets/ui/ui-0.bmp",
                         "./out/assets/ui/ui-1.bmp",
                         "./out/assets/ui/ui-2.bmp",
                         "./out/assets/ui/ui-3.bmp",
                         "./out/assets/ui/ui-4.bmp",
                         "./out/assets/ui/ui-5.bmp",
                         "./out/assets/ui/ui-6.bmp",
                         "./out/assets/ui/ui-7.bmp",
                         "./out/assets/ui/ui-8.bmp"
                         "./out/assets/ui/ui-9.bmp"};
    len = 10;
    alloc_size = sizeof(SDL_Texture *) * len;
    SDL_Texture **uiTextures = (SDL_Texture **)malloc(alloc_size);
    memcpy(uiTextures, load_texture(renderer, enemiesPath, len), alloc_size);

    /*
    pthread_t update_thread;
    pthread_create(&update_thread, NULL, (void *)update, (void *)current_data);
    pthread_detach(update_thread);
    */

    Uint32 current_time;
    Uint32 last_time = SDL_GetTicks();
    float deltatime = 0.0f;

    time_t counter_start = time(0);
    time_t counter_current = counter_start;
    int frames = 0;

    bool running = true;
    while (running)
    {
        counter_current = time(0);
        frames++;
        if ((counter_current - counter_start) >= 1)
        {
            //printf("\rFPS: %d", frames);
            //fflush(stdout);
            frames = 0;
            counter_start = counter_current;
        }

        current_time = SDL_GetTicks();
        deltatime = current_time - last_time;
        deltatime /= 100;

        UpdateInfo updateInfo;
        updateInfo.mouse = NONE;
        updateInfo.key = SDLK_UNKNOWN;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                updateInfo.key = event.key.keysym.sym;
                break;
            case SDL_MOUSEMOTION:
                updateInfo.mouse = MOVE;

                break;
            case SDL_MOUSEBUTTONDOWN:
                updateInfo.mouse = RIGHT;
                break;
            case SDL_MOUSEWHEEL:
                if (event.button.y > 0)
                {
                    updateInfo.mouse = WHELL_UP;
                }
                else
                {
                    updateInfo.mouse = WHELL_DOWN;
                }
                break;
            }
            if (updateInfo.mouse != WHELL_UP && updateInfo.mouse != WHELL_DOWN && updateInfo.mouse != NONE)
            {
                updateInfo.mouse_position.x = event.button.x;
                updateInfo.mouse_position.y = event.button.y;
            }
        }

        update(deltatime, updateInfo);
        SDL_RenderClear(renderer);
        //draw(renderer);
        SDL_RenderPresent(renderer);

        /*CPU fix*/
        SDL_Delay(1);
        last_time = current_time;
    }

    free(enemiesTextures);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void draw(SDL_Renderer *renderer)
{
}

void update(float deltatime, UpdateInfo info)
{
}

int int_length(int a)
{
    int length = 0;
    while (a > 0)
    {
        a /= 10;
        length++;
    }
    return length;
}

void draw_number(SDL_Texture **ui, SDL_Renderer *render, int n, Vector2 position, Size size)
{
    int length = int_length(n);
    char *n_str = malloc(sizeof(char) * length);
    sprintf(n_str, "%d", n);
    for (size_t i = 0; i < length; i++)
    {
        draw_from_texture(render, ui[atoi(n_str[i])], (Vector2){position.x + (i * size.width), position.y}, size);
    }
}

void init_game(GameData *data, SDL_Renderer *render)
{
    Player p;
    SDL_Surface *image = SDL_LoadBMP("./out/assets/player/player-0.bmp");
    if (image)
    {
        p.texture = SDL_CreateTextureFromSurface(render, image);
        SDL_FreeSurface(image);
    }
    p.size.width = 100;
    p.size.height = 100;
    p.speed = 2;
    p.life = 100;
    p.bullet = malloc(sizeof(Laser) * p.bullet_len);
    p.position.x = WINDOW_SIZE / 2 - p.size.height;
    p.position.y = WINDOW_SIZE - p.size.height;

    data->player = p;
    data->enemies = malloc(sizeof(Enemy) * data->enemies_length);
    data->meteor = malloc(sizeof(Meteor) * data->meteor_length);
}