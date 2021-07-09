#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>

#include <pthread.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "src/util.h"
#include "src/texture.h"

#define FPS_MAX 200

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

struct GlobalGameData
{
    GameData data;
    GameTextures textures;
    double deltatime;
    bool running;
};

static struct GlobalGameData global_data;

void update(void *);
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

    init_textures(&global_data.textures, renderer);
    init_game(&global_data.data, global_data.textures, renderer);

    pthread_t update_thread;
    pthread_create(&update_thread, NULL, (void *)update, (void *)NULL);
    pthread_detach(update_thread);

    Uint32 current_time;
    Uint32 last_time = SDL_GetTicks();
    double deltatime = 0.0f;

    /*
    clock_t current_time;
    clock_t last_time = clock();
    double deltatime = 0.0f;
    */

    time_t counter_start = time(0);
    time_t counter_current = counter_start;
    int frames = 0;

    global_data.running = true;
    SDL_Event event;
    while (global_data.running)
    {
        counter_current = time(0);
        frames++;

        current_time = SDL_GetTicks();
        deltatime = current_time - last_time;
        deltatime /= 100;

        /*
        current_time = clock();
        deltatime = current_time - last_time;
        deltatime /= 10000;
        */

        global_data.deltatime = deltatime;

        if ((counter_current - counter_start) >= 1)
        {
            //printf("\rFPS: %d Deltatime: %f", frames, deltatime);
            //fflush(stdout);
            char *title = malloc(sizeof(char) * int_length(frames));
            sprintf(title, "Space Shooter - %d FPS", frames);
            SDL_SetWindowTitle(window, title);
            free(title);

            frames = 0;
            counter_start = counter_current;
        }
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                global_data.running = false;
            }
        }

        SDL_RenderClear(renderer);
        draw(renderer, global_data.data, global_data.textures);
        SDL_RenderPresent(renderer);

        /*CPU fix*/
        SDL_Delay(1000/FPS_MAX);
        /**/

        last_time = current_time;
    }

    free(global_data.textures.ui);
    free(global_data.textures.player);
    free(global_data.textures.enemy);
    free(global_data.textures.meteor);
    free(global_data.textures.laser);

    free(global_data.data.enemies);
    free(global_data.data.meteor);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void draw(SDL_Renderer *render, GameData data, GameTextures textures)
{
    draw_from_texture(render, data.player.texture, data.player.position, data.player.size);
}

void update(void *n)
{
    SDL_Keycode current_key;
    bool ispress = false;
    while (global_data.running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                global_data.running = false;
            }
            if (ispress)
            {
                if (event.type == SDL_KEYUP && current_key == event.key.keysym.sym)
                {
                    ispress = false;
                    current_key = SDLK_UNKNOWN;
                }
            }
            else
            {
                if (event.type == SDL_KEYDOWN && current_key != event.key.keysym.sym)
                {
                    current_key = event.key.keysym.sym;
                    ispress = true;
                }
            }

            if (ispress && current_key != SDLK_UNKNOWN)
            {
                player_update(&global_data.data.player, global_data.deltatime, event.key.keysym.sym);
            }
        }
    }

    pthread_exit(0);
}

void draw_number(SDL_Texture **ui, SDL_Renderer *render, int n, Vector2 position, Size size)
{
    int length = int_length(n);
    char *n_str = malloc(sizeof(char) * length);
    sprintf(n_str, "%d", n);
    for (size_t i = 0; i < length; i++)
    {
        char *str_index = malloc(sizeof(char));
        str_index[0] = n_str[i];
        int index = atoi(str_index);
        draw_from_texture(render, ui[index - 1], (Vector2){position.x + (i * size.width), position.y}, size);
    }
}

void init_game(GameData *data, GameTextures textures, SDL_Renderer *render)
{
    Player p;
    p.texture = textures.player[3];
    p.size.width = 100;
    p.size.height = 100;
    p.speed = 300.0f;
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