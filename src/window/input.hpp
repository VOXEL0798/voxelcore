#pragma once

#include <cstdint>
#include <glm/vec2.hpp>
#include <string>

#include "util/HandlersList.hpp"


namespace dv {
    class value;
}

enum class BindType { BIND = 0, REBIND = 1 };

/// @brief Represents sdl scancode values.
enum class Keycode : std::int32_t {
    SPACE = 0x00000020u,
    APOSTROPHE = 0x00000027u,
    COMMA = 0x0000002cu,
    MINUS = 0x0000002du,
    PERIOD = 0x0000002eu,
    SLASH = 0x0000002fu,
    NUM_0 = 0x40000059u,
    NUM_1,
    NUM_2,
    NUM_3,
    NUM_4,
    NUM_5,
    NUM_6,
    NUM_7,
    NUM_8,
    NUM_9,
    SEMICOLON = 0x0000003bu,
    EQUAL = 0x0000003du,
    A = 0x00000061u,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    LEFT_BRACKET = 0x0000005bu,
    BACKSLASH = 0x0000005cu,
    RIGHT_BRACKET = 0x0000005du,
    GRAVE_ACCENT = 0x00000060u,
    ESCAPE = 0x0000001bu,
    ENTER = 0x0000000du,
    TAB = 0x00000009u,
    BACKSPACE = 0x00000008u,
    INSERT = 0x40000049u,
    DELETE = 0x0000007fu,
    LEFT = 0x40000050u,
    RIGHT = 0x4000004fu,
    DOWN = 0x40000051u,
    UP = 0x40000052u,
    PAGE_UP = 0x4000004bu,
    PAGE_DOWN = 0x4000004eu,
    HOME = 0x4000004au,
    END = 0x4000004du,
    CAPS_LOCK = 0x40000039u,
    NUM_LOCK = 0x40000053u,
    PRINT_SCREEN = 0x40000046u,
    PAUSE = 0x40000048u,
    F1 = 0x4000003au,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    LEFT_SHIFT = 0x400000e1u,
    LEFT_CONTROL = 0x400000e0u,
    LEFT_ALT = 0x400000e2u,
    // LEFT_SUPER = 343,
    RIGHT_SHIFT = 0x400000e5u,
    RIGHT_CONTROL = 0x400000e4u,
    RIGHT_ALT = 0x400000e6u,
    // RIGHT_SUPER = 347,
    MENU = 0x40000076u,
    UNKNOWN = 0x00000000u
};

/// @brief Represents sdl mouse button IDs.
/// @details There is a subset of sdl mouse button IDs.
enum class Mousecode : int {
    BUTTON_1 = 0,  // Left mouse button
    BUTTON_2 = 1,  // Right mouse button
    BUTTON_3 = 2,  // Middle mouse button
    BUTTON_4 = 3,  // Side mouse button 1
    BUTTON_5 = 4,  // Side mouse button 2
    UNKNOWN = -1,
};

inline Mousecode mousecodes_all[] {
    Mousecode::BUTTON_1,
    Mousecode::BUTTON_2,
    Mousecode::BUTTON_3,
    Mousecode::BUTTON_4,
    Mousecode::BUTTON_5,
};

namespace input_util {
    void initialize();
    Keycode keycode_from(const std::string& name);
    Mousecode mousecode_from(const std::string& name);

    /// @return Key label by keycode
    std::string to_string(Keycode code);
    /// @return Mouse button label by keycode
    std::string to_string(Mousecode code);

    /// @return Key name by keycode
    std::string get_name(Keycode code);
    /// @return Mouse button name by keycode
    std::string get_name(Mousecode code);
}

enum class InputType {
    KEYBOARD,
    MOUSE,
};

struct Binding {
    util::HandlersList<> onactived;

    InputType type;
    int code;
    bool state = false;
    bool justChanged = false;
    bool enabled = true;

    Binding() = default;
    Binding(InputType type, int code) : type(type), code(code) {
    }

    bool active() const {
        return state;
    }

    bool jactive() const {
        return state && justChanged;
    }

    void reset(InputType, int);
    void reset(Keycode);
    void reset(Mousecode);

    inline std::string text() const {
        switch (type) {
            case InputType::KEYBOARD: {
                return input_util::to_string(static_cast<Keycode>(code));
            }
            case InputType::MOUSE: {
                return input_util::to_string(static_cast<Mousecode>(code));
            }
        }
        return "<unknown input type>";
    }
};

class Bindings {
    std::unordered_map<std::string, Binding> bindings;
public:
    bool active(const std::string& name) const {
        const auto& found = bindings.find(name);
        if (found == bindings.end()) {
            return false;
        }
        return found->second.active();
    }

    bool jactive(const std::string& name) const {
        const auto& found = bindings.find(name);
        if (found == bindings.end()) {
            return false;
        }
        return found->second.jactive();
    }

    Binding* get(const std::string& name) {
        const auto found = bindings.find(name);
        if (found == bindings.end()) {
            return nullptr;
        }
        return &found->second;
    }

    const Binding* get(const std::string& name) const {
        const auto found = bindings.find(name);
        if (found == bindings.end()) {
            return nullptr;
        }
        return &found->second;
    }

    Binding& require(const std::string& name);

    const Binding& require(const std::string& name) const;

    void bind(const std::string& name, InputType type, int code) {
        bindings.try_emplace(name, Binding(type, code));
    }

    void rebind(const std::string& name, InputType type, int code) {
        require(name).reset(type, code);
    }

    auto& getAll() {
        return bindings;
    }

    void enableAll() {
        for (auto& entry : bindings) {
            entry.second.enabled = true;
        }
    }

    void read(const dv::value& map, BindType bindType);
    std::string write() const;
};

struct CursorState {
    bool locked = false;
    glm::vec2 pos {};
    glm::vec2 delta {};
};

class Input {
public:
    virtual ~Input() = default;

    virtual void pollEvents() = 0;

    virtual const char* getClipboardText() const = 0;
    virtual void setClipboardText(const char* str) = 0;

    virtual int getScroll() = 0;

    virtual bool pressed(Keycode keycode) const = 0;
    virtual bool jpressed(Keycode keycode) const = 0;

    virtual bool clicked(Mousecode mousecode) const = 0;
    virtual bool jclicked(Mousecode mousecode) const = 0;

    virtual CursorState getCursor() const = 0;

    virtual bool isCursorLocked() const = 0;
    virtual void toggleCursor() = 0;

    virtual Bindings& getBindings() = 0;

    virtual const Bindings& getBindings() const = 0;

    virtual ObserverHandler addKeyCallback(
        Keycode key, KeyCallback callback
    ) = 0;

    virtual const std::vector<Keycode>& getPressedKeys() const = 0;
    virtual const std::vector<uint>& getCodepoints() const = 0;

    ObserverHandler addCallback(const std::string& name, KeyCallback callback) {
        return getBindings().require(name).onactived.add(callback);
    }
};
