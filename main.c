#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>

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

typedef struct GameTextures
{
    SDL_Texture **ui;
    SDL_Texture **enemy;
    SDL_Texture **player;
    SDL_Texture **meteor;
    SDL_Texture **laser;
} GameTextures;

void update(float, GameData *, GameTextures, UpdateInfo);
void draw(SDL_Renderer *, GameData, GameTextures);
void init_game(GameData *data, GameTextures textures, SDL_Renderer *render);
void init_textures(GameTextures *, SDL_Renderer *render);

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
    GameTextures textures;

    init_textures(&textures, renderer);
    init_game(&data, textures, renderer);
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

        update(deltatime, &data, textures, updateInfo);
        SDL_RenderClear(renderer);
        draw(renderer, data, textures);
        SDL_RenderPresent(renderer);

        /*CPU fix*/
        SDL_Delay(1);
        last_time = current_time;
    }

    free(textures.ui);
    free(textures.player);
    free(textures.enemy);
    free(textures.meteor);
    free(textures.laser);

    free(data.enemies);
    free(data.meteor);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void draw(SDL_Renderer *render, GameData data, GameTextures textures)
{
    draw_from_texture(render, data.player.texture, data.player.position, data.player.size);
}

void update(float deltatime, GameData *data, GameTextures textures, UpdateInfo info)
{
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

void init_game(GameData *data, GameTextures textures, SDL_Renderer *render)
{
    Player p;
    p.texture = textures.player[3];
    p.size.width = 100;
    p.size.height = 100;
    p.speed = 2.0f;
    p.life = 100;
    p.bullet = malloc(sizeof(Laser) * p.bullet_len);
    p.position.x = (WINDOW_SIZE - p.size.width) / 2;
    p.position.y = WINDOW_SIZE - p.size.height;

    data->player = p;
    data->enemies = malloc(sizeof(Enemy) * data->enemies_length);
    data->meteor = malloc(sizeof(Meteor) * data->meteor_length);
}

void init_textures(GameTextures *textures, SDL_Renderer *render)
{
    load_directory_textures("./out/assets/player", &textures->player, render);
    load_directory_textures("./out/assets/enemies", &textures->enemy, render);
    load_directory_textures("./out/assets/meteor", &textures->meteor, render);
    load_directory_textures("./out/assets/laser", &textures->laser, render);
    load_directory_textures("./out/assets/ui", &textures->ui, render);
}