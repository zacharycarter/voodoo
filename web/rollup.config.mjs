import { nodeResolve } from "@rollup/plugin-node-resolve";
import postcss from "rollup-plugin-postcss";
import tailwindcss from "tailwindcss";
import autoprefixer from "autoprefixer";

export default {
  input: "./web/src/js/index.js",
  output: {
    file: "./web/voodoo.bundle.js",
    format: "iife",
    name: "voodoo",
  },
  plugins: [
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
  ],
};
