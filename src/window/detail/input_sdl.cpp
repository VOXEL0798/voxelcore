#include "window/detail/input_sdl.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_clipboard.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_video.h>

#include <cstring>

#include "debug/Logger.hpp"
#include "window/detail/window_sdl.hpp"
#include "window/input.hpp"

static debug::Logger logger("input");

std::string input_util::to_string(Keycode code) {
    auto icode_repr = static_cast<SDL_Scancode>(code);
    const char* name = SDL_GetKeyName(
        SDL_GetKeyFromScancode(icode_repr, SDL_KMOD_NONE, false)
    );
    return std::string(name);
}

Keycode input_util::keycode_from(const std::string& name) {
    return static_cast<Keycode>(SDL_GetScancodeFromName(name.c_str()));
}

input_sdl::input_sdl(window_sdl& window) : window(window) {
    input_util::initialize();
    // We should always get char stream (maybe)
    SDL_StartTextInput(window.getSdlWindow());
}

void input_sdl::pollEvents() {
    delta.x = 0.0f;
    delta.y = 0.0f;
    scroll = 0;
    currentFrame++;
    codepoints.clear();
    pressedKeys.clear();

    bool prevPressed = false;

    static SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                window.setShouldClose(true);
                break;
            case SDL_EVENT_KEY_DOWN:
                prevPressed = keys[event.key.scancode];
                keys[event.key.scancode] = true;
                frames[event.key.scancode] = currentFrame;
                if (!prevPressed) {
                    keyCallbacks[static_cast<Keycode>(event.key.scancode)]
                        .notify();
                }
                pressedKeys.push_back(static_cast<Keycode>(event.key.scancode));
                break;
            case SDL_EVENT_KEY_UP:
                keys[event.key.scancode] = false;
                frames[event.key.scancode] = currentFrame;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                prevPressed = keys[event.button.button + mouse_keys_offset];
                keys[event.button.button + mouse_keys_offset] = true;
                frames[event.button.button + mouse_keys_offset] = currentFrame;
                if (!prevPressed) {
                    keyCallbacks[static_cast<Keycode>(
                                     event.button.button + mouse_keys_offset
                                 )]
                        .notify();
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                keys[event.button.button + mouse_keys_offset] = false;
                frames[event.button.button + mouse_keys_offset] = currentFrame;
                break;
            case SDL_EVENT_MOUSE_MOTION:
                if (cursorDrag) {
                    delta += glm::vec2 {event.motion.xrel, event.motion.yrel};
                } else {
                    cursorDrag = true;
                }
                cursor = {event.motion.x, event.motion.y};
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                window.setSize({event.window.data1, event.window.data2});
                break;
            case SDL_EVENT_TEXT_INPUT:
                std::vector<char> vec(
                    event.text.text,
                    event.text.text + std::strlen(event.text.text)
                );
                std::copy(
                    vec.begin(), vec.end(), std::back_inserter(codepoints)
                );
        }
    }
    for (auto& [_, binding] : bindings.getAll()) {
        if (!binding.enabled) {
            binding.state = false;
            continue;
        }
        binding.justChanged = false;

        bool newstate = false;
        switch (binding.type) {
            case InputType::KEYBOARD:
                newstate = pressed(static_cast<Keycode>(binding.code));
                break;
            case InputType::MOUSE:
                newstate = clicked(static_cast<Mousecode>(binding.code));
                break;
        }
        if (newstate) {
            if (!binding.state) {
                binding.state = true;
                binding.justChanged = true;
                binding.onactived.notify();
            }
        } else {
            if (binding.state) {
                binding.state = false;
                binding.justChanged = true;
            }
        }
    }
}

const char* input_sdl::getClipboardText() const {
    return SDL_GetClipboardText();
}

void input_sdl::setClipboardText(const char* text) {
    SDL_SetClipboardText(text);
}

int input_sdl::getScroll() {
    return scroll;
}

bool input_sdl::pressed(Keycode key) const {
    int keycode = static_cast<int>(key);
    if (keycode < 0 || keycode >= keys_buffer_size) {
        return false;
    }
    return keys[keycode];
}
bool input_sdl::jpressed(Keycode keycode) const {
    return pressed(keycode) &&
           frames[static_cast<int>(keycode)] == currentFrame;
}

bool input_sdl::clicked(Mousecode code) const {
    return pressed(
        static_cast<Keycode>(mouse_keys_offset + static_cast<int>(code))
    );
}
bool input_sdl::jclicked(Mousecode code) const {
    return clicked(code) &&
           frames[static_cast<int>(code) + mouse_keys_offset] == currentFrame;
}

CursorState input_sdl::getCursor() const {
    return {isCursorLocked(), cursor, delta};
}

bool input_sdl::isCursorLocked() const {
    return cursorLocked;
}

void input_sdl::toggleCursor() {
    cursorDrag = false;
    cursorLocked = !cursorLocked;
    SDL_SetWindowRelativeMouseMode(window.getSdlWindow(), cursorLocked);
}

Bindings& input_sdl::getBindings() {
    return bindings;
}

const Bindings& input_sdl::getBindings() const {
    return bindings;
}

ObserverHandler input_sdl::addKeyCallback(Keycode key, KeyCallback callback) {
    return keyCallbacks[key].add(std::move(callback));
}

const std::vector<Keycode>& input_sdl::getPressedKeys() const {
    return pressedKeys;
}

const std::vector<char>& input_sdl::getCodepoints() const {
    return codepoints;
}
