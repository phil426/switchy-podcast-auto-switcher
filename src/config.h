#pragma once
#include <string>
#include <vector>
#include "switch-engine.h"

class Config {
public:
    Config(); ~Config();
    std::vector<CamMapping> get_mappings() const { return mappings_; }
    void set_mappings(const std::vector<CamMapping> &m) { mappings_ = m; }
    Responsiveness get_responsiveness() const { return responsiveness_; }
    void set_responsiveness(Responsiveness r) { responsiveness_ = r; }
    int get_hold_time_ms() const { return hold_time_ms_; }
    void set_hold_time_ms(int ms) { hold_time_ms_ = ms; }
    std::string get_fallback_scene() const { return fallback_scene_; }
    void set_fallback_scene(const std::string &s) { fallback_scene_ = s; }
    bool get_transition_fade() const { return transition_fade_; }
    void set_transition_fade(bool f) { transition_fade_ = f; }
    int get_fade_duration_ms() const { return fade_duration_ms_; }
    void set_fade_duration_ms(int ms) { fade_duration_ms_ = ms; }
    void load(); void save() const;
private:
    std::vector<CamMapping> mappings_;
    Responsiveness responsiveness_ = Responsiveness::Neutral;
    int hold_time_ms_ = 800;
    std::string fallback_scene_;
    bool transition_fade_ = false;
    int fade_duration_ms_ = 300;
    std::string get_config_path() const;
};
