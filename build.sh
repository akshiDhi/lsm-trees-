

emcc lsm_wasm.cpp -o lsm.js \
  -s EXPORTED_FUNCTIONS='["_put","_get","_flush_table","_compact_table"]' \
  -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s ENVIRONMENT=web

echo "Build complete. Place lsm.js and lsm.wasm next to index.html and serve via HTTP."
