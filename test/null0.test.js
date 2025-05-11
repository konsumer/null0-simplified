import { test } from "node:test";

import { host, getScreen, mem, ptr, BLACK, RED, BLUE, GREEN } from "./host.js";

const screen = getScreen();

test("50x50 screen", ({ assert }) => {
  assert.equal(screen.length, 50 * 50 * 4);
});

test("clear", ({ assert }) => {
  host._host_clear(0, RED);

  // all bytes are set to [255,0,0,255]
  for (let p in screen) {
    const b = p % 4;
    if (b === 0) {
      assert.equal(screen[p], 255);
    }
    if (b === 1) {
      assert.equal(screen[p], 0);
    }
    if (b === 2) {
      assert.equal(screen[p], 0);
    }
    if (b === 3) {
      assert.equal(screen[p], 255);
    }
  }
});
