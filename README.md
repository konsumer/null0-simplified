This is a simplified version of [null0](https://github.com/konsumer/null0-pntr). The idea is that it's a game-engine where user-code (called cart) is in wasm. There is a web and native host that can run this cart.

There is no path/sound/input stuff (for now) to keep it simple. IO operations will be done with WASI.

## Libraries Used

- It uses [pntr](https://raw.githubusercontent.com/RobLoach/pntr/refs/heads/master/README.md) for drawing.

## Development Tasks

```sh
# build for web & run test-server (watches for changes)
npm start

# run unit-tests
npm test

# build web-host
npm run build:web

# build carts
npm run build:cart

# build native-host
npm run build:native
```
