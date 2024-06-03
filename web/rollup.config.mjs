import commonjs from '@rollup/plugin-commonjs';
import { nodeResolve } from "@rollup/plugin-node-resolve";
import postcss from "rollup-plugin-postcss";
import tailwindcss from "tailwindcss";
import autoprefixer from "autoprefixer";
import tla from 'rollup-plugin-tla';

export default {
  input: "./web/src/js/index.js",
  output: {
    file: "./web/voodoo.bundle.js",
    format: "es",
    name: "voodoo",
  },
  plugins: [
    commonjs({transformMixedEsModules: true}),
    nodeResolve(),
    postcss({
      plugins: [
        tailwindcss({
          content: ["./web/index.html"],
          theme: {
            extend: {},
          },
          plugins: [],
        }),
        autoprefixer({}),
      ],
    }),
    tla()
  ],
};
