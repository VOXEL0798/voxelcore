#include "input.hpp"

#include <unordered_map>

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>

#include "debug/Logger.hpp"
#include "data/dv.hpp"
#include "util/stringutil.hpp"

#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

static debug::Logger logger("input");

static std::unordered_map<std::string, std::uint64_t> keycodes {
    {"enter", SDLK_RETURN },
    {"space", SDLK_SPACE },
    {"backspace", SDLK_BACKSPACE },
    {"caps-lock", SDLK_CAPSLOCK },
    {"escape", SDLK_ESCAPE },
    {"delete", SDLK_DELETE },
    {"home", SDLK_HOME },
    {"end", SDLK_END },
    {"tab", SDLK_TAB },
    {"insert", SDLK_INSERT },
    {"page-down", SDLK_PAGEDOWN },
    {"page-up", SDLK_PAGEUP },
    {"left-shift", SDLK_LSHIFT },
    {"right-shift", SDLK_RSHIFT },
    {"left-ctrl", SDLK_LCTRL },
    {"right-ctrl", SDLK_RCTRL},
    {"left-alt", SDLK_LALT },
    {"right-alt", SDLK_RALT },
    {"left-super", 0 /*todo*/ },
    {"right-super", 0 /*todo*/ },
    {"grave-accent", SDLK_GRAVE },
    {"left", SDLK_LEFT },
    {"right", SDLK_RIGHT },
    {"down", SDLK_DOWN },
    {"up", SDLK_UP },
};
SDL_Event s;
static std::unordered_map<std::string, std::uint32_t> mousecodes {
    {"left", SDL_BUTTON_LEFT },
    {"right", SDL_BUTTON_RIGHT },
    {"middle", SDL_BUTTON_MIDDLE },
    {"side1", SDL_BUTTON_MASK(4) /*todo*/},
    {"side2", SDL_BUTTON_MASK(5) /*todo*/},
    {"side3", SDL_BUTTON_MASK(6) /*todo*/},
    {"side4", SDL_BUTTON_MASK(7) /*todo*/},
    {"side5", SDL_BUTTON_MASK(8) /*todo*/},
};

static std::unordered_map<int, std::string> keynames {};
static std::unordered_map<int, std::string> buttonsnames{};

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

void input_util::initialize() {
    for (std::uint32_t i = 0; i <= 9; i++) {
        keycodes[std::to_string(i)] = SDLK_0 + i;
    }
    for (std::uint32_t i = 0; i < 25; i++) {
        keycodes["f" + std::to_string(i + 1)] = SDLK_F1 + i;
    }
    for (char i = 'a'; i <= 'z'; i++) {
        keycodes[std::string({i})] = SDLK_A - 'a' + i;
    }
    for (const auto& entry : keycodes) {
        keynames[entry.second] = entry.first;
    }
    for (const auto& entry : mousecodes) {
        buttonsnames[entry.second] = entry.first;
    }
}

Keycode input_util::keycode_from(const std::string& name) {
    const auto& found = keycodes.find(name);
    if (found == keycodes.end()) {
        return Keycode::UNKNOWN;
    }
    return static_cast<Keycode>(found->second);
}

Mousecode input_util::mousecode_from(const std::string& name) {
    const auto& found = mousecodes.find(name);
    if (found == mousecodes.end()) {
        return Mousecode::UNKNOWN;
    }
    return static_cast<Mousecode>(found->second);
}

std::string input_util::to_string(Keycode code) {
    SDL_Scancode scancode = static_cast<SDL_Scancode>(code);
    switch (scancode) {
        case SDLK_TAB:
            return "Tab";
        case SDL_SCANCODE_LCTRL:
            return "Left Ctrl";
        case SDL_SCANCODE_RCTRL:
            return "Right Ctrl";
        case SDL_SCANCODE_LALT:
            return "Left Alt";
        case SDL_SCANCODE_RALT:
            return "Right Alt";
        case SDL_SCANCODE_LSHIFT:
            return "Left Shift";
        case SDL_SCANCODE_RSHIFT:
            return "Right Shift";
        case SDL_SCANCODE_CAPSLOCK:
            return "Caps-Lock";
        case SDL_SCANCODE_SPACE:
            return "Space";
        case SDL_SCANCODE_ESCAPE:
            return "Esc";
        case SDL_SCANCODE_RETURN:
            return "Enter";
        case SDL_SCANCODE_UP:
            return "Up";
        case SDL_SCANCODE_DOWN:
            return "Down";
        case SDL_SCANCODE_LEFT:
            return "Left";
        case SDL_SCANCODE_RIGHT:
            return "Right";
        case SDL_SCANCODE_BACKSPACE:
            return "Backspace";
        case SDL_SCANCODE_F1:
            return "F1";
        case SDL_SCANCODE_F2:
            return "F2";
        case SDL_SCANCODE_F3:
            return "F3";
        case SDL_SCANCODE_F4:
            return "F4";
        case SDL_SCANCODE_F5:
            return "F5";
        case SDL_SCANCODE_F6:
            return "F6";
        case SDL_SCANCODE_F7:
            return "F7";
        case SDL_SCANCODE_F8:
            return "F8";
        case SDL_SCANCODE_F9:
            return "F9";
        case SDL_SCANCODE_F10:
            return "F10";
        case SDL_SCANCODE_F11:
            return "F11";
        case SDL_SCANCODE_F12:
            return "F12";
        case SDL_SCANCODE_DELETE:
            return "Delete";
        case SDL_SCANCODE_HOME:
            return "Home";
        case SDL_SCANCODE_END:
            return "End";
        case SDL_SCANCODE_LGUI:
            return "Left Super";
        case SDL_SCANCODE_RGUI:
            return "Right Super";
        case SDL_SCANCODE_PAGEUP:
            return "Page Up";
        case SDL_SCANCODE_PAGEDOWN:
            return "Page Down";
        case SDL_SCANCODE_INSERT:
            return "Insert";
        case SDL_SCANCODE_PRINTSCREEN:
            return "Print Screen";
        case SDL_SCANCODE_NUMLOCKCLEAR:
            return "Num Lock";
        case SDL_SCANCODE_APPLICATION:
            return "Menu";
        case SDL_SCANCODE_PAUSE:
            return "Pause";
        default:
            return "Unknown";
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
        case Mousecode::BUTTON_6:
        case Mousecode::BUTTON_7:
        case Mousecode::BUTTON_8:
            return "XButton " + std::to_string(static_cast<int>(code) - 
                static_cast<int>(Mousecode::BUTTON_3));
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
                logger.error()
                    << "unknown input type: " << prefix << " (binding "
                    << util::quote(key) << ")";
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

#include "coders/toml.hpp"

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
