import { test } from "node:test";

import { host, screen, mem, ptr, BLACK, RED, BLUE, GREEN } from "./host.js";

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

test("more complex drawing", ({ assert }) => {
  host._host_clear(0, BLACK);
  host._host_draw_rectangle(0, 10, 10, 40, 40, 0, BLUE);
  host._host_draw_rectangle(0, 20, 20, 30, 30, 0, GREEN);
  host._host_draw_rectangle(0, 30, 30, 20, 20, 0, RED);
  host._host_draw_circle(0, 25, 25, 20, 0, BLACK);
  assert.snapshot(screen);
});
