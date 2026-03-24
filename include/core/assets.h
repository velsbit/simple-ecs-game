#ifndef ASSETS_H
#define ASSETS_H

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t TextureID;

void assets_init(void);
TextureID assets_load_texture(const char *filename);
void *assets_get_texture(TextureID id);
void assets_shutdown(void);

bool assets_load_map(const char *filename);
bool assets_generate_map(uint32_t seed);

#endif