import { nodeResolve } from "@rollup/plugin-node-resolve";
import postcss from "rollup-plugin-postcss";
import tailwindcss from "tailwindcss";
import autoprefixer from "autoprefixer";

export default {
  input: "./src/js/index.js",
  output: {
    file: "./dist/voodoo.bundle.js",
    format: "iife",
    name: "voodoo",
  },
  plugins: [
    nodeResolve(),
    postcss({
      plugins: [
        tailwindcss({
          content: ["./index.html"],
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
