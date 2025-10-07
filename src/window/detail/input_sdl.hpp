#include <SDL3/SDL_keycode.h>

#include "window/input.hpp"
#include "window/window.hpp"

class window_sdl;

class input_sdl final : public Input {
public:
    inline static constexpr short mouse_keys_offset = 512;
    inline static constexpr short keys_buffer_size =
        mouse_keys_offset + sizeof(mousecodes_all) / sizeof(mousecodes_all[0]);

    input_sdl(window_sdl& window);

    void pollEvents() override;

    const char* getClipboardText() const override;
    void setClipboardText(const char* str) override;

    int getScroll() override;

    bool pressed(Keycode keycode) const override;
    bool jpressed(Keycode keycode) const override;

    bool clicked(Mousecode mousecode) const override;
    bool jclicked(Mousecode mousecode) const override;

    CursorState getCursor() const override;

    bool isCursorLocked() const override;
    void toggleCursor() override;

    Bindings& getBindings() override;

    const Bindings& getBindings() const override;

    ObserverHandler addKeyCallback(Keycode key, KeyCallback callback) override;

    const std::vector<Keycode>& getPressedKeys() const override;
    const std::vector<char>& getCodepoints() const override;
private:
    glm::vec2 delta;
    glm::vec2 cursor;
    std::int32_t scroll = 0;
    std::uint32_t currentFrame = 0;
    std::uint32_t frames[keys_buffer_size] {};
    bool keys[keys_buffer_size] {};
    bool cursorLocked = false;
    bool cursorDrag = false;
    Bindings bindings;
    std::vector<char> codepoints;
    std::vector<Keycode> pressedKeys;
    std::unordered_map<Keycode, util::HandlersList<>> keyCallbacks;

    window_sdl& window;
};