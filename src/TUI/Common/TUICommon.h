#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <ncurses.h>

namespace FuegoTUI {

enum class ColorPair {
    DEFAULT = 1,
    HEADER = 2,
    SUCCESS = 3,
    WARNING = 4,
    ERROR = 5,
    INFO = 6,
    HIGHLIGHT = 7
};

enum class Alignment {
    LEFT,
    CENTER,
    RIGHT
};

struct Position {
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Size {
    int width, height;
    Size(int w = 0, int h = 0) : width(w), height(h) {}
};

class TUIComponent {
public:
    virtual ~TUIComponent() = default;
    virtual void render() = 0;
    virtual void handleInput(int key) = 0;
    virtual void setPosition(const Position& pos) { position = pos; }
    virtual void setSize(const Size& size) { this->size = size; }
    virtual Position getPosition() const { return position; }
    virtual Size getSize() const { return size; }

protected:
    Position position{0, 0};
    Size size{0, 0};
};

class TUIManager {
public:
    static TUIManager& getInstance();
    
    void initialize();
    void cleanup();
    void run();
    void stop();
    
    void addComponent(std::shared_ptr<TUIComponent> component);
    void removeComponent(std::shared_ptr<TUIComponent> component);
    
    void setHeader(const std::string& text);
    void setStatus(const std::string& text);
    void showMessage(const std::string& message, ColorPair color = ColorPair::INFO);
    
    void refresh();
    void clear();
    
    Size getScreenSize() const;
    Position getCursorPosition() const;
    
    void setColor(ColorPair pair, short foreground, short background);
    void useColor(ColorPair pair);
    void resetColor();

private:
    TUIManager() = default;
    ~TUIManager() = default;
    TUIManager(const TUIManager&) = delete;
    TUIManager& operator=(const TUIManager&) = delete;
    
    bool initialized = false;
    bool running = false;
    std::string headerText;
    std::string statusText;
    std::vector<std::shared_ptr<TUIComponent>> components;
    
    void renderHeader();
    void renderStatus();
    void renderComponents();
    void handleKeyboardInput();
};

class TUIWindow {
public:
    TUIWindow(const Position& pos, const Size& size, const std::string& title = "");
    ~TUIWindow();
    
    void render();
    void setTitle(const std::string& title);
    void addText(const std::string& text, const Position& pos, ColorPair color = ColorPair::DEFAULT);
    void addText(const std::string& text, int x, int y, ColorPair color = ColorPair::DEFAULT);
    void clear();
    void refresh();
    
    Position getPosition() const { return position; }
    Size getSize() const { return size; }
    
    void setPosition(const Position& pos) { position = pos; }
    void setSize(const Size& size) { this->size = size; }

private:
    Position position;
    Size size;
    std::string title;
    WINDOW* window = nullptr;
    
    void drawBorder();
};

class TUIButton {
public:
    TUIButton(const std::string& text, const Position& pos, const Size& size);
    ~TUIButton();
    
    void render();
    void setText(const std::string& text);
    void setCallback(std::function<void()> callback);
    void setSelected(bool selected);
    bool isSelected() const { return selected; }
    
    void handleClick();
    
    Position getPosition() const { return position; }
    Size getSize() const { return size; }

private:
    std::string text;
    Position position;
    Size size;
    bool selected = false;
    std::function<void()> callback;
    WINDOW* window = nullptr;
};

class TUIList {
public:
    TUIList(const Position& pos, const Size& size);
    ~TUIList();
    
    void addItem(const std::string& item);
    void removeItem(size_t index);
    void clear();
    
    void render();
    void handleInput(int key);
    
    size_t getSelectedIndex() const { return selectedIndex; }
    std::string getSelectedItem() const;
    
    void setPosition(const Position& pos) { position = pos; }
    void setSize(const Size& size) { this->size = size; }

private:
    std::vector<std::string> items;
    size_t selectedIndex = 0;
    size_t scrollOffset = 0;
    Position position;
    Size size;
    WINDOW* window = nullptr;
    
    void scrollToSelected();
};

class TUIInput {
public:
    TUIInput(const Position& pos, const Size& size, const std::string& prompt = "");
    ~TUIInput();
    
    void render();
    void handleInput(int key);
    void setText(const std::string& text);
    std::string getText() const { return text; }
    void clear();
    void setPrompt(const std::string& prompt);
    
    void setPosition(const Position& pos) { position = pos; }
    void setSize(const Size& size) { this->size = size; }
    void setFocused(bool focused);
    bool isFocused() const { return focused; }

private:
    std::string text;
    std::string prompt;
    Position position;
    Size size;
    bool focused = false;
    size_t cursorPos = 0;
    WINDOW* window = nullptr;
};

// Utility functions
std::string formatXFGAmount(uint64_t amount);
std::string formatTime(uint64_t timestamp);
std::string formatHash(const std::string& hash);
std::string truncateString(const std::string& str, size_t maxLength);
std::string centerString(const std::string& str, size_t width);

} // namespace FuegoTUI