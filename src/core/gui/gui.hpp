#pragma once
#include "rltk/rltk.hpp"

#include "entt/entt.hpp"
#include "core/gui/item_menu.hpp"

namespace radl {

using rltk::term;

}

namespace radl::gui {


void init();

void render_gui();
void clear_gui();

class RadlUI {
private:
    int mouse_x = 0;
    int mouse_y = 0;
    std::array<bool, 7> mouse_button_pressed;

    sf::RenderTexture m_game_window_texture;

public:
    RadlUI();

    ~RadlUI();

    void process_event(const sf::Event &event);

void update();
    void frame();
    void render_game_window();

    /**
     * @brief Display the UI
     *
     */
    void display();
};

// void render_inventory_use(std::function<char(void)> get_option);

}  // namespace radl::gui
