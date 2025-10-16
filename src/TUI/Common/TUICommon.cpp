#include "TUICommon.h"
#include <curses.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace FuegoTUI {

TUIManager& TUIManager::getInstance() {
    static TUIManager instance;
    return instance;
}

void TUIManager::initialize() {
    if (initialized) return;
    
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    // Initialize colors
    if (has_colors()) {
        start_color();
        init_pair(static_cast<short>(ColorPair::DEFAULT), COLOR_WHITE, COLOR_BLACK);
        init_pair(static_cast<short>(ColorPair::HEADER), COLOR_CYAN, COLOR_BLACK);
        init_pair(static_cast<short>(ColorPair::SUCCESS), COLOR_GREEN, COLOR_BLACK);
        init_pair(static_cast<short>(ColorPair::WARNING), COLOR_YELLOW, COLOR_BLACK);
        init_pair(static_cast<short>(ColorPair::ERROR), COLOR_RED, COLOR_BLACK);
        init_pair(static_cast<short>(ColorPair::INFO), COLOR_BLUE, COLOR_BLACK);
        init_pair(static_cast<short>(ColorPair::HIGHLIGHT), COLOR_MAGENTA, COLOR_BLACK);
    }
    
    initialized = true;
    running = true;
}

void TUIManager::cleanup() {
    if (!initialized) return;
    
    endwin();
    initialized = false;
    running = false;
}

void TUIManager::run() {
    if (!initialized) return;
    
    while (running) {
        clear();
        renderHeader();
        renderComponents();
        renderStatus();
        refresh();
        
        handleKeyboardInput();
    }
}

void TUIManager::stop() {
    running = false;
}

void TUIManager::addComponent(std::shared_ptr<TUIComponent> component) {
    components.push_back(component);
}

void TUIManager::removeComponent(std::shared_ptr<TUIComponent> component) {
    components.erase(
        std::remove(components.begin(), components.end(), component),
        components.end()
    );
}

void TUIManager::setHeader(const std::string& text) {
    headerText = text;
}

void TUIManager::setStatus(const std::string& text) {
    statusText = text;
}

void TUIManager::showMessage(const std::string& message, ColorPair color) {
    // Simple message display - could be enhanced with popup windows
    useColor(color);
    mvprintw(getmaxy(stdscr) - 2, 1, "%s", message.c_str());
    resetColor();
    refresh();
}

void TUIManager::refresh() {
    ::refresh();
}

void TUIManager::clear() {
    ::clear();
}

Size TUIManager::getScreenSize() const {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    return Size(maxX, maxY);
}

Position TUIManager::getCursorPosition() const {
    int y, x;
    getyx(stdscr, y, x);
    return Position(x, y);
}

void TUIManager::setColor(ColorPair pair, short foreground, short background) {
    init_pair(static_cast<short>(pair), foreground, background);
}

void TUIManager::useColor(ColorPair pair) {
    attron(COLOR_PAIR(static_cast<short>(pair)));
}

void TUIManager::resetColor() {
    attroff(COLOR_PAIR(static_cast<short>(ColorPair::DEFAULT)));
}

void TUIManager::renderHeader() {
    if (headerText.empty()) return;
    
    useColor(ColorPair::HEADER);
    mvprintw(0, 0, "%s", headerText.c_str());
    resetColor();
}

void TUIManager::renderStatus() {
    if (statusText.empty()) return;
    
    int maxY = getmaxy(stdscr);
    useColor(ColorPair::INFO);
    mvprintw(maxY - 1, 0, "%s", statusText.c_str());
    resetColor();
}

void TUIManager::renderComponents() {
    for (auto& component : components) {
        component->render();
    }
}

void TUIManager::handleKeyboardInput() {
    int key = getch();
    
    // Handle global keys
    if (key == 'q' || key == 'Q' || key == 27) { // ESC or 'q'
        stop();
        return;
    }
    
    // Pass input to components
    for (auto& component : components) {
        component->handleInput(key);
    }
}

// TUIWindow implementation
TUIWindow::TUIWindow(const Position& pos, const Size& size, const std::string& title)
    : position(pos), size(size), title(title) {
    window = newwin(size.height, size.width, position.y, position.x);
}

TUIWindow::~TUIWindow() {
    if (window) {
        delwin(window);
    }
}

void TUIWindow::render() {
    if (!window) return;
    
    wclear(window);
    drawBorder();
    
    if (!title.empty()) {
        wmove(window, 0, 1);
        waddstr(window, title.c_str());
    }
    
    wrefresh(window);
}

void TUIWindow::setTitle(const std::string& title) {
    this->title = title;
}

void TUIWindow::addText(const std::string& text, const Position& pos, ColorPair color) {
    if (!window) return;
    
    wattron(window, COLOR_PAIR(static_cast<short>(color)));
    mvwaddstr(window, pos.y, pos.x, text.c_str());
    wattroff(window, COLOR_PAIR(static_cast<short>(color)));
}

void TUIWindow::addText(const std::string& text, int x, int y, ColorPair color) {
    addText(text, Position(x, y), color);
}

void TUIWindow::clear() {
    if (window) {
        wclear(window);
    }
}

void TUIWindow::refresh() {
    if (window) {
        wrefresh(window);
    }
}

void TUIWindow::drawBorder() {
    if (!window) return;
    
    box(window, 0, 0);
}

// TUIButton implementation
TUIButton::TUIButton(const std::string& text, const Position& pos, const Size& size)
    : text(text), position(pos), size(size) {
    window = newwin(size.height, size.width, position.y, position.x);
}

TUIButton::~TUIButton() {
    if (window) {
        delwin(window);
    }
}

void TUIButton::render() {
    if (!window) return;
    
    wclear(window);
    
    if (selected) {
        wattron(window, A_REVERSE);
    }
    
    // Center text in button
    int textX = (size.width - text.length()) / 2;
    int textY = size.height / 2;
    
    mvwaddstr(window, textY, textX, text.c_str());
    
    if (selected) {
        wattroff(window, A_REVERSE);
    }
    
    wrefresh(window);
}

void TUIButton::setText(const std::string& text) {
    this->text = text;
}

void TUIButton::setCallback(std::function<void()> callback) {
    this->callback = callback;
}

void TUIButton::setSelected(bool selected) {
    this->selected = selected;
}

void TUIButton::handleClick() {
    if (callback) {
        callback();
    }
}

// TUIList implementation
TUIList::TUIList(const Position& pos, const Size& size)
    : position(pos), size(size) {
    window = newwin(size.height, size.width, position.y, position.x);
}

TUIList::~TUIList() {
    if (window) {
        delwin(window);
    }
}

void TUIList::addItem(const std::string& item) {
    items.push_back(item);
}

void TUIList::removeItem(size_t index) {
    if (index < items.size()) {
        items.erase(items.begin() + index);
        if (selectedIndex >= items.size() && !items.empty()) {
            selectedIndex = items.size() - 1;
        }
    }
}

void TUIList::clear() {
    items.clear();
    selectedIndex = 0;
    scrollOffset = 0;
}

void TUIList::render() {
    if (!window) return;
    
    wclear(window);
    
    for (size_t i = 0; i < size.height - 2 && i + scrollOffset < items.size(); ++i) {
        size_t itemIndex = i + scrollOffset;
        
        if (itemIndex == selectedIndex) {
            wattron(window, A_REVERSE);
        }
        
        std::string displayText = truncateString(items[itemIndex], size.width - 2);
        mvwaddstr(window, i + 1, 1, displayText.c_str());
        
        if (itemIndex == selectedIndex) {
            wattroff(window, A_REVERSE);
        }
    }
    
    wrefresh(window);
}

void TUIList::handleInput(int key) {
    switch (key) {
        case KEY_UP:
            if (selectedIndex > 0) {
                selectedIndex--;
                scrollToSelected();
            }
            break;
        case KEY_DOWN:
            if (selectedIndex < items.size() - 1) {
                selectedIndex++;
                scrollToSelected();
            }
            break;
        case KEY_HOME:
            selectedIndex = 0;
            scrollOffset = 0;
            break;
        case KEY_END:
            selectedIndex = items.size() - 1;
            scrollToSelected();
            break;
    }
}

std::string TUIList::getSelectedItem() const {
    if (selectedIndex < items.size()) {
        return items[selectedIndex];
    }
    return "";
}

void TUIList::scrollToSelected() {
    if (selectedIndex < scrollOffset) {
        scrollOffset = selectedIndex;
    } else if (selectedIndex >= scrollOffset + size.height - 2) {
        scrollOffset = selectedIndex - size.height + 3;
    }
}

// TUIInput implementation
TUIInput::TUIInput(const Position& pos, const Size& size, const std::string& prompt)
    : prompt(prompt), position(pos), size(size) {
    window = newwin(size.height, size.width, position.y, position.x);
}

TUIInput::~TUIInput() {
    if (window) {
        delwin(window);
    }
}

void TUIInput::render() {
    if (!window) return;
    
    wclear(window);
    
    // Draw prompt
    if (!prompt.empty()) {
        mvwaddstr(window, 0, 0, prompt.c_str());
    }
    
    // Draw input field
    if (focused) {
        wattron(window, A_REVERSE);
    }
    
    std::string displayText = truncateString(text, size.width - 2);
    mvwaddstr(window, 1, 1, displayText.c_str());
    
    if (focused) {
        wattroff(window, A_REVERSE);
        // Show cursor
        wmove(window, 1, 1 + cursorPos);
    }
    
    wrefresh(window);
}

void TUIInput::handleInput(int key) {
    if (!focused) return;
    
    switch (key) {
        case KEY_BACKSPACE:
        case 127: // Delete key
            if (cursorPos > 0) {
                text.erase(cursorPos - 1, 1);
                cursorPos--;
            }
            break;
        case KEY_DC: // Delete character at cursor
            if (cursorPos < text.length()) {
                text.erase(cursorPos, 1);
            }
            break;
        case KEY_LEFT:
            if (cursorPos > 0) {
                cursorPos--;
            }
            break;
        case KEY_RIGHT:
            if (cursorPos < text.length()) {
                cursorPos++;
            }
            break;
        case KEY_HOME:
            cursorPos = 0;
            break;
        case KEY_END:
            cursorPos = text.length();
            break;
        default:
            if (key >= 32 && key <= 126) { // Printable characters
                text.insert(cursorPos, 1, static_cast<char>(key));
                cursorPos++;
            }
            break;
    }
}

void TUIInput::setText(const std::string& text) {
    this->text = text;
    cursorPos = text.length();
}

void TUIInput::clear() {
    text.clear();
    cursorPos = 0;
}

void TUIInput::setPrompt(const std::string& prompt) {
    this->prompt = prompt;
}

void TUIInput::setFocused(bool focused) {
    this->focused = focused;
}

// Utility functions
std::string formatXFGAmount(uint64_t amount) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(8) << (amount / 100000000.0) << " XFG";
    return oss.str();
}

std::string formatTime(uint64_t timestamp) {
    std::time_t time = static_cast<std::time_t>(timestamp);
    std::tm* tm = std::localtime(&time);
    
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
    return std::string(buffer);
}

std::string formatHash(const std::string& hash) {
    if (hash.length() > 16) {
        return hash.substr(0, 8) + "..." + hash.substr(hash.length() - 8);
    }
    return hash;
}

std::string truncateString(const std::string& str, size_t maxLength) {
    if (str.length() <= maxLength) {
        return str;
    }
    return str.substr(0, maxLength - 3) + "...";
}

std::string centerString(const std::string& str, size_t width) {
    if (str.length() >= width) {
        return str.substr(0, width);
    }
    
    size_t padding = (width - str.length()) / 2;
    return std::string(padding, ' ') + str;
}

} // namespace FuegoTUI