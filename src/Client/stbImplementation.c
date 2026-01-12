// The only C File to define STB_IMPLEMENTATION so that stb doesn't get compiled multiple times but can be included everywhere else
// ReSharper disable CppUnusedIncludeDirective

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#undef STB_IMAGE_WRITE_IMPLEMENTATION

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"
#undef STB_IMAGE_RESIZE_IMPLEMENTATION

#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"
#undef STB_PERLIN_IMPLEMENTATION
