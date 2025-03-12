import { eq } from "../../global.ts";
import type { PassConfig } from "../../global.ts";

type setting = keyof PassConfig;
type settingVal = PassConfig[setting];

export function update(
  pass: "low" | "high",
  channel: number,
  settingKey: setting,
  settingValue: settingVal,
) {
  let key = pass === "low" ? "lowpass" : "highpass";
  let parentObject = eq.value[channel][key];
  parentObject[settingKey] = settingValue;
  eq.set({
    ...eq.value,
    [key]: parentObject,
  });
  window.dispatchEvent(
    new CustomEvent("update", {
      detail: { key: settingKey, value: settingValue, type: key, channel },
    }),
  );
}

export function toLog(position: number) {
  // position will be between 0 and 100
  var minp = 10;
  var maxp = 20000;

  // The result should be between 100 an 10000000
  var minv = Math.log(10);
  var maxv = Math.log(20000);

  // calculate adjustment factor
  var scale = (maxv - minv) / (maxp - minp);

  return Math.exp(minv + scale * (position - minp));
}

export function fromLog(value: number) {
  // value will be between 100 and 10000000
  var minp = 10;
  var maxp = 20000;

  var minv = Math.log(10);
  var maxv = Math.log(20000);

  // calculate adjustment factor
  var scale = (maxp - minp) / (maxv - minv);

  return minp + scale * (Math.log(value) - minv);
}

export function rangeChangedLog(valInput: HTMLInputElement) {
  return (e: Event) => {
    let rawValue = (e.target as HTMLInputElement).value;
    let value = toLog(parseInt(rawValue)).toFixed(0);
    valInput.value = value;
    return value;
  };
}

export function valueChangedLog(rangeInput: HTMLInputElement) {
  return (e: Event) => {
    let rawValue = (e.target as HTMLInputElement).value;
    let value = fromLog(parseInt(rawValue)).toString();
    rangeInput.value = value;
    return rawValue;
  };
}

export function valueChanged(input: HTMLInputElement) {
  return (e: Event) => {
    let value = (e.target as HTMLInputElement).value;
    input.value = value;
    return value;
  };
}
