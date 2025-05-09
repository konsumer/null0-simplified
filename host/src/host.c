// implementation of shared functions (derived from host-specific
// functions)

#include "host.h"

#define PNTR_IMPLEMENTATION
#define PNTR_ENABLE_DEFAULT_FONT
#define PNTR_ENABLE_VARGS
#define PNTR_ENABLE_TTF
#define PNTR_ENABLE_UTF8
#define PNTR_ENABLE_MATH
#define PNTR_ENABLE_JPEG
#define PNTR_PIXELFORMAT_RGBA
#define PNTR_LOAD_FILE fs_load_file
#define PNTR_SAVE_FILE fs_save_file
#include "pntr.h"

static AppData appData = {};

char *copy_string_from_cart(unsigned int cart_pointer) {
  unsigned int size = cart_strlen(cart_pointer) + 1;
  return (char *)copy_memory_from_cart(cart_pointer, size);
}

unsigned int copy_string_to_cart(char *host_pointer) {
  unsigned int size = strlen(host_pointer) + 1;
  return copy_memory_to_cart((void *)host_pointer, size);
}

pntr_image *get_image(unsigned int id) {
  return appData.images[id];
}

unsigned int add_image(pntr_image *image) {
  unsigned int id = cvector_size(appData.images);
  cvector_push_back(appData.images, image);
  return id;
}

pntr_font *get_font(unsigned int id) {
  return appData.fonts[id];
}

unsigned int add_font(pntr_font *font) {
  unsigned int id = cvector_size(appData.fonts);
  cvector_push_back(appData.fonts, font);
  return id;
}

pntr_color cart_color(unsigned int colorPtr) {
  CartColor *c = copy_memory_from_cart(colorPtr, sizeof(CartColor));
  pntr_color ret = pntr_new_color(c->r, c->g, c->b, c->a);
  free(c);
  return ret;
}

/// these are shared host-functions:

// Log a string
HOST_FUNCTION(void, trace, (uint32_t strPtr), {
  char *str = copy_string_from_cart(strPtr);
  printf("%s\n", str);
  free(str);
})

// Clear an image
HOST_FUNCTION(void, clear, (uint32_t destinationPtr, uint32_t colorPtr), {
  pntr_clear_background(get_image(destinationPtr), cart_color(colorPtr));
})
