#include "window/input.hpp"

#include "coders/toml.hpp"
#include "debug/Logger.hpp"
#include "util/stringutil.hpp"

#include <SDL3/SDL_mouse.h>

debug::Logger logger("input");

static std::unordered_map<int, std::string> keynames {};
static std::unordered_map<int, std::string> buttonsnames {};

static std::unordered_map<std::string, int> mousecodes {
    {"left", SDL_BUTTON_LEFT},
    {"right", SDL_BUTTON_RIGHT},
    {"middle", SDL_BUTTON_MIDDLE},
    {"side1", SDL_BUTTON_X1},
    {"side2", SDL_BUTTON_X2},
};

std::string input_util::get_name(Mousecode code) {
    const auto found = buttonsnames.find(static_cast<int>(code));
    if (found == buttonsnames.end()) {
        return "unknown";
    }
    return found->second;
}

std::string input_util::get_name(Keycode code) {
    const auto found = keynames.find(static_cast<int>(code));
    if (found == keynames.end()) {
        return "unknown";
    }
    return found->second;
}

void Binding::reset(InputType type, int code) {
    this->type = type;
    this->code = code;
}

void Binding::reset(Keycode code) {
    reset(InputType::KEYBOARD, static_cast<int>(code));
}

void Binding::reset(Mousecode code) {
    reset(InputType::MOUSE, static_cast<int>(code));
}

Mousecode input_util::mousecode_from(const std::string& name) {
    return {};
}

void input_util::initialize() {
    // for (int i = 0; i <= 9; i++) {
    //     keycodes[std::to_string(i)] = GLFW_KEY_0 + i;
    // }
    // for (int i = 0; i < 25; i++) {
    //     keycodes["f" + std::to_string(i + 1)] = GLFW_KEY_F1 + i;
    // }
    // for (char i = 'a'; i <= 'z'; i++) {
    //     keycodes[std::string({i})] = GLFW_KEY_A - 'a' + i;
    // }
    // for (const auto& entry : keycodes) {
    //     keynames[entry.second] = entry.first;
    // }
    for (const auto& entry : mousecodes) {
        buttonsnames[entry.second] = entry.first;
    }
}

std::string input_util::to_string(Mousecode code) {
    switch (code) {
        case Mousecode::BUTTON_1:
            return "LMB";
        case Mousecode::BUTTON_2:
            return "RMB";
        case Mousecode::BUTTON_3:
            return "MMB";
        case Mousecode::BUTTON_4:
        case Mousecode::BUTTON_5:
            return "XButton " + std::to_string(
                                    static_cast<int>(code) -
                                    static_cast<int>(Mousecode::BUTTON_3)
                                );
        default:
            return "unknown button";
    }
}

const Binding& Bindings::require(const std::string& name) const {
    if (const auto found = get(name)) {
        return *found;
    }
    throw std::runtime_error("binding '" + name + "' does not exist");
}

Binding& Bindings::require(const std::string& name) {
    if (const auto found = get(name)) {
        return *found;
    }
    throw std::runtime_error("binding '" + name + "' does not exist");
}

void Bindings::read(const dv::value& map, BindType bindType) {
    for (auto& [sectionName, section] : map.asObject()) {
        for (auto& [name, value] : section.asObject()) {
            auto key = sectionName + "." + name;
            auto [prefix, codename] = util::split_at(value.asString(), ':');
            InputType type;
            int code;
            if (prefix == "key") {
                type = InputType::KEYBOARD;
                code = static_cast<int>(input_util::keycode_from(codename));
            } else if (prefix == "mouse") {
                type = InputType::MOUSE;
                code = static_cast<int>(input_util::mousecode_from(codename));
            } else {
                logger.error() << "unknown input type: " << prefix
                               << " (binding " << util::quote(key) << ")";
                continue;
            }
            if (bindType == BindType::BIND) {
                bind(key, type, code);
            } else if (bindType == BindType::REBIND) {
                rebind(key, type, code);
            }
        }
    }
}

std::string Bindings::write() const {
    auto obj = dv::object();
    for (auto& entry : bindings) {
        const auto& binding = entry.second;
        std::string value;
        switch (binding.type) {
            case InputType::KEYBOARD:
                value =
                    "key:" +
                    input_util::get_name(static_cast<Keycode>(binding.code));
                break;
            case InputType::MOUSE:
                value =
                    "mouse:" +
                    input_util::get_name(static_cast<Mousecode>(binding.code));
                break;
            default:
                throw std::runtime_error("unsupported control type");
        }
        obj[entry.first] = std::move(value);
    }
    return toml::stringify(obj);
}
