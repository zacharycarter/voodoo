import {createSignal, createEffect, on} from "solid-js";

const create = createSignal;
export {create};

export function getter(signal) {
  return signal[0];
}

export function setter(signal) {
  return signal[1];
}

export function get(signal) {
  return signal[0]();
}

export function set(signal, value) {
  return signal[1](value);
}

export function update(signal, update) {
  return signal[1](update);
}

// should take an array of signals or accessors
export function onEffect(signals) {
  createEffect(on(() => { signals.forEach((x) => typeof x === 'function' ? x() : get(x)); }, f));
}