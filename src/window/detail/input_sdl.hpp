#include "window/input.hpp"

struct SDL_Window;

class input_sdl final : public Input {
public:
    input_sdl();

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
    const std::vector<uint>& getCodepoints() const override;
private:
    Bindings bindings;
    std::vector<uint> codepoints;
    std::vector<Keycode> pressedKeys;
};