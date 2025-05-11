import loadHost from "../wbuild/host/null0.mjs";

export const host = await loadHost({ noInitialRun: true });
host.cart = { memory: new WebAssembly.Memory({ initial: 1 }) };

export const mem = new DataView(host.cart.memory.buffer);

const encoder = new TextEncoder();
const decoder = new TextDecoder();

// set a string in cart-mem
mem.setString = (p, s) => {
  let i = 0;
  for (const b of encoder.encode(s)) {
    mem.setUint8(i++ + p, b);
  }
  mem.setUint8(i++ + p, 0);
};

// get a string from cart-mem
mem.getString = (p) => {
  let i = 0;
  const out = new Uint8Array(1024);
  while (i++ < 1024) {
    const b = mem.getUint8(p + i);
    if (b === 0) {
      out[i] = b;
    }
  }
  return decoder.decode(out);
};

// setup some basic color pointers in cart-mem

export const BLACK = 0;
mem.setInt32(BLACK, 0x000000ff, true);

export const RED = 4;
mem.setInt32(RED, 0xff0000ff, true);

export const GREEN = 8;
mem.setInt32(GREEN, 0x00ff00ff, true);

export const BLUE = 12;
mem.setInt32(BLUE, 0x0000ffff, true);

// safe (ish) place to start putting values in cart-mem
export const ptr = 16;

// create a fake screen to draw on (at id 0)
host._host_image_new(50, 50, BLACK);

const s = host.HEAP32[host._host_get_image(0) / 4];
export const screen = host.HEAPU8.subarray(s, s + 50 * 50 * 4);
