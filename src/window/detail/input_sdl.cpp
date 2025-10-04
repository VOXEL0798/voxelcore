#include "window/detail/input_sdl.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_clipboard.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

#include "debug/Logger.hpp"
#include "window/input.hpp"

static debug::Logger logger("input");

std::string input_util::to_string(Keycode code) {
    int icode_repr = static_cast<int>(code);
    const char* name = SDL_GetKeyName(icode_repr);
    logger.info() << icode_repr << ": " << name;
    return std::string(name);
}

Keycode input_util::keycode_from(const std::string& name) {
    logger.info() << name << ": " << SDL_GetKeyFromName(name.c_str());
    return static_cast<Keycode>(SDL_GetKeyFromName(name.c_str()));
}

input_sdl::input_sdl() {
    input_util::initialize();
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
                break;
            case SDL_EVENT_KEY_DOWN:
                logger.info() << "Keyboard button: " << event.key.key;
                if (event.key.key >= keys_buffer_size) {
                    // Win key return 1073742051
                    break;
                }
                prevPressed = keys[event.key.key];
                keys[event.key.key] = true;
                frames[event.key.key] = currentFrame;
                if (!prevPressed) {
                    keyCallbacks[static_cast<Keycode>(event.key.key)].notify();
                }
                pressedKeys.push_back(static_cast<Keycode>(event.key.key));
                break;
            case SDL_EVENT_KEY_UP:
                if (event.key.key >= keys_buffer_size) {
                    // Win key return 1073742051
                    break;
                }
                keys[event.key.key] = false;
                frames[event.key.key] = currentFrame;
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
                cursor = {event.motion.x, event.motion.y};
                delta = {event.motion.xrel, event.motion.yrel};
                break;
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
    if (keys[keycode]) logger.info() << "lol";
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
    // cursorDrag = false;
    // if (cursorLocked) {
    //     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // } else {
    //     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // }
    // cursorLocked = !cursorLocked;
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

const std::vector<uint>& input_sdl::getCodepoints() const {
    return codepoints;
}
