#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// font.h contains a 2.2 MB font in a form of a C array
// The reason for that is to port a standalone file and embed
// any resources inside
// Don't open it in any text editor, for the love of god
#include "font.h"

#define OS_UNKNOWN 0
#define OS_WINDOWS 1
#define OS_LINUX 2

#ifdef _WIN32
#define OS OS_WINDOWS
#elif __linux__
#define OS OS_LINUX
#else
#define OS OS_UNKNOWN
#endif

void load_color(SDL_Renderer *renderer, int *color);
SDL_Color arr_to_color(int *color);
float lerp(float a, float b, float t);

int main(int argc, char *argv[]) {
  int color_active[3] = {230, 230, 240};
  int color_inactive[3] = {179, 179, 179};
  int color_bg[3] = {24, 24, 27};
  float scale_factor = 3.0f;
  int fade_window = 1;
  char options[16][2][128] = {
      {"󰈹 Firefox", "firefox"},
      {"󰗃 YouTube", "firefox https://youtube.com"},
      {" Spotify", "firefox https://open.spotify.com"},
      {"󰨞 VS Code", "Code"},
  };
  int options_num = 4;
  char greets[16][128] = {
      "hi there",      "hi hello hii", "it's time",
      "back already?", "what now?",    "code code code...",
  };
  int greets_num = 6;
  char footers[16][128] = {
      "there's no place like 127.0.0.1",
      "chatgpt time!!",
      "waiting for something to happen?",
      "hop on nvim",
      "hop on vscode",
      "youtube binging time",
      "attack/helicopter",
      "arch/btw",
  };
  int footers_num = 8;

  char config_path[128];

  if (getenv("XDG_CONFIG_HOME")) {
    sprintf(config_path, "%s/sWelcomer/swelcomer.scf",
            getenv("XDG_CONFIG_HOME"));
  } else {
    if (OS == OS_WINDOWS) {
      sprintf(config_path, "%s/sWelcomer/swelcomer.scf",
              getenv("LOCALAPPDATA"));
    } else if (OS == OS_LINUX) {
      sprintf(config_path, "%s/.config/sWelcomer/swelcomer.scf",
              getenv("HOME"));
    }
  }

  FILE *config_file;
  config_file = fopen(config_path, "r");

  if (config_file != NULL) {
    char section[128] = "";

    char line[512] = "";
    int line_ch = 0;
    char ch;

    while ((ch = fgetc(config_file)) != EOF) {
      line[line_ch] = ch;
      line_ch++;

      if (line[line_ch - 1] == '\n') {
        if (line[strlen(line) - 1] == '\n') {
          line[strlen(line) - 1] = '\0';
        }

        int equals_pos = -1;

        for (int i = 0; i < (int)strlen(line); i++) {
          if (line[i] == '=') {
            equals_pos = i;
          }
        }

        if (equals_pos != -1) {
          char key[128] = "";
          char value[128] = "";

          memcpy(key, line, equals_pos);
          memcpy(value, line + equals_pos + 1, strlen(line) - equals_pos - 1);

          while (key[strlen(key) - 1] == ' ') {
            key[strlen(key) - 1] = '\0';
          }

          while (value[0] == ' ') {
            memmove(value, value + 1, 127);
          }

          if (strcmp(section, "general") == 0) {
            if (strcmp(key, "window_factor") == 0) {
              scale_factor = atof(value);
            }

            if (strcmp(key, "fade_window") == 0) {
              if (strcmp(value, "true") == 0) {
                fade_window = 1;
              } else {
                fade_window = 0;
              }
            }

            if (strcmp(key, "color_active") == 0) {
              if (value[0] == '#') {
                memmove(value, value + 1, 127);
              }

              color_active[0] = (strtol(value, NULL, 16) >> 16) & 0xff;
              color_active[1] = (strtol(value, NULL, 16) >> 8) & 0xff;
              color_active[2] = strtol(value, NULL, 16) & 0xff;
            }

            if (strcmp(key, "color_inactive") == 0) {
              if (value[0] == '#') {
                memmove(value, value + 1, 127);
              }

              color_inactive[0] = (strtol(value, NULL, 16) >> 16) & 0xff;
              color_inactive[1] = (strtol(value, NULL, 16) >> 8) & 0xff;
              color_inactive[2] = strtol(value, NULL, 16) & 0xff;
            }

            if (strcmp(key, "color_bg") == 0) {
              if (value[0] == '#') {
                memmove(value, value + 1, 127);
              }

              color_bg[0] = (strtol(value, NULL, 16) >> 16) & 0xff;
              color_bg[1] = (strtol(value, NULL, 16) >> 8) & 0xff;
              color_bg[2] = strtol(value, NULL, 16) & 0xff;
            }
          }
        }

        if (strcmp(section, "options") == 0) {
          int semicolon_pos = -1;

          for (int i = 0; i < (int)strlen(line); i++) {
            if (line[i] == ';') {
              semicolon_pos = i;
            }
          }

          if (semicolon_pos != -1) {
            char name[128] = "";
            char command[128] = "";

            memcpy(name, line, semicolon_pos);
            memcpy(command, line + semicolon_pos + 1,
                   strlen(line) - semicolon_pos - 1);

            while (name[strlen(name) - 1] == ' ') {
              name[strlen(name) - 1] = '\0';
            }

            while (command[0] == ' ') {
              memmove(command, command + 1, 127);
            }

            if (line[0] != '\0' && line[0] != '[' &&
                line[strlen(line) - 1] != ']') {
              strcpy(options[options_num][0], name);
              strcpy(options[options_num][1], command);
              options_num++;
            }
          }
        }

        if (strcmp(section, "greets") == 0) {
          if (line[0] != '\0' && line[0] != '[' &&
              line[strlen(line) - 1] != ']') {
            strcpy(greets[greets_num], line);
            greets_num++;
          }
        }

        if (strcmp(section, "footers") == 0) {
          if (line[0] != '\0' && line[0] != '[' &&
              line[strlen(line) - 1] != ']') {
            strcpy(footers[footers_num], line);
            footers_num++;
          }
        }

        if (line[0] == '[' && line[strlen(line) - 1] == ']') {
          memset(section, 0, 128);
          memcpy(section, line + 1, strlen(line) - 2);

          if (strcmp(section, "options") == 0) {
            options_num = 0;
          }

          if (strcmp(section, "greets") == 0) {
            greets_num = 0;
          }

          if (strcmp(section, "footers") == 0) {
            footers_num = 0;
          }
        }

        memset(line, 0, 512);
        line_ch = 0;
      }
    }

    fclose(config_file);
  }

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Init error: %s\n", SDL_GetError());
    return 1;
  }

  if (TTF_Init() != 0) {
    fprintf(stderr, "TTF Init error: %s\n", TTF_GetError());
    return 1;
  }

  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);

  SDL_Window *window = SDL_CreateWindow(
      "sWelcomer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0,
      SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);

  if (window == NULL) {
    fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_SetWindowOpacity(window, 0.0f);

  int window_size[] = {(int)(display_mode.w / scale_factor),
                       (int)(display_mode.h / scale_factor)};
  SDL_SetWindowSize(window, window_size[0], window_size[1]);
  SDL_SetWindowPosition(window, display_mode.w / 2 - window_size[0] / 2,
                        display_mode.h / 2 - window_size[1] / 2);

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (renderer == NULL) {
    fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_RWops *font_rw = SDL_RWFromConstMem(font_data, FONT_DATA_SIZE);
  TTF_Font *font = TTF_OpenFontRW(font_rw, 1, 48);

  int running = 1;
  SDL_Event event;

  srand(time(0));

  int greet_current = rand() % greets_num;
  int footer_current = rand() % footers_num;
  float window_opacity = 0.0f;

  int option = 0;
  int option_colors[64][3];
  float option_offsets[64] = {};

  for (int i = 0; i < 64; i++) {
    option_colors[i][0] = color_inactive[0];
    option_colors[i][1] = color_inactive[1];
    option_colors[i][2] = color_inactive[2];
  }

  if (!fade_window) {
    window_opacity = 0.8f;
  }

  while (running) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = 0;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          running = 0;
          break;
        case SDLK_UP:
          option = (option == 0) ? options_num - 1 : option - 1;
          break;
        case SDLK_DOWN:
          option = (option == options_num - 1) ? 0 : option + 1;
          break;
        case SDLK_RETURN:
          char command[128];

          if (OS == OS_WINDOWS) {
            sprintf(command, "start %s", options[option][1]);
          } else if (OS == OS_LINUX) {
            sprintf(command, "%s", options[option][1]);
          }

          system(command);
          running = 0;
          break;
        default:
          break;
        }
        break;
      default:
        break;
      }
    }

    SDL_SetWindowOpacity(window, window_opacity);

    window_opacity =
        (window_opacity <= 0.9) ? window_opacity + 0.1 : window_opacity;

    load_color(renderer, color_bg);
    SDL_RenderClear(renderer);

    SDL_Surface *greet_surface = TTF_RenderUTF8_Shaded(
        font, greets[greet_current], arr_to_color(color_active),
        arr_to_color(color_bg));
    SDL_Texture *greet_texture =
        SDL_CreateTextureFromSurface(renderer, greet_surface);
    SDL_Rect greet_dst = {window_size[0] / 2 - greet_surface->w / 2,
                          window_size[1] / 50, greet_surface->w,
                          greet_surface->h};
    SDL_RenderCopy(renderer, greet_texture, NULL, &greet_dst);

    SDL_Surface *footer_surface = TTF_RenderUTF8_Shaded(
        font, footers[footer_current], arr_to_color(color_inactive),
        arr_to_color(color_bg));
    SDL_Texture *footer_texture =
        SDL_CreateTextureFromSurface(renderer, footer_surface);
    SDL_Rect footer_dst = {window_size[0] / 2 - footer_surface->w / 6,
                           window_size[1] - footer_surface->h / 2,
                           footer_surface->w / 3, footer_surface->h / 3};
    SDL_RenderCopy(renderer, footer_texture, NULL, &footer_dst);

    for (int i = 0; i < options_num; i++) {
      if (i == option) {
        option_offsets[i] = lerp(option_offsets[i], -4.0f, 0.25f);
        option_colors[i][0] = lerp(option_colors[i][0], color_active[0], 0.25f);
        option_colors[i][1] = lerp(option_colors[i][1], color_active[1], 0.25f);
        option_colors[i][2] = lerp(option_colors[i][2], color_active[2], 0.25f);
      } else {
        option_offsets[i] = lerp(option_offsets[i], 0.0f, 0.25f);
        option_colors[i][0] =
            lerp(option_colors[i][0], color_inactive[0], 0.25f);
        option_colors[i][1] =
            lerp(option_colors[i][1], color_inactive[1], 0.25f);
        option_colors[i][2] =
            lerp(option_colors[i][2], color_inactive[2], 0.25f);
      }

      SDL_Surface *option_surface = TTF_RenderUTF8_Shaded(
          font, options[i][0], arr_to_color(option_colors[i]),
          arr_to_color(color_bg));
      SDL_Texture *option_texture =
          SDL_CreateTextureFromSurface(renderer, option_surface);
      SDL_Rect option_dst = {window_size[0] / 2 - option_surface->w / 4,
                             window_size[1] / 3 - option_surface->h / 2 +
                                 i * option_surface->h / 2 +
                                 (int)option_offsets[i],
                             option_surface->w / 2, option_surface->h / 2};
      SDL_RenderCopy(renderer, option_texture, NULL, &option_dst);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  TTF_Quit();

  return 0;
}

void load_color(SDL_Renderer *renderer, int *color) {
  SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 255);
}

SDL_Color arr_to_color(int *color) {
  return (SDL_Color){(uint8_t)color[0], (uint8_t)color[1], (uint8_t)color[2],
                     255};
}

float lerp(float a, float b, float t) { return a + (b - a) * t; }
