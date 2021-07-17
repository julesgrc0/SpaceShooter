#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>

#include <pthread.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
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

    double boost_time;
} GameData;

typedef struct GameAudio
{
    Mix_Music *player_laser;
    Mix_Music *enemies_laser;
    Mix_Music *player_shield;
    Mix_Music *player_shield_stop;
    Mix_Music *lose;
    Mix_Music *item_p;
    Mix_Music *item_m;
} GameAudio;

typedef struct GameTextures
{
    SDL_Texture **ui;
    SDL_Texture **enemy;
    SDL_Texture **player;
    SDL_Texture **meteor;
    SDL_Texture **laser;
    SDL_Texture **background;
    SDL_Texture **bonus;
    SDL_Texture **effect;
} GameTextures;

struct GlobalGameData
{
    GameAudio audio;
    GameData data;
    GameTextures textures;

    TTF_Font *font40;
    TTF_Font *font12;

    double deltatime;
    time_t total_time;
    time_t stop_time;
    bool running;

    bool shiel_song;
    bool shiel_song_stop;
    bool song_item_p;
    bool song_item_m;
    bool audio_enable;

    bool menu;
    int menu_state;
    int menu_index;
    double menu_press;
    double menu_start_time;
    int menu_start_index;

    int score;
    int color_index;
    int kill;
};

static struct GlobalGameData global_data;

//void update_detachthread(void *);
void update_mainthread(SDL_Keycode key);
void draw(SDL_Renderer *, GameData, GameTextures);

void menu_mode(SDL_Renderer *renderer, SDL_Keycode key);

void init_game(GameData *data, GameTextures textures, SDL_Renderer *render);
void init_textures(GameTextures *, SDL_Renderer *render);
void init_audio(GameAudio *);

bool update_boost(bool *is, Vector2 *pos, double *time);
void song_boost(bool *p, bool *l, bool *im, bool *ip, Mix_Music *sm, Mix_Music *sp);
void draw_text(const char *text, SDL_Renderer *renderer, TTF_Font *font, Vector2 position, Size size, bool centerX, bool centerY);

int main(int argc, char **argv)
{
    if (argc > 1 && atoi(argv[1]) >= 500)
    {
        WINDOW_SIZE = atoi(argv[1]);
    }

    if (argc > 2 && atoi(argv[2]) > 0 && atoi(argv[2]) < 5)
    {
        PLAYER_DEFAULT = atoi(argv[2]) - 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        return 1;
    }

    TTF_Init();

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
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

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    if (!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
    {
        return 1;
    }
#endif
    /* DEBUG
    SDL_SetHint(SDL_HINT_RENDER_DIRECT3D11_DEBUG, "1");
    SDL_SetHint(SDL_HINT_EVENT_LOGGING, "2");
    */

    global_data.menu_start_index = 4;
    global_data.menu_start_time = 0;
    global_data.score = 0;
    global_data.audio_enable = true;
    global_data.menu = true;

    init_textures(&global_data.textures, renderer);
    init_audio(&global_data.audio);
    init_game(&global_data.data, global_data.textures, renderer);

    global_data.font12 = TTF_OpenFont("./out/assets/font.ttf", 28);
    global_data.font40 = TTF_OpenFont("./out/assets/font.ttf", 60);

    SDL_SetWindowIcon(window, SDL_LoadBMP("./out/assets/bonus/bonus-96.bmp"));

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
            /*
            printf("\rFPS: %d Deltatime: %f Time: %d", frames, deltatime, time_interval(global_data.total_time));
            fflush(stdout);
            */

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
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    global_data.color_index++;
                    if (global_data.color_index >= 4)
                    {
                        global_data.color_index = 0;
                    }
                }
                else if (event.button.button == SDL_BUTTON_LEFT)
                {
                    global_data.audio_enable = !global_data.audio_enable;
                }
            }
        }

        if (global_data.menu)
        {
            SDL_RenderClear(renderer);
            menu_mode(renderer, current_key);
            SDL_RenderPresent(renderer);
        }
        else
        {
            if (global_data.total_time == 0)
            {
                global_data.total_time = time(0);
            }

            update_mainthread(current_key);
            SDL_RenderClear(renderer);
            draw(renderer, global_data.data, global_data.textures);
            SDL_RenderPresent(renderer);
        }

        /*CPU fix*/
        SDL_Delay(1000 / FPS_MAX);
        /**/

        last_time = current_time;
    }

    Mix_FreeMusic(global_data.audio.player_laser);
    Mix_FreeMusic(global_data.audio.enemies_laser);
    Mix_FreeMusic(global_data.audio.player_shield);

    free(global_data.textures.bonus);
    free(global_data.textures.ui);
    free(global_data.textures.player);
    free(global_data.textures.enemy);
    free(global_data.textures.meteor);
    free(global_data.textures.laser);

    free(global_data.data.enemies);
    free(global_data.data.meteor);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_CloseAudio();
    SDL_Quit();
    return 0;
}

void draw(SDL_Renderer *render, GameData data, GameTextures textures)
{
    draw_size_texture(render, textures.background[global_data.data.background_index], (Vector2){0, 0}, (Size){WINDOW_SIZE, WINDOW_SIZE});
    draw_from_texture(render, textures.player[data.player.texture + global_data.color_index], data.player.position, data.player.size);

    for (size_t i = 0; i < data.enemies_length; i++)
    {

        draw_from_texture(render, textures.enemy[data.enemies[i].type + (global_data.color_index * 5)], data.enemies[i].position, data.enemies[i].size);
    }

    int laser_texture_index = 0;
    if (global_data.data.player.boost_fire)
    {
        laser_texture_index = 26;
    }
    else if (global_data.data.player.boost_damage)
    {
        laser_texture_index = 7;
    }

    for (size_t i = 0; i < data.player.bullet_len; i++)
    {
        rotation_draw_texture(render, textures.laser[laser_texture_index], data.player.bullet[i].position, data.player.bullet[i].size, 0, SDL_FLIP_NONE);
    }

    int meteor_textures_add = 0;
    if (global_data.data.player.life <= 50)
    {
        meteor_textures_add = 10;
    }
    for (size_t i = 0; i < data.meteor_length; i++)
    {
        rotation_draw_texture(render, textures.meteor[data.meteor[i].type + meteor_textures_add], data.meteor[i].position, data.meteor[i].size, 0, SDL_FLIP_NONE);
    }

    for (size_t i = 0; i < data.enemies_length; i++)
    {

        for (size_t k = 0; k < data.enemies[i].bullet_len; k++)
        {
            Laser l = data.enemies[i].bullet[k];
            rotation_draw_texture(render, textures.laser[32], data.enemies[i].bullet[k].position, data.enemies[i].bullet[k].size, 180, SDL_FLIP_NONE);
        }
    }

    draw_from_texture(render, textures.bonus[1], data.player.fire_pos, (Size){BOOST_SIZE, BOOST_SIZE});
    draw_from_texture(render, textures.bonus[0], data.player.resistence_pos, (Size){BOOST_SIZE, BOOST_SIZE});
    draw_from_texture(render, textures.bonus[14], data.player.damage_pos, (Size){BOOST_SIZE, BOOST_SIZE});

    global_data.data.player.time += global_data.deltatime;

    draw_number(textures.ui, render, data.player.life, (Vector2){0, WINDOW_SIZE - 20}, (Size){25, 50});
    draw_number(textures.ui, render, global_data.score, (Vector2){0, 0}, (Size){30, 50});

    if (data.player.boost_resistence)
    {
        draw_from_texture(render, textures.effect[0], (Vector2){global_data.data.player.position.x - 23, global_data.data.player.position.y - 40}, global_data.data.player.size);

        draw_from_texture(render, textures.bonus[3], (Vector2){WINDOW_SIZE - 50, WINDOW_SIZE - 50}, (Size){50, 50});
    }
    else if (data.player.boost_fire)
    {
        draw_from_texture(render, textures.bonus[2], (Vector2){WINDOW_SIZE - 50, WINDOW_SIZE - 50}, (Size){50, 50});
    }
    else if (data.player.boost_damage)
    {
        draw_from_texture(render, textures.bonus[13], (Vector2){WINDOW_SIZE - 50, WINDOW_SIZE - 50}, (Size){50, 50});
    }

    draw_success(render, textures);
    draw_resistence(render, textures);
    draw_kill(render, textures);
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

                    global_data.data.enemies[k].life -= global_data.data.player.damage;
                    if (global_data.data.enemies[k].life <= 0)
                    {
                        global_data.score += SCORE_VALUE * global_data.data.enemies[k].speed;
                        enemy_dead(&global_data.data.enemies, &global_data.data.enemies_length, &k);
                        global_data.kill++;
                    }
                }
            }
        }
    }

    for (size_t k = 0; k < global_data.data.enemies_length; k++)
    {
        for (size_t i = 0; i < global_data.data.enemies[k].bullet_len; i++)
        {

            if (!global_data.data.player.boost_resistence && AABB(global_data.data.enemies[k].bullet[i].position, global_data.data.enemies[k].bullet[i].size, global_data.data.player.position, global_data.data.player.size))
            {
                remove_laser(&global_data.data.enemies[k].bullet, &global_data.data.enemies[k].bullet_len, &i);

                global_data.data.player.life -= 10;
                if (global_data.data.player.life <= 0)
                {
                    if (global_data.audio_enable)
                    {
                        Mix_PlayMusic(global_data.audio.lose, 1);
                    }

                    //printf("\nScore: %d\nTotal time: %lds\nKill: %d\n", global_data.score, time_interval(global_data.total_time), global_data.kill);
                    global_data.menu = true;
                    global_data.menu_state = 2;
                    global_data.stop_time = time(0);

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
            if (global_data.audio_enable)
            {
                Mix_PlayMusic(global_data.audio.player_laser, 1);
            }
            if (global_data.data.player.boost_fire)
            {

                l.position.x -= l.size.width * 2;
                laser_add(&global_data.data.player.bullet, l, &global_data.data.player.bullet_len);
                l.position.x += l.size.width * 4;
                laser_add(&global_data.data.player.bullet, l, &global_data.data.player.bullet_len);
            }
            else
            {

                laser_add(&global_data.data.player.bullet, l, &global_data.data.player.bullet_len);
            }
        }
    }

    add_boost_time(&global_data.data.player, &global_data.data.boost_time, global_data.deltatime);

    bool last_resistence = global_data.data.player.boost_resistence;
    bool last_fire = global_data.data.player.boost_fire;
    bool last_damage = global_data.data.player.boost_damage;

    if (update_boost(&global_data.data.player.boost_fire, &global_data.data.player.fire_pos, &global_data.data.player.boost_fire_time))
    {
        song_boost(&global_data.data.player.boost_fire, &last_fire, &global_data.song_item_m, &global_data.song_item_p, global_data.audio.item_m, global_data.audio.item_p);
    }
    else if (update_boost(&global_data.data.player.boost_resistence, &global_data.data.player.resistence_pos, &global_data.data.player.boost_resistence_time))
    {
        if (global_data.data.player.boost_resistence && !global_data.shiel_song)
        {
            global_data.shiel_song = true;
            global_data.shiel_song_stop = false;
            if (global_data.audio_enable)
            {
                Mix_PlayMusic(global_data.audio.player_shield, 3);
            }
        }
        else if (!global_data.data.player.boost_resistence)
        {
            global_data.shiel_song = false;

            if (!global_data.shiel_song_stop && last_resistence != global_data.data.player.boost_resistence)
            {
                if (global_data.audio_enable)
                {
                    Mix_PlayMusic(global_data.audio.player_shield_stop, 3);
                }
                global_data.shiel_song_stop = true;
            }
        }
    }
    else if (update_boost(&global_data.data.player.boost_damage, &global_data.data.player.damage_pos, &global_data.data.player.boost_damage_time))
    {
        song_boost(&global_data.data.player.boost_damage, &last_damage, &global_data.song_item_m, &global_data.song_item_p, global_data.audio.item_m, global_data.audio.item_p);
        if (global_data.data.player.damage == 10)
        {
            global_data.data.player.damage = 20;
        }
        else
        {
            global_data.data.player.damage = 10;
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
                if (global_data.audio_enable)
                {
                    Mix_PlayMusic(global_data.audio.enemies_laser, 1);
                }
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

/*
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
*/

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

void draw_success(SDL_Renderer *render, GameTextures textures)
{
    if (global_data.score > 10000)
    {
        draw_from_texture(render, textures.bonus[7], (Vector2){WINDOW_SIZE - 50, 0}, (Size){50, 50});
    }
    if (global_data.score > 20000)
    {
        draw_from_texture(render, textures.bonus[9], (Vector2){WINDOW_SIZE - 100, 0}, (Size){50, 50});
    }
    if (global_data.score > 30000)
    {
        draw_from_texture(render, textures.bonus[8], (Vector2){WINDOW_SIZE - 150, 0}, (Size){50, 50});
    }
}

void draw_kill(SDL_Renderer *render, GameTextures textures)
{
    if (global_data.kill >= 10)
    {
        draw_from_texture(render, textures.bonus[11], (Vector2){WINDOW_SIZE - 50, 100}, (Size){50, 50});
    }

    if (global_data.kill >= 20)
    {
        draw_from_texture(render, textures.bonus[10], (Vector2){WINDOW_SIZE - 100, 100}, (Size){50, 50});
    }

    if (global_data.kill >= 40)
    {
        draw_from_texture(render, textures.bonus[12], (Vector2){WINDOW_SIZE - 150, 100}, (Size){50, 50});
    }
}

void draw_resistence(SDL_Renderer *render, GameTextures textures)
{
    time_t diff = time_interval(global_data.total_time);
    if (global_data.data.player.life <= 50 && diff > 60)
    {
        draw_from_texture(render, textures.bonus[4], (Vector2){WINDOW_SIZE - 50, 55}, (Size){50, 50});
    }

    if (global_data.data.player.life <= 30 && diff > 60 * 2)
    {
        draw_from_texture(render, textures.bonus[6], (Vector2){WINDOW_SIZE - 100, 55}, (Size){50, 50});
    }

    if (global_data.data.player.life <= 10 && diff > 60 * 3)
    {
        draw_from_texture(render, textures.bonus[5], (Vector2){WINDOW_SIZE - 150, 55}, (Size){50, 50});
    }
}

void draw_gameover_success(SDL_Renderer *render, GameTextures textures)
{
    time_t diff = global_data.stop_time - global_data.total_time;
    int x = WINDOW_SIZE / 2 + 50;
    int y = WINDOW_SIZE / 2 + 50;
    if (global_data.data.player.life <= 50 && diff > 60)
    {
        draw_from_texture(render, textures.bonus[4], (Vector2){x - 50, y + 55}, (Size){50, 50});
    }

    if (global_data.data.player.life <= 30 && diff > 60 * 2)
    {
        draw_from_texture(render, textures.bonus[6], (Vector2){x - 100, y + 55}, (Size){50, 50});
    }

    if (global_data.data.player.life <= 10 && diff > 60 * 3)
    {
        draw_from_texture(render, textures.bonus[5], (Vector2){x - 150, y + 55}, (Size){50, 50});
    }
    if (global_data.kill >= 10)
    {
        draw_from_texture(render, textures.bonus[11], (Vector2){x - 50, y + 100}, (Size){50, 50});
    }

    if (global_data.kill >= 20)
    {
        draw_from_texture(render, textures.bonus[10], (Vector2){x - 100, y + 100}, (Size){50, 50});
    }

    if (global_data.kill >= 40)
    {
        draw_from_texture(render, textures.bonus[12], (Vector2){x - 150, y + 100}, (Size){50, 50});
    }
    if (global_data.score > 10000)
    {
        draw_from_texture(render, textures.bonus[7], (Vector2){x - 50, y + 0}, (Size){50, 50});
    }
    if (global_data.score > 20000)
    {
        draw_from_texture(render, textures.bonus[9], (Vector2){x - 100, y + 0}, (Size){50, 50});
    }
    if (global_data.score > 30000)
    {
        draw_from_texture(render, textures.bonus[8], (Vector2){x - 150, y + 0}, (Size){50, 50});
    }
}

void init_game(GameData *data, GameTextures textures, SDL_Renderer *render)
{
    Player p;
    p.texture = PLAYER_DEFAULT * 4;
    p.size.width = 100;
    p.size.height = 100;
    p.damage = 10;
    p.speed = PLAYER_SPEED;
    p.life = 100;
    p.fire_pos = (Vector2){0, -BOOST_SIZE};
    p.resistence_pos = (Vector2){0, -BOOST_SIZE};
    p.damage_pos = (Vector2){0, -BOOST_SIZE};
    p.boost_damage = false;
    p.boost_resistence = false;
    p.boost_fire = false;
    p.boost_fire_time = 0;
    p.boost_resistence_time = 0;
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
    load_directory_textures("./out/assets/bonus", &textures->bonus, render);
    load_directory_textures("./out/assets/effect", &textures->effect, render);
}

void init_audio(GameAudio *audio)
{
    audio->player_laser = Mix_LoadMUS("./out/assets/audio/sfx_laser1.ogg");
    audio->enemies_laser = Mix_LoadMUS("./out/assets/audio/sfx_laser2.ogg");
    audio->player_shield = Mix_LoadMUS("./out/assets/audio/sfx_shieldDown.ogg");
    audio->player_shield_stop = Mix_LoadMUS("./out/assets/audio/sfx_shieldUp.ogg");
    audio->lose = Mix_LoadMUS("./out/assets/audio/sfx_lose.ogg");
    audio->item_p = Mix_LoadMUS("./out/assets/audio/sfx_zap.ogg");
    audio->item_m = Mix_LoadMUS("./out/assets/audio/sfx_twoTone.ogg");
}

void song_boost(bool *p, bool *l, bool *im, bool *ip, Mix_Music *sm, Mix_Music *sp)
{
    if ((*p) && !(*ip))
    {
        (*ip) = true;
        (*im) = false;
        if (global_data.audio_enable)
        {
            Mix_PlayMusic(sp, 2);
        }
    }
    else if (!(*p))
    {
        (*ip) = false;

        if (!(*im) && (*l) != (*p))
        {
            if (global_data.audio_enable)
            {
                Mix_PlayMusic(sm, 2);
            }
            (*im) = true;
        }
    }
}

bool update_boost(bool *is, Vector2 *pos, double *time)
{
    if ((*is) && (*pos).y == -BOOST_SIZE)
    {
        (*time) += global_data.deltatime * 10;
        if ((*time) > BOOST_DURATION)
        {
            (*is) = false;
            (*time) = 0;
        }
        return true;
    }
    if (!(*is) && (*pos).y != -BOOST_SIZE)
    {
        (*pos).y += global_data.deltatime * BOOST_SPEED;
        if (AABB((*pos), (Size){BOOST_SIZE, BOOST_SIZE}, global_data.data.player.position, global_data.data.player.size))
        {
            (*is) = true;
            (*pos) = (Vector2){0, -BOOST_SIZE};
            (*time) = 0;
        }
        if ((*pos).y > WINDOW_SIZE)
        {
            (*pos) = (Vector2){0, -BOOST_SIZE};
            (*is) = false;
            (*time) = 0;
        }
        return true;
    }

    return false;
}

void menu_mode(SDL_Renderer *renderer, SDL_Keycode key)
{
    if (global_data.menu_state == 0)
    {
        draw_text("Space Shooter", renderer, global_data.font40, (Vector2){0, 30}, (Size){-1, 10}, true, false);
        draw_text("press space to start", renderer, global_data.font12, (Vector2){0, WINDOW_SIZE - 200}, (Size){-1, 10}, true, false);

        for (size_t i = 0; i < 4; i++)
        {
            if (i == global_data.menu_index)
            {
                draw_from_texture(renderer, global_data.textures.ui[10], (Vector2){(((WINDOW_SIZE / 5) * (1 + i)) - 50) - 10, (WINDOW_SIZE - 200) / 2 - 10}, (Size){0, 0});
            }
            draw_from_texture(renderer, global_data.textures.player[i * 4 + global_data.color_index], (Vector2){((WINDOW_SIZE / 5) * (1 + i)) - 50, (WINDOW_SIZE - 200) / 2}, (Size){200, 200});
        }

        if (key == SDLK_RIGHT)
        {
            global_data.menu_press += global_data.deltatime * 10;
            if (global_data.menu_press > 10)
            {
                global_data.menu_press = 0;
                global_data.menu_index++;
            }
        }
        else if (key == SDLK_LEFT)
        {
            global_data.menu_press += global_data.deltatime * 10;
            if (global_data.menu_press > 10)
            {
                global_data.menu_press = 0;
                global_data.menu_index--;
            }
        }

        if (global_data.menu_index < 0)
        {
            global_data.menu_index = 3;
        }
        else if (global_data.menu_index > 3)
        {
            global_data.menu_index = 0;
        }

        if (key == SDLK_UNKNOWN)
        {
            global_data.menu_press = 0;
        }

        if (key == SDLK_SPACE)
        {
            global_data.data.player.texture = global_data.menu_index * 4;
            global_data.menu_state = 1;
        }
    }
    else if (global_data.menu_state == 1)
    {
        global_data.menu_start_time += global_data.deltatime * 10;

        if (global_data.menu_start_time > 100)
        {
            global_data.menu_start_index--;
            global_data.menu_start_time = 0;
            if (global_data.menu_start_index <= 0)
            {
                global_data.menu_start_index = 0;
                global_data.menu = false;
            }
        }

        if (global_data.menu_start_index == 4)
        {
            draw_text("READY", renderer, global_data.font40, (Vector2){0, 0}, (Size){-1, -1}, true, true);
        }
        else
        {
            char *num = malloc(sizeof(char) * 5);
            sprintf(num, "%d", global_data.menu_start_index);
            draw_text(num, renderer, global_data.font40, (Vector2){0, 0}, (Size){-1, -1}, true, true);
            free(num);
        }
    }
    else if (global_data.menu_state == 2)
    {
        draw_text("Game Over", renderer, global_data.font40, (Vector2){0, 50}, (Size){-1, -1}, true, false);

        char *str_score = malloc(sizeof(char) * 100);
        char *str_kill = malloc(sizeof(char) * 10);
        char *str_time = malloc(sizeof(char) * 10);

        sprintf(str_score, "Score %d", global_data.score);
        sprintf(str_kill, "Kill %d", global_data.kill);
        sprintf(str_time, "Time %lds", global_data.stop_time - global_data.total_time);

        draw_text(str_score, renderer, global_data.font12, (Vector2){0, (WINDOW_SIZE / 2) - 100}, (Size){-1, -1}, true, false);
        draw_text(str_kill, renderer, global_data.font12, (Vector2){0, (WINDOW_SIZE / 2) - 50}, (Size){-1, -1}, true, false);
        draw_text(str_time, renderer, global_data.font12, (Vector2){0, (WINDOW_SIZE / 2)}, (Size){-1, -1}, true, false);

        free(str_score);
        free(str_kill);
        free(str_time);

        draw_gameover_success(renderer, global_data.textures);

        if (key == SDLK_SPACE)
        {
            global_data.running = false;
        }
    }
}

void draw_text(const char *text, SDL_Renderer *renderer, TTF_Font *font, Vector2 position, Size size, bool centerX, bool centerY)
{
    SDL_Color textBackgroundColor = {0x00, 0x00, 0x00, 0xFF};
    SDL_Color textColor = {0xFF, 0xFF, 0xFF, 0xFF};

    SDL_Surface *textSurface = TTF_RenderText_Shaded(font, text, textColor, textBackgroundColor);

    if (textSurface)
    {
        SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (text)
        {
            if (centerX)
            {
                position.x = (WINDOW_SIZE - textSurface->w) / 2;
            }

            if (centerY)
            {
                position.y = (WINDOW_SIZE - textSurface->h) / 2;
            }

            if (size.width == -1)
            {
                size.width = textSurface->w;
            }
            if (size.height == -1)
            {
                size.height = textSurface->h;
            }

            draw_from_texture(renderer, text, position, size);
            SDL_DestroyTexture(text);
        }

        SDL_FreeSurface(textSurface);
    }
}