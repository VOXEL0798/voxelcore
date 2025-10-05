#include <SDL3/SDL_video.h>
#include "window/window.hpp"

struct  SDL_Window;

class window_sdl final : public Window {
public:
    window_sdl(DisplaySettings *settings, std::string title) noexcept;
    ~window_sdl();
    window_sdl(const window_sdl &) = delete;
    window_sdl(window_sdl &&) = default;
    window_sdl &operator=(const window_sdl &) = delete;
    window_sdl &operator=(window_sdl &&) = default;

    void swapBuffers() const noexcept override;

    bool isMaximized() const override;
    bool isFocused() const override;
    bool isIconified() const override;

    bool isShouldClose() const override;
    void setShouldClose(bool flag) override;

    void setCursor(CursorShape shape) override;
    void toggleFullscreen() override;
    bool isFullscreen() const override;

    void setIcon(const ImageData *image) override;

    void pushScissor(glm::vec4 area) override;
    void popScissor() override;
    void resetScissor() override;

    double time() override;

    void setFramerate(int framerate) override;

    // todo: move somewhere
    std::unique_ptr<ImageData> takeScreenshot() override;

    [[nodiscard]] bool isValid() const override;
    [[nodiscard]] SDL_Window* getSdlWindow() const;
private:
    bool isSuccessfull = true;
    bool maximized = false;
    bool focused = false;
    bool iconified = false;
    bool toClose = false;
    bool fullscreen = false;

    SDL_Window *window;
    SDL_GLContext context;
};