#ifdef EMSCRIPEN
#include <emscripten.h>
#endif

#include "fs.h"
#include "host.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static bool keepgoing = true;
static void emscripten_cart_update() {
  if (keepgoing) {
    keepgoing = cart_update();
  }
}

int main(int argc, char *argv[]) {
  unsigned char *fileData = NULL;
  unsigned int bytesRead = 0;
  unsigned int wasmSize = 0;
  unsigned char *wasmBytes = NULL;

  if (argc < 2) {
    printf("Error: No input file specified\n");
    printf("Usage: %s <CART>\n", argv[0]);
    return 1;
  }

  fileData = fs_load_file_real(argv[1], &bytesRead);

  if (fileData == NULL) {
    printf("Error: Could not load cart '%s'\n", argv[1]);
    return 1;
  }

  if (!PHYSFS_init("/")) {
    fprintf(stderr, "Could not start filesystem.\n");
    return false;
  }

  DetectFileType cartType = fs_parse_magic_bytes(*(uint32_t *)fileData);

  if (cartType == FILE_TYPE_ZIP) {
    if (!PHYSFS_mountMemory(fileData, bytesRead, NULL, "cart.zip", NULL, 1)) {
      PHYSFS_deinit();
      free(fileData);
      fprintf(stderr, "Could not mount cart.\n");
      return 1;
    }
    wasmBytes = fs_load_file("main.wasm", &wasmSize);
    if (wasmSize == 0) {
      free(fileData);
      fprintf(stderr, "Could not load main.wasm\n");
      return 1;
    }
  } else if (cartType == FILE_TYPE_WASM) {
    wasmSize = bytesRead;
    wasmBytes = fileData;
  } else {
    free(fileData);
    fprintf(stderr, "Only wasm/zip cart-files are supported.\n");
    return 1;
  }

  // load screen on image 0
  add_image(pntr_gen_image_color(640, 480, PNTR_BLACK));
  add_font(pntr_load_font_default());

  printf("main: setup screen & default-font\n");

  if (!cart_init(wasmBytes, wasmSize)) {
    free(fileData);
    printf("Error: Could not start cart '%s'\n", argv[1]);
    return 1;
  }
  free(fileData);
  free(wasmBytes);

#ifdef EMSCRIPTEN
  emscripten_set_main_loop(emscripten_cart_update, 0, 1);
#else
  while (cart_update()) {
  }
#endif

  cart_close();
  return 0;
}
