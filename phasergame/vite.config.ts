import { defineConfig } from 'vite';

export default defineConfig({
  server: {
    port: 5174,
    open: false,
  },
  build: {
    assetsInlineLimit: 0,
  },
  test: {
    globals: true,
    environment: 'node',
  },
});
