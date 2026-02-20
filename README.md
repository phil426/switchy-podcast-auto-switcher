# Switchy — The Podcast Auto Camera Switcher

> **Switchy** automatically switches OBS Studio scenes based on who's speaking — just like the **RØDECaster Video** Auto Switching feature, but for everyone.

![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)
![OBS Studio 30+](https://img.shields.io/badge/OBS%20Studio-30%2B-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey)

---

## ✨ Features

| Feature | Description |
|---|---|
| **Mic → Camera Mapping** | Link any OBS audio source to any OBS scene |
| **Priority Levels** | Low / Medium / High bias — give your host cam priority |
| **Responsiveness Modes** | Relaxed 🐢 / Neutral / Fast ⚡ EMA smoothing |
| **Hold Time** | Minimum time a camera stays active (no flickering) |
| **Noise Gate** | Per-mic dBFS threshold — ignore background noise |
| **Fallback Scene** | Auto-switch to a wide shot when no one speaks |
| **Fade / Cut Transition** | Optional fade with configurable duration |
| **Live VU Meters** | Real-time audio levels in the OBS dock |
| **One-click Toggle** | Big green ON button in the dock — just like the RODECaster |

---

## 🎬 How It Works

Every ~10ms, Switchy:
1. Reads the **RMS audio level** of each mapped microphone
2. Applies **Exponential Moving Average** smoothing (controlled by Responsiveness)
3. Adds a **priority bias** (±6 dB) to favour preferred cameras
4. Switches to the loudest speaker's scene — respecting the **hold time**
5. Falls back to a wide shot when everyone goes silent

---

## 📸 UI Overview

**Dock Panel**
```
┌─────────────────────────────────┐
│  🎥 Switchy               [ON] │
├─────────────────────────────────┤
│  Mic 1 ───┤██████░░░░│→ Host   │
│  Mic 2 ───┤███░░░░░░░│→ Guest  │
│  Mic 3 ───┤░░░░░░░░░░│→ Cam C  │
├─────────────────────────────────┤
│  Responsiveness: [Neutral  ▼]  │
│  [⚙ Open Settings]             │
└─────────────────────────────────┘
```

**Settings Dialog** — map each mic to a scene, set thresholds, pick responsiveness, hold time, fallback scene, and transition type.

---

## 🔧 Requirements

- **OBS Studio** ≥ 30.0.0
- A C++17 compiler (MSVC 2019+ / Xcode 14+ / GCC 11+)
- CMake ≥ 3.16
- Qt 6 (or Qt 5 fallback) — bundled with OBS

---

## 🚀 Building

```bash
git clone https://github.com/phil426/switchy-podcast-auto-switcher.git
cd switchy-podcast-auto-switcher

cmake -B build -S . \
  -DCMAKE_BUILD_TYPE=Release \
  -Dlibobs_DIR=/path/to/obs-studio/build

cmake --build build --config Release
cmake --install build
```

The plugin is auto-installed to your OBS plugins directory.

---

## 🎙️ Quick Start

1. Open OBS → find the **Switchy** dock (View → Docks)
2. Click **⚙ Open Settings**
3. Add a mapping per speaker:
   - **Audio Source** → microphone
   - **Scene / Camera** → OBS scene to cut to
   - **Priority** → High for close-ups, Low for wide shot
   - **Threshold** → silence gate (default −40 dB)
4. Set a **Fallback Scene** (e.g. wide shot)
5. Hit **● Enable** — Switchy takes over 🎉

---

## 📁 Project Structure

```
switchy-podcast-auto-switcher/
├── CMakeLists.txt
├── buildspec.json
├── src/
│   ├── plugin-main.cpp         OBS entry point
│   ├── audio-monitor.cpp/h     Audio tap → dBFS callback
│   ├── switch-engine.cpp/h     EMA + priority + hold time logic
│   ├── config.cpp/h            JSON persistence
│   ├── utils.h                 dBFS math + EMA helper
│   └── ui/
│       ├── dock-widget.cpp/h   OBS dock panel
│       └── settings-dialog.cpp/h  Settings UI
└── data/locale/en-US.ini
```

---

## 📄 License

MIT © 2026 phil426

Inspired by the [RØDECaster Video](https://rode.com/en-us/products/rodecaster-video) Auto Switching feature.
