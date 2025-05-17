import { WasiPreview1 } from "@easywasm/wasi";

// this is the emscripten-exported function for host
import loadHost from "./host/null0.mjs";
import PhysFS from "./PhysFS.js";

const debug = false;

export default async function loadCart(cartUrl, canvas) {
  const cartName = cartUrl.split("/").pop();
  return await loadHost({
    canvas,
    arguments: [cartName],
    preRun(h) {
      h.FS.createPreloadedFile("", cartName, cartUrl, true, false);

      // this is called on each frame by emscripten-code
      h.cart_callback_update = (screenPtr) => {
        const screenSize = canvas.width * canvas.height * 4;
        h.screen.data.set(h.HEAPU8.subarray(screenPtr, screenPtr + screenSize));
        h.ctx.putImageData(h.screen, 0, 0);
      };

      // this is called at startup by emscripten-code
      h.cart_callback_init = (wasmBytesPtr, wasmSize) => {
        canvas.width = 640;
        canvas.height = 480;
        h.ctx = canvas.getContext("2d");
        h.screen = new ImageData(canvas.width, canvas.height);

        const wasmBytes = h.HEAPU8.subarray(
          wasmBytesPtr,
          wasmBytesPtr + wasmSize,
        );

        const fs = new PhysFS();
        const wasi_snapshot_preview1 = new WasiPreview1({ fs });
        const imports = { wasi_snapshot_preview1, null0: {} };

        // expose host-functions that are named _host_whatever as null0.whatever
        for (const f of Object.keys(h)) {
          if (f.startsWith("_host_")) {
            const func = f.replace(/^_host_/, "");
            imports.null0[func] = (...args) => {
              if (debug) {
                console.log(func, args);
              }
              return h[f](...args);
            };
          }
        }

        WebAssembly.instantiate(wasmBytes, imports).then(({ instance }) => {
          h.cart = instance.exports;
          fs.wasm = h;
          wasi_snapshot_preview1.start(h.cart);
          if (h.cart.load) {
            h.cart.load();
          }
        });
        return true;
      };
    },
  });
}
