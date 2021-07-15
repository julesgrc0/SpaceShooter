#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>

#include <pthread.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "src/util.h"
#include "src/texture.h"

#define BACKGROUND_INTERVAL 1000
#define SCORE_VALUE 10
#define FPS_MAX 1000

typedef struct GameData
{
    Enemy *enemies;
    Player player;
    Meteor *meteor;

    size_t meteor_length;
    double meteor_time;

    size_t enemies_length;
    double enemies_time;

    double background_time;
    int background_index;
} GameData;

typedef struct GameTextures
{
    SDL_Texture **ui;
    SDL_Texture **enemy;
    SDL_Texture **player;
    SDL_Texture **meteor;
    SDL_Texture **laser;
    SDL_Texture **background;
} GameTextures;

struct GlobalGameData
{
    GameData data;
    GameTextures textures;
    double deltatime;
    long double total_time;
    bool running;
    int score;
};

static struct GlobalGameData global_data;

void update_detachthread(void *);
void update_mainthread(SDL_Keycode key);
void draw(SDL_Renderer *, GameData, GameTextures);
void init_game(GameData *data, GameTextures textures, SDL_Renderer *render);
void init_textures(GameTextures *, SDL_Renderer *render);

int main(int argc, char **argv)
{
    if (argc > 1 && atoi(argv[1]) >= 500)
    {
        WINDOW_SIZE = atoi(argv[1]);
    }

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

    /*
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
    SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
    SDL_SetHint(SDL_HINT_WINDOWS_NO_CLOSE_ON_ALT_F4, "0");
    SDL_SetHint(SDL_HINT_BMP_SAVE_LEGACY_FORMAT, "1");

    SDL_SetHint(SDL_HINT_RENDER_LOGICAL_SIZE_MODE, "letterbox");
    SDL_SetHint(SDL_HINT_RENDER_DIRECT3D_THREADSAFE, "0");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    */

    /* DEBUG
    SDL_SetHint(SDL_HINT_RENDER_DIRECT3D11_DEBUG, "1");
    SDL_SetHint(SDL_HINT_EVENT_LOGGING, "2");
    */

    global_data.score = 0;
    init_textures(&global_data.textures, renderer);
    init_game(&global_data.data, global_data.textures, renderer);

    /*
    pthread_t update_thread;
    pthread_create(&update_thread, NULL, (void *)update_detachthread, (void *)NULL);
    pthread_detach(update_thread);
    */
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
    SDL_Keycode current_key;
    bool is_press = false;
    while (global_data.running)
    {
        counter_current = time(0);
        frames++;

        current_time = SDL_GetTicks();
        deltatime = current_time - last_time;
        deltatime /= 100;

        global_data.total_time += deltatime;

        /* global_data.data.background_time += deltatime * 10;
        if (global_data.data.background_time > BACKGROUND_INTERVAL)
        {
            global_data.data.background_time = 0;
            global_data.data.background_index++;

            if (global_data.data.background_index > 3)
            {
                global_data.data.background_index = 0;
            }
        }
        */

        if (global_data.data.player.life <= 100)
        {
            global_data.data.background_index = 3;
        }
        if (global_data.data.player.life <= 50)
        {
            global_data.data.background_index = 2;
        }
        if (global_data.data.player.life <= 25)
        {
            global_data.data.background_index = 1;
        }
        if (global_data.data.player.life <= 10)
        {
            global_data.data.background_index = 0;
        }

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
            if (event.type == SDL_KEYDOWN)
            {
                current_key = event.key.keysym.sym;
                is_press = true;
            }
            if (event.type == SDL_KEYUP)
            {
                is_press = false;
                current_key = SDLK_UNKNOWN;
            }
        }
        update_mainthread(current_key);
        SDL_RenderClear(renderer);
        draw(renderer, global_data.data, global_data.textures);
        SDL_RenderPresent(renderer);

        /*CPU fix*/
        SDL_Delay(1000 / FPS_MAX);
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
    draw_size_texture(render, textures.background[global_data.data.background_index], (Vector2){0, 0}, (Size){WINDOW_SIZE, WINDOW_SIZE});
    draw_from_texture(render, data.player.texture, data.player.position, data.player.size);
    draw_number(textures.ui, render, data.player.life, (Vector2){0, WINDOW_SIZE - 20}, (Size){25, 50});
    draw_number(textures.ui, render, global_data.score, (Vector2){0, 0}, (Size){30, 50});

    for (size_t i = 0; i < data.enemies_length; i++)
    {
        draw_from_texture(render, textures.enemy[data.enemies[i].type], data.enemies[i].position, data.enemies[i].size);
    }

    for (size_t i = 0; i < data.player.bullet_len; i++)
    {
        rotation_draw_texture(render, textures.laser[0], data.player.bullet[i].position, data.player.bullet[i].size, 0, SDL_FLIP_NONE);
    }

    //  Segmentation fault
    for (size_t i = 0; i < data.meteor_length; i++)
    {
        rotation_draw_texture(render, textures.meteor[data.meteor[i].type], data.meteor[i].position, data.meteor[i].size, 0, SDL_FLIP_NONE);
    }

    //  Segmentation fault
    for (size_t i = 0; i < data.enemies_length; i++)
    {

        for (size_t k = 0; k < data.enemies[i].bullet_len; k++)
        {
            Laser l = data.enemies[i].bullet[k];
            rotation_draw_texture(render, textures.laser[32], data.enemies[i].bullet[k].position, data.enemies[i].bullet[k].size, 180, SDL_FLIP_NONE);
        }
    }

    global_data.data.player.time += global_data.deltatime;
}

void update_mainthread(SDL_Keycode key)
{
    for (size_t i = 0; i < global_data.data.player.bullet_len; i++)
    {
        for (size_t k = 0; k < global_data.data.enemies_length; k++)
        {
            if (i < global_data.data.player.bullet_len)
            {
                if (AABB(global_data.data.player.bullet[i].position, (Size){LASER_W, LASER_H}, global_data.data.enemies[k].position, global_data.data.enemies[k].size))
                {
                    remove_laser(&global_data.data.player.bullet, &global_data.data.player.bullet_len, &i);

                    global_data.data.enemies[k].life -= 10;
                    if (global_data.data.enemies[k].life <= 0)
                    {
                        global_data.score += SCORE_VALUE * global_data.data.enemies[k].speed;
                        enemy_dead(&global_data.data.enemies, &global_data.data.enemies_length, &k);
                    }
                }
            }
        }
    }

    for (size_t k = 0; k < global_data.data.enemies_length; k++)
    {
        for (size_t i = 0; i < global_data.data.enemies[k].bullet_len; i++)
        {

            if (AABB(global_data.data.enemies[k].bullet[i].position, global_data.data.enemies[k].bullet[i].size, global_data.data.player.position, global_data.data.player.size))
            {
                remove_laser(&global_data.data.enemies[k].bullet, &global_data.data.enemies[k].bullet_len, &i);

                global_data.data.player.life -= 10;
                if (global_data.data.player.life <= 0)
                {
                    global_data.running = false;
                    break;
                }
            }
        }
    }

    if ((key != SDLK_LEFT && key != SDLK_RIGHT && key != SDLK_SPACE) && !global_data.data.player.move_stop)
    {
        //player_after_move(&global_data.data.player, !global_data.data.player.left, global_data.deltatime);
    }
    else
    {
        if (player_update(&global_data.data.player, global_data.deltatime, key) && global_data.data.player.time > PLAYER_SHOOT_TIME)
        {
            global_data.data.player.time = 0;

            Laser l;
            l.angle = -90;
            l.size = (Size){LASER_W, LASER_H};
            l.position = (Vector2){global_data.data.player.position.x + (global_data.data.player.size.width - l.size.width) / 2, global_data.data.player.position.y - global_data.data.player.size.height / 2};
            l.speed = LASER_SPEED;

            laser_add(&global_data.data.player.bullet, l, &global_data.data.player.bullet_len);
        }
    }

    add_meteor_time(&global_data.data.meteor, &global_data.data.meteor_length, &global_data.data.meteor_time, global_data.deltatime);
    if (global_data.data.meteor_length)
    {
        meteor_update(&global_data.data.meteor, &global_data.data.meteor_length, global_data.deltatime);
    }

    add_enemy_time(&global_data.data.enemies, &global_data.data.enemies_length, &global_data.data.enemies_time, global_data.deltatime);
    if (global_data.data.enemies_length)
    {
        enemy_update(&global_data.data.enemies, global_data.data.enemies_length, global_data.deltatime);
        for (size_t i = 0; i < global_data.data.enemies_length; i++)
        {
            global_data.data.enemies[i].waitShoot += global_data.deltatime;
            if (global_data.data.enemies[i].waitShoot >= ENEMY_SHOOT_INTERVAL)
            {
                global_data.data.enemies[i].waitShoot = 0;

                Laser l;
                l.angle = 90;
                l.type = 0;
                l.speed = LASER_SPEED;
                l.size = (Size){LASER_W, LASER_H};
                l.position = (Vector2){global_data.data.enemies[i].position.x + (global_data.data.enemies[i].size.width / 2) + l.size.width, global_data.data.enemies[i].position.y + global_data.data.enemies[i].size.height / 2};

                laser_add(&global_data.data.enemies[i].bullet, l, &global_data.data.enemies[i].bullet_len);
            }

            if (global_data.data.enemies[i].bullet_len)
            {
                laser_update(&global_data.data.enemies[i].bullet, &global_data.data.enemies[i].bullet_len, global_data.deltatime);
            }
        }
    }
    if (global_data.data.player.bullet_len)
    {
        laser_update(&global_data.data.player.bullet, &global_data.data.player.bullet_len, global_data.deltatime);
    }
}

void update_detachthread(void *n)
{
    SDL_Keycode current_key;
    bool ispress = false;
    double last_delta;

    while (global_data.running)
    {

        for (size_t k = 0; k < global_data.data.enemies_length; k++)
        {
            for (size_t i = 0; i < global_data.data.enemies[k].bullet_len; i++)
            {

                if (AABB(global_data.data.enemies[k].bullet[i].position, global_data.data.enemies[k].bullet[i].size, global_data.data.player.position, global_data.data.player.size))
                {
                    remove_laser(&global_data.data.enemies[k].bullet, &global_data.data.enemies[k].bullet_len, &i);

                    global_data.data.player.life -= 10;
                    if (global_data.data.player.life <= 0)
                    {
                        global_data.running = false;
                        break;
                    }
                }
            }
        }
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

            if (ispress)
            {
                if (player_update(&global_data.data.player, global_data.deltatime, event.key.keysym.sym) && global_data.data.player.time > PLAYER_SHOOT_TIME)
                {
                    global_data.data.player.time = 0;

                    Laser l;
                    l.angle = -90;
                    l.size = (Size){LASER_W, LASER_H};
                    l.position = (Vector2){global_data.data.player.position.x + (global_data.data.player.size.width - l.size.width) / 2, global_data.data.player.position.y - global_data.data.player.size.height / 2};
                    l.speed = LASER_SPEED;

                    laser_add(&global_data.data.player.bullet, l, &global_data.data.player.bullet_len);
                }
            }
        }

        if (!ispress && !global_data.data.player.move_stop)
        {
            player_after_move(&global_data.data.player, !global_data.data.player.left, global_data.deltatime);
        }

        //  Segmentation fault (core dumped)
        for (size_t i = 0; i < global_data.data.player.bullet_len; i++)
        {
            Laser laser = global_data.data.player.bullet[i];
            for (size_t k = 0; k < global_data.data.enemies_length; k++)
            {
                if (AABB(laser.position, laser.size, global_data.data.enemies[k].position, global_data.data.enemies[k].size))
                {
                    remove_laser(&global_data.data.player.bullet, &global_data.data.player.bullet_len, &i);

                    global_data.data.enemies[k].life -= 10;
                    if (global_data.data.enemies[k].life <= 0)
                    {
                        global_data.score += SCORE_VALUE * global_data.data.enemies[k].speed;
                        enemy_dead(&global_data.data.enemies, &global_data.data.enemies_length, &k);
                    }
                }
            }
        }
        //

        if (last_delta != global_data.deltatime)
        {
            last_delta = global_data.deltatime;

            add_meteor_time(&global_data.data.meteor, &global_data.data.meteor_length, &global_data.data.meteor_time, global_data.deltatime);
            if (global_data.data.meteor_length)
            {
                meteor_update(&global_data.data.meteor, &global_data.data.meteor_length, global_data.deltatime);
            }

            add_enemy_time(&global_data.data.enemies, &global_data.data.enemies_length, &global_data.data.enemies_time, global_data.deltatime);
            if (global_data.data.enemies_length)
            {
                enemy_update(&global_data.data.enemies, global_data.data.enemies_length, global_data.deltatime);
                for (size_t i = 0; i < global_data.data.enemies_length; i++)
                {
                    global_data.data.enemies[i].waitShoot += global_data.deltatime;
                    if (global_data.data.enemies[i].waitShoot >= ENEMY_SHOOT_INTERVAL)
                    {
                        global_data.data.enemies[i].waitShoot = 0;

                        Laser l;
                        l.angle = 90;
                        l.type = 0;
                        l.speed = LASER_SPEED;
                        l.size = (Size){LASER_W, LASER_H};
                        l.position = (Vector2){global_data.data.enemies[i].position.x + (global_data.data.enemies[i].size.width / 2) + l.size.width, global_data.data.enemies[i].position.y + global_data.data.enemies[i].size.height / 2};

                        laser_add(&global_data.data.enemies[i].bullet, l, &global_data.data.enemies[i].bullet_len);
                    }

                    if (global_data.data.enemies[i].bullet_len)
                    {
                        laser_update(&global_data.data.enemies[i].bullet, &global_data.data.enemies[i].bullet_len, global_data.deltatime);
                    }
                }
            }
        }

        if (global_data.data.player.bullet_len)
        {
            laser_update(&global_data.data.player.bullet, &global_data.data.player.bullet_len, global_data.deltatime);
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
        char str_index[1] = {n_str[i]};
        int index = atoi(str_index);
        draw_from_texture(render, ui[index], (Vector2){position.x + (i * size.width), position.y}, size);
    }
}

void init_game(GameData *data, GameTextures textures, SDL_Renderer *render)
{
    Player p;
    p.texture = textures.player[0];
    p.size.width = 100;
    p.size.height = 100;
    p.speed = PLAYER_SPEED;
    p.life = 100;
    p.bullet_len = 0;
    p.bullet = malloc(sizeof(Laser) * p.bullet_len);
    p.position.x = (WINDOW_SIZE - p.size.width) / 2;
    p.position.y = WINDOW_SIZE - p.size.height;

    data->player = p;
    data->enemies = malloc(sizeof(Enemy) * data->enemies_length);
    data->meteor = malloc(sizeof(Meteor) * data->meteor_length);

    data->enemies_time = ENEMY_TIME;
}

void init_textures(GameTextures *textures, SDL_Renderer *render)
{
    load_directory_textures("./out/assets/player", &textures->player, render);
    load_directory_textures("./out/assets/enemies", &textures->enemy, render);
    load_directory_textures("./out/assets/meteor", &textures->meteor, render);
    load_directory_textures("./out/assets/laser", &textures->laser, render);
    load_directory_textures("./out/assets/ui", &textures->ui, render);
    load_directory_textures("./out/assets/background", &textures->background, render);
}