#include "window/detail/input_sdl.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_clipboard.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

#include <unordered_map>

#include "debug/Logger.hpp"

static debug::Logger logger("input");

std::string input_util::to_string(Keycode code) {
    return {};
}

input_sdl::input_sdl() {
}

void input_sdl::pollEvents() {
    /* static */ SDL_Event event;
    while (SDL_PollEvent(&event)) {
    
    }
    // delta.x = 0.0f;
    // delta.y = 0.0f;
    // scroll = 0;
    // currentFrame++;
    // codepoints.clear();
    // pressedKeys.clear();
    // glfwPollEvents();

    // for (auto& [_, binding] : bindings.getAll()) {
    //     if (!binding.enabled) {
    //         binding.state = false;
    //         continue;
    //     }
    //     binding.justChanged = false;

    //     bool newstate = false;
    //     switch (binding.type) {
    //         case InputType::KEYBOARD:
    //             newstate = pressed(static_cast<Keycode>(binding.code));
    //             break;
    //         case InputType::MOUSE:
    //             newstate = clicked(static_cast<Mousecode>(binding.code));
    //             break;
    //     }

    //     if (newstate) {
    //         if (!binding.state) {
    //             binding.state = true;
    //             binding.justChanged = true;
    //             binding.onactived.notify();
    //         }
    //     } else {
    //         if (binding.state) {
    //             binding.state = false;
    //             binding.justChanged = true;
    //         }
    //     }
    // }
}

const char* input_sdl::getClipboardText() const {
    return SDL_GetClipboardText();
}

void input_sdl::setClipboardText(const char* text) {
    SDL_SetClipboardText(text);
}

int input_sdl::getScroll() {
    return {};
}

bool input_sdl::pressed(Keycode key) const {
    return {};
}
bool input_sdl::jpressed(Keycode keycode) const {
    return {};
}

bool input_sdl::clicked(Mousecode code) const {
    return {};
}
bool input_sdl::jclicked(Mousecode code) const {
    return {};
}

CursorState input_sdl::getCursor() const {
    return {isCursorLocked(), {}, {}};
}

bool input_sdl::isCursorLocked() const {
    return {};
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
    return {};
}

const std::vector<Keycode>& input_sdl::getPressedKeys() const {
    return pressedKeys;
}

const std::vector<uint>& input_sdl::getCodepoints() const {
    return codepoints;
}
