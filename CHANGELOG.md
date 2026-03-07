# Changelog

All notable changes to Podswitch will be documented here.
Format based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]

### Added
- Initial plugin implementation inspired by RØDECaster Video Auto Switching
- `AudioMonitor`: per-source OBS audio capture tap with RMS → dBFS conversion
- `SwitchEngine`: EMA-smoothed level tracking with priority bias and hold time
- `Config`: JSON settings persistence via `obs_data` to OBS config directory
- Qt dock panel with live gradient VU meters and green/grey enable toggle
- Full settings dialog with drag-and-drop-style mapping table
- 3 responsiveness modes: Relaxed (α=0.05), Neutral (α=0.15), Fast (α=0.40)
- Priority levels: Low (−6 dB), Medium (0 dB), High (+6 dB)
- Per-mapping noise threshold gate (default −40 dBFS)
- Fallback scene when no speaker is active
- Optional fade transition with configurable duration
- GitHub Actions CI for Windows, macOS, and Linux builds
- Cross-platform CMakeLists.txt (requires OBS Studio ≥ 30.0.0)

## [1.0.0] — 2026-02-19
- First public release 🎙️
