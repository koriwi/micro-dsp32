import type { ChartConfiguration, TooltipItem } from "chart.js";

export const chartData = (dataChannel1: any, dataChannel2: any) => {
  return {
    data: {
      datasets: [
        {
          data: dataChannel1,
          label: "Channel 1",
          fill: "start",
          tension: 0.33,
          backgroundColor: "#ff000055",
          borderColor: "#ffffff",
        },
        {
          data: dataChannel2,
          label: "Channel 2",
          fill: "start",
          tension: 0.33,
          backgroundColor: "#00ff0055",
          borderColor: "#ffffff",
        },
      ],
    },
    type: "line",
    options: {
      animation: {
        duration: 250,
      },
      responsive: true,
      maintainAspectRatio: false,
      plugins: {
        legend: {
          labels: {
            color: "#fff", // Legend text color
          },
        },
        tooltip: {
          backgroundColor: "rgba(0, 0, 0, 0.8)", // Tooltip background
          titleColor: "#fff", // Tooltip title color
          bodyColor: "#fff", // Tooltip body color
          callbacks: {
            title: (item: TooltipItem<"line">[]) => (
              console.log(item), item[0].parsed.x.toFixed(0) + " Hz"
            ),
            label: (item: TooltipItem<"line">) =>
              item.parsed.y.toFixed(1) + " dB",
          },
        },
      },
      scales: {
        x: {
          ticks: {
            color: "#fff", // X-axis label color
          },
          type: "logarithmic",
          min: 10,
          title: { display: true, text: "Frequency (Hz)", color: "#fff" },
          max: 20000,
          grid: {
            color: "rgba(255, 255, 255, 0.1)", // X-axis grid lines
          },
        },
        y: {
          ticks: {
            color: "#fff", // X-axis label color
          },
          min: -30,
          max: 10,
          type: "linear",
          axis: "y",
          title: { display: true, text: "Gain (dB)", color: "#fff" },
          grid: {
            color: "rgba(255, 255, 255, 0.1)", // X-axis grid lines
          },
        },
      },
      elements: {
        line: {
          borderWidth: 2, // Line thickness
        },
        point: {
          radius: 5, // Point radius
          backgroundColor: "#fff", // Point color
        },
      },
    },
  } as ChartConfiguration;
};
