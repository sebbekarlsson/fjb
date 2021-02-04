import babel from "rollup-plugin-babel";
import resolve from "rollup-plugin-node-resolve";
import commonjs from "rollup-plugin-commonjs";
import json from "@rollup/plugin-json";
import autoNamedExports from 'rollup-plugin-auto-named-exports';
import exportDefault from 'rollup-plugin-export-default';



export default {
  input: "./src/main.js",
  output: {
    file: "./dist.js",
    format: "iife",
    name: "bundle",
    globals: {
      lodash: "_",
    },
  },
  plugins: [
    babel({
      exclude: "node_modules/**",
    }),
    resolve(),
    commonjs(),
    json(),
    autoNamedExports(),
    exportDefault(),
  ],
};
