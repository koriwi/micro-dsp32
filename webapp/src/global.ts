import { atom } from "nanostores";

export interface PassConfig {
  order: number;
  q: number;
  freq: number;
}
export interface EQConfig {
  lowpass: PassConfig;
  highpass: PassConfig;
  [pass: string]: PassConfig | undefined;
}
export interface DeviceConfig {
  name: string;
  eq: EQConfig[];
}
export const deviceConfig = atom<DeviceConfig>({
  name: "",
  eq: [
    {
      lowpass: {
        order: 1,
        q: 0.7,
        freq: 1000,
      },
      highpass: {
        order: 1,
        q: 0.7,
        freq: 1000,
      },
    },
    {
      lowpass: {
        order: 1,
        q: 0.7,
        freq: 1000,
      },
      highpass: {
        order: 1,
        q: 0.7,
        freq: 1000,
      },
    },
  ],
});
