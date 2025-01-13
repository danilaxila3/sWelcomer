#include "font.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

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

int main(int argc, char *argv[]) {
  int color_active[3] = {230, 230, 240};
  int color_inactive[3] = {179, 179, 179};
  int color_bg[3] = {24, 24, 27};
  float scale_factor = 3.0f;

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

            if (strcmp(key, "color_bg") == 0) {
              if (value[0] == '#') {
                memmove(value, value + 1, 127);
              }

              color_bg[0] = (strtol(value, NULL, 16) >> 16) & 0xff;
              color_bg[1] = (strtol(value, NULL, 16) >> 8) & 0xff;
              color_bg[2] = strtol(value, NULL, 16) & 0xff;

              printf("%d,%d,%d\n", color_bg[0], color_bg[1], color_bg[2]);
            }
          }
        }

        if (line[0] == '[' && line[strlen(line) - 1] == ']') {
          memcpy(section, line + 1, strlen(line) - 2);
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

  int running = 1;
  SDL_Event event;

  float window_opacity = 0.0f;

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

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void load_color(SDL_Renderer *renderer, int *color) {
  SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 255);
}
