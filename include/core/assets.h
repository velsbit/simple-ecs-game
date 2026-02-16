#ifndef ASSETS_H
#define ASSETS_H

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t SpriteID;

void assets_init(void);
SpriteID assets_load_texture(const char *filename);
void *assets_get_texture(SpriteID id);
void assets_shutdown(void);

bool assets_load_map(const char *filename);

#endif