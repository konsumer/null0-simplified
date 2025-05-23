// implementation of shared functions (derived from host-specific
// functions)

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

#include "host.h"

static cvector_vector_type(pntr_image *) images;
static cvector_vector_type(pntr_font *) fonts;

char *copy_string_from_cart(unsigned int cart_pointer) {
  unsigned int size = cart_strlen(cart_pointer) + 1;
  return (char *)copy_memory_from_cart(cart_pointer, size);
}

unsigned int copy_string_to_cart(char *host_pointer) {
  unsigned int size = strlen(host_pointer) + 1;
  return copy_memory_to_cart((void *)host_pointer, size);
}

pntr_color copy_color_from_cart(unsigned int colorPtr) {
  CartColor *c = copy_memory_from_cart(colorPtr, sizeof(CartColor));
  pntr_color ret = pntr_new_color(c->r, c->g, c->b, c->a);
  free(c);
  return ret;
}

unsigned int copy_color_to_cart(pntr_color color) {
  CartColor *c = malloc(sizeof(CartColor));
  c->r = color.rgba.r;
  c->g = color.rgba.g;
  c->b = color.rgba.b;
  c->a = color.rgba.a;
  return copy_memory_to_cart(c, sizeof(CartColor));
}

// These helpers make it easier to use the static appData elsewhere

pntr_image *get_image(unsigned int id) {
  return images[id];
}

unsigned int add_image(pntr_image *image) {
  if (image == NULL) {
    return 0;
  }
  unsigned int id = cvector_size(images);
  cvector_push_back(images, image);
  return id;
}

pntr_font *get_font(unsigned int id) {
  return fonts[id];
}

unsigned int add_font(pntr_font *font) {
  unsigned int id = cvector_size(fonts);
  cvector_push_back(fonts, font);
  return id;
}

// these are EMSCRIPTEN-only functions to mess with resources (for use in unit-tests)
#ifdef EMSCRIPTEN
EMSCRIPTEN_KEEPALIVE pntr_image *host_get_image(unsigned int id) {
  return get_image(id);
}
EMSCRIPTEN_KEEPALIVE pntr_font *host_get_font(unsigned int id) {
  return get_font(id);
}

#endif // EMSCRIPTEN

/// these are shared host-functions:

// Clear an image
HOST_FUNCTION(void, clear, (uint32_t imageID, uint32_t colorPtr), {
  pntr_clear_background(images[imageID], copy_color_from_cart(colorPtr));
})

// Draw an arc on an image. Set thickness to 0 for "fill".
HOST_FUNCTION(void, draw_arc, (uint32_t imageID, int32_t centerX, int32_t centerY, float radius, float startAngle, float endAngle, int32_t segments, uint32_t thickness, uint32_t colorPtr), {
  pntr_color color = copy_color_from_cart(colorPtr);
  if (thickness != 0) {
    pntr_draw_arc_thick(images[imageID], centerX, centerY, radius, startAngle, endAngle, segments, thickness, color);
  } else {
    pntr_draw_arc_fill(images[imageID], centerX, centerY, radius, startAngle, endAngle, segments, color);
  }
})

// Draw a circle on an image. Set thickness to 0 for "fill".
HOST_FUNCTION(void, draw_circle, (uint32_t imageID, int32_t centerX, int32_t centerY, int32_t radius, uint32_t thickness, uint32_t colorPtr), {
  pntr_color color = copy_color_from_cart(colorPtr);
  if (thickness != 0) {
    pntr_draw_circle_thick(images[imageID], centerX, centerY, radius, thickness, color);
  } else {
    pntr_draw_circle_fill(images[imageID], centerX, centerY, radius, color);
  }
})

// Draw an ellipse on an image. Set thickness to 0 for "fill".
HOST_FUNCTION(void, draw_ellipse, (uint32_t imageID, int32_t centerX, int32_t centerY, int32_t radiusX, int32_t radiusY, uint32_t thickness, uint32_t colorPtr), {
  pntr_color color = copy_color_from_cart(colorPtr);
  if (thickness != 0) {
    pntr_draw_ellipse_thick(images[imageID], centerX, centerY, radiusX, radiusY, thickness, color);
  } else {
    pntr_draw_ellipse_fill(images[imageID], centerX, centerY, radiusX, radiusY, color);
  }
})

// Draw a line on an image.
HOST_FUNCTION(void, draw_line, (uint32_t imageID, int32_t startPosX, int32_t startPosY, int32_t endPosX, int32_t endPosY, uint32_t thickness, uint32_t colorPtr), {
  pntr_color color = copy_color_from_cart(colorPtr);
  if (thickness != 0) {
    pntr_draw_line_thick(images[imageID], startPosX, startPosY, endPosX, endPosY, thickness, color);
  } else {
    pntr_draw_line(images[imageID], startPosX, startPosY, endPosX, endPosY, color);
  }
})

// Draw a single pixel on an image
HOST_FUNCTION(void, draw_point, (uint32_t imageID, int32_t x, int32_t y, uint32_t colorPtr), {
  pntr_draw_point(images[imageID], x, y, copy_color_from_cart(colorPtr));
})

// Draw a rectangle on an image. Set thickness to 0 for "fill".
HOST_FUNCTION(void, draw_rectangle, (uint32_t imageID, int32_t posX, int32_t posY, int32_t width, int32_t height, uint32_t thickness, uint32_t colorPtr), {
  pntr_color color = copy_color_from_cart(colorPtr);
  if (thickness != 0) {
    pntr_draw_rectangle_thick(images[imageID], posX, posY, width, height, thickness, color);
  } else {
    pntr_draw_rectangle_fill(images[imageID], posX, posY, width, height, color);
  }
})

// Draw a filled round-rectangle on an image. Set thickness to 0 for "fill".
HOST_FUNCTION(void, draw_rectangle_rounded, (uint32_t imageID, int32_t x, int32_t y, int32_t width, int32_t height, int32_t cornerRadius, uint32_t thickness, uint32_t colorPtr), {
  pntr_color color = copy_color_from_cart(colorPtr);
  if (thickness != 0) {
    pntr_draw_rectangle_thick_rounded(images[imageID], x, y, width, height, cornerRadius, cornerRadius, cornerRadius, cornerRadius, thickness, color);
  } else {
    pntr_draw_rectangle_rounded_fill(images[imageID], x, y, width, height, cornerRadius, color);
  }
})

// Draw text on an image
HOST_FUNCTION(void, draw_text, (uint32_t imageID, uint32_t fontID, uint32_t textPtr, int32_t posX, int32_t posY, uint32_t colorPtr), {
  char *text = copy_string_from_cart(textPtr);
  pntr_draw_text(images[imageID], fonts[fontID], text, posX, posY, copy_color_from_cart(colorPtr));
  free(text);
})

// Draw a triangle on an image. Set thickness to 0 for "fill".
HOST_FUNCTION(void, draw_triangle, (uint32_t imageID, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t thickness, uint32_t colorPtr), {
  pntr_color color = copy_color_from_cart(colorPtr);
  if (thickness != 0) {
    pntr_draw_triangle_thick(images[imageID], x1, y1, x2, y2, x3, y3, thickness, color);
  } else {
    pntr_draw_triangle_fill(images[imageID], x1, y1, x2, y2, x3, y3, color);
  }
})

HOST_FUNCTION(void, draw_image, (uint32_t destinationPtr, uint32_t srcPtr, int32_t posX, int32_t posY), {
  pntr_image *destination = images[destinationPtr];
  pntr_image *src = images[srcPtr];
  pntr_draw_image(destination, src, posX, posY);
})

// Draw an image, flipped, on an image
HOST_FUNCTION(void, draw_image_flipped, (uint32_t destinationPtr, uint32_t srcPtr, int32_t posX, int32_t posY, bool flipHorizontal, bool flipVertical, bool flipDiagonal), {
  pntr_image *destination = images[destinationPtr];
  pntr_image *src = images[srcPtr];
  pntr_draw_image_flipped(destination, src, posX, posY, flipHorizontal, flipVertical, flipDiagonal);
})

// Draw an image, rotated, on an image
HOST_FUNCTION(void, draw_image_rotated, (uint32_t destinationPtr, uint32_t srcPtr, int32_t posX, int32_t posY, float degrees, float offsetX, float offsetY, uint32_t filter), {
  pntr_image *destination = images[destinationPtr];
  pntr_image *src = images[srcPtr];
  pntr_draw_image_rotated(destination, src, posX, posY, degrees, offsetX, offsetY, filter);
})

// Draw an image, scaled, on an image
HOST_FUNCTION(void, draw_image_scaled, (uint32_t destinationPtr, uint32_t srcPtr, int32_t posX, int32_t posY, float scaleX, float scaleY, float offsetX, float offsetY, uint32_t filter), {
  pntr_image *destination = images[destinationPtr];
  pntr_image *src = images[srcPtr];
  pntr_draw_image_scaled(destination, src, posX, posY, scaleX, scaleY, offsetX, offsetY, filter);
})

// Draw a tinted image on an image
HOST_FUNCTION(void, draw_image_tint, (uint32_t destinationPtr, uint32_t srcPtr, int32_t posX, int32_t posY, uint32_t tintPtr), {
  pntr_image *destination = images[destinationPtr];
  pntr_image *src = images[srcPtr];
  pntr_color tint = copy_color_from_cart(tintPtr);
  pntr_draw_image_tint(destination, src, posX, posY, tint);
})

// Use an image as an alpha-mask on another image
HOST_FUNCTION(void, image_alpha_mask, (uint32_t imagePtr, uint32_t alphaMaskPtr, int32_t posX, int32_t posY), {
  pntr_image *image = images[imagePtr];
  pntr_image *alphaMask = images[alphaMaskPtr];
  pntr_image_alpha_mask(image, alphaMask, posX, posY);
})

// Adjust the brightness of an image, in-place
HOST_FUNCTION(void, image_color_brightness, (uint32_t imagePtr, float factor), {
  pntr_image *image = images[imagePtr];
  pntr_image_color_brightness(image, factor);
})

// Change the contrast of an image, in-place
HOST_FUNCTION(void, image_color_contrast, (uint32_t imagePtr, float contrast), {
  pntr_image *image = images[imagePtr];
  pntr_image_color_contrast(image, contrast);
})

// Fade a color in an image, in-place
HOST_FUNCTION(void, image_color_fade, (uint32_t imagePtr, float alpha), {
  pntr_image *image = images[imagePtr];
  pntr_image_color_fade(image, alpha);
})

// Invert the colors in an image, in-place
HOST_FUNCTION(void, image_color_invert, (uint32_t imagePtr), {
  pntr_image *image = images[imagePtr];
  pntr_image_color_invert(image);
})

// Replace a color in an image, in-place
HOST_FUNCTION(void, image_color_replace, (uint32_t imagePtr, uint32_t colorPtr, uint32_t replacePtr), {
  pntr_image *image = images[imagePtr];
  pntr_color color = copy_color_from_cart(colorPtr);
  pntr_color replace = copy_color_from_cart(replacePtr);
  pntr_image_color_replace(image, color, replace);
})

// Tint a color in an image, in-place
HOST_FUNCTION(void, image_color_tint, (uint32_t imagePtr, uint32_t colorPtr), {
  pntr_image *image = images[imagePtr];
  pntr_color color = copy_color_from_cart(colorPtr);
  pntr_image_color_tint(image, color);
})

// Copy an image to a new image
HOST_FUNCTION(uint32_t, image_copy, (uint32_t imagePtr), {
  pntr_image *image = images[imagePtr];
  return add_image(pntr_image_copy(image));
})

// Crop an image, in-place
HOST_FUNCTION(void, image_crop, (uint32_t imagePtr, int32_t x, int32_t y, int32_t width, int32_t height), {
  pntr_image *image = images[imagePtr];
  pntr_image_crop(image, x, y, width, height);
})

// Flip an image, in-place
HOST_FUNCTION(void, image_flip, (uint32_t imagePtr, bool horizontal, bool vertical), {
  pntr_image *image = images[imagePtr];
  pntr_image_flip(image, horizontal, vertical);
})

// Create a new image of a gradient
HOST_FUNCTION(uint32_t, image_gradient, (int32_t width, int32_t height, uint32_t topLeftPtr, uint32_t topRightPtr, uint32_t bottomLeftPtr, uint32_t bottomRightPtr), {
  pntr_color topLeft = copy_color_from_cart(topLeftPtr);
  pntr_color topRight = copy_color_from_cart(topRightPtr);
  pntr_color bottomLeft = copy_color_from_cart(bottomLeftPtr);
  pntr_color bottomRight = copy_color_from_cart(bottomRightPtr);
  return add_image(pntr_gen_image_gradient(width, height, topLeft, topRight, bottomLeft, bottomRight));
})

HOST_FUNCTION(uint32_t, image_load, (uint32_t filenamePtr), {
  char *filename = copy_string_from_cart(filenamePtr);
  pntr_image *i = pntr_load_image(filename);
  return add_image(i);
})

// Meaure an image (use 0 for screen)
HOST_FUNCTION(uint32_t, image_measure, (uint32_t imagePtr), {
  pntr_image *image = images[imagePtr];
  pntr_vector dim = (pntr_vector){.x = image->width, .y = image->height};
  return copy_memory_to_cart(&dim, sizeof(dim));
})

// Create a new blank image
HOST_FUNCTION(uint32_t, image_new, (int32_t width, int32_t height, uint32_t colorPtr), {
  pntr_color color = copy_color_from_cart(colorPtr);
  return add_image(pntr_gen_image_color(width, height, color));
})

// Resize an image, in-place
HOST_FUNCTION(void, image_resize, (uint32_t imagePtr, int32_t newWidth, int32_t newHeight, int32_t offsetX, int32_t offsetY, uint32_t fillPtr), {
  pntr_image *image = images[imagePtr];
  pntr_color fill = copy_color_from_cart(fillPtr);
  pntr_image_resize_canvas(image, newWidth, newHeight, offsetX, offsetY, fill);
})

// Create a new image, rotating another image
HOST_FUNCTION(uint32_t, image_rotate, (uint32_t imagePtr, float degrees, uint32_t filter), {
  pntr_image *image = images[imagePtr];
  return add_image(pntr_image_rotate(image, degrees, filter));
})

// Save an image to persistant storage
HOST_FUNCTION(void, image_save, (uint32_t imagePtr, uint32_t filenamePtr), {
  pntr_image *image = images[imagePtr];
  char *filename = copy_string_from_cart(filenamePtr);
  pntr_save_image(image, filename);
})

// Unload an image
HOST_FUNCTION(void, image_unload, (uint32_t imagePtr), {
  pntr_image *image = images[imagePtr];
  pntr_unload_image(image);
})

// Create an image from a region of another image
HOST_FUNCTION(uint32_t, subimage, (uint32_t imagePtr, int32_t x, int32_t y, int32_t width, int32_t height), {
  pntr_image *image = images[imagePtr];
  return add_image(pntr_image_subimage(image, x, y, width, height));
})

// Copy a font to a new font
HOST_FUNCTION(uint32_t, font_copy, (uint32_t fontPtr), {
  pntr_font *font = fonts[fontPtr];
  return add_font(pntr_font_copy(font));
})

// Load a BMF font from a file in cart
HOST_FUNCTION(uint32_t, font_load_bmf, (uint32_t filenamePtr, uint32_t charactersPtr), {
  char *filename = copy_string_from_cart(filenamePtr);
  char *characters = copy_string_from_cart(charactersPtr);
  return add_font(pntr_load_font_bmf(filename, characters));
})

// Load a BMF font from an image
HOST_FUNCTION(uint32_t, font_load_bmf_from_image, (uint32_t imagePtr, uint32_t charactersPtr), {
  pntr_image *image = images[imagePtr];
  char *characters = copy_string_from_cart(charactersPtr);
  return add_font(pntr_load_font_bmf_from_image(image, characters));
})

// Load a TTF font from a file in cart
HOST_FUNCTION(uint32_t, font_load_ttf, (uint32_t filenamePtr, int32_t fontSize), {
  char *filename = copy_string_from_cart(filenamePtr);
  return add_font(pntr_load_font_ttf(filename, fontSize));
})

// Load a TTY font from a file in cart
HOST_FUNCTION(uint32_t, font_load_tty, (uint32_t filenamePtr, int32_t glyphWidth, int32_t glyphHeight, uint32_t charactersPtr), {
  char *filename = copy_string_from_cart(filenamePtr);
  char *characters = copy_string_from_cart(charactersPtr);
  return add_font(pntr_load_font_tty(filename, glyphWidth, glyphHeight, characters));
})

// Load a TTY font from an image
HOST_FUNCTION(uint32_t, font_load_tty_from_image, (uint32_t imagePtr, int32_t glyphWidth, int32_t glyphHeight, uint32_t charactersPtr), {
  pntr_image *image = images[imagePtr];
  char *characters = copy_string_from_cart(charactersPtr);
  return add_font(pntr_load_font_tty_from_image(image, glyphWidth, glyphHeight, characters));
})

// Scale a font, return a new font
HOST_FUNCTION(uint32_t, font_scale, (uint32_t fontPtr, float scaleX, float scaleY, uint32_t filter), {
  pntr_font *font = fonts[fontPtr];
  return add_font(pntr_font_scale(font, scaleX, scaleY, filter));
})

// Unload a font
HOST_FUNCTION(void, font_unload, (uint32_t fontPtr), {
  pntr_font *font = fonts[fontPtr];
  pntr_unload_font(font);
})

// Measure the size of some text
HOST_FUNCTION(uint32_t, text_measure, (uint32_t fontPtr, uint32_t textPtr), {
  pntr_font *font = fonts[fontPtr];
  char *text = copy_string_from_cart(textPtr);
  pntr_vector dim = pntr_measure_text_ex(font, text, strlen(text));
  return copy_memory_to_cart(&dim, sizeof(dim));
})

// Blend 2 colors together
HOST_FUNCTION(uint32_t, color_alpha_blend, (uint32_t dstPtr, uint32_t srcPtr), {
  pntr_color dst = copy_color_from_cart(dstPtr);
  pntr_color src = copy_color_from_cart(srcPtr);
  return copy_color_to_cart(pntr_color_alpha_blend(dst, src));
})

// Interpolate colors
HOST_FUNCTION(uint32_t, color_bilinear_interpolate, (uint32_t color00Ptr, uint32_t color01Ptr, uint32_t color10Ptr, uint32_t color11Ptr, float coordinateX, float coordinateY), {
  pntr_color color00 = copy_color_from_cart(color00Ptr);
  pntr_color color01 = copy_color_from_cart(color01Ptr);
  pntr_color color10 = copy_color_from_cart(color10Ptr);
  pntr_color color11 = copy_color_from_cart(color11Ptr);
  return copy_color_to_cart(pntr_color_bilinear_interpolate(color00, color01, color10, color11, coordinateX, coordinateY));
})

// Change the brightness of a color
HOST_FUNCTION(uint32_t, color_brightness, (uint32_t colorPtr, float factor), {
  pntr_color color = copy_color_from_cart(colorPtr);
  return copy_color_to_cart(pntr_color_brightness(color, factor));
})

// Change contrast of a color
HOST_FUNCTION(uint32_t, color_contrast, (uint32_t colorPtr, float contrast), {
  pntr_color color = copy_color_from_cart(colorPtr);
  return copy_color_to_cart(pntr_color_contrast(color, contrast));
})

// Fade a color
HOST_FUNCTION(uint32_t, color_fade, (uint32_t colorPtr, float alpha), {
  pntr_color color = copy_color_from_cart(colorPtr);
  return copy_color_to_cart(pntr_color_fade(color, alpha));
})

// Invert a color
HOST_FUNCTION(uint32_t, color_invert, (uint32_t colorPtr), {
  pntr_color color = copy_color_from_cart(colorPtr);
  return copy_color_to_cart(pntr_color_invert(color));
})

// Tint a color with another color
HOST_FUNCTION(uint32_t, color_tint, (uint32_t colorPtr, uint32_t tintPtr), {
  pntr_color color = copy_color_from_cart(colorPtr);
  pntr_color tint = copy_color_from_cart(tintPtr);
  return copy_color_to_cart(pntr_color_tint(color, tint));
})
