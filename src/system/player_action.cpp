#include "SFML/System.hpp"
#include "spdlog/spdlog.h"
#include "player_action.hpp"
#include "core/engine.hpp"
#include "system/camera.hpp"
#include <component/component.hpp>

namespace radl {

void handle_screen_resize(const sf::Event& ev, entt::registry& reg,
                          entt::entity e) {
    if(ev.type == sf::Event::Resized) {
        using rltk::console;
        camera_update(reg, e);
        spdlog::debug("screen resize: x: {}, y: {}", console->term_width,
                      console->term_height);
    }
}

static position_t get_next_position(const sf::Event& ev, bool* player_input) {
    int dx = 0;
    int dy = 0;

    if(ev.type != sf::Event::EventType::KeyPressed) {
        return {dx, dy};
    }

    using sf::Keyboard;
    switch(ev.key.code) {
    case Keyboard::Numpad8:
    case Keyboard::K: {
        --dy;
    } break;
    case Keyboard::Numpad2:
    case Keyboard::J: {
        ++dy;
    } break;
    case Keyboard::Numpad4:
    case Keyboard::H: {
        --dx;
    } break;
    case Keyboard::Numpad6:
    case Keyboard::L: {
        ++dx;
    } break;
    case Keyboard::Numpad7:
    case Keyboard::Y: {
        --dx;
        --dy;
    } break;
    case Keyboard::Numpad9:
    case Keyboard::U: {
        ++dx;
        --dy;
    } break;
    case Keyboard::Numpad1:
    case Keyboard::B: {
        --dx;
        ++dy;
    } break;
    case Keyboard::Numpad3:
    case Keyboard::N: {
        ++dx;
        ++dy;
    } break;
    case sf::Keyboard::Numpad5: {
        *player_input = true;
    } break;
    default:
        break;
    }

    if(!*player_input) {
        *player_input = dx != 0 || dy != 0;
    }
    return position_t{dx, dy};
}


bool process_input(entt::registry& reg, entt::entity e) {
    bool player_input = false;
    using rltk::my_event_queue;
    if(!engine::event_queue.empty()) {
        auto ev = engine::event_queue.front();
        engine::event_queue.pop();
        if(ev.type != sf::Event::EventType::KeyPressed) {
            return false;
        }
        handle_screen_resize(ev, reg, e);
        auto delta_pos = get_next_position(ev, &player_input);
        if(delta_pos != position_t{0, 0}) {
            move_attack(reg, e, delta_pos);
        }
    }
    return player_input;
}

namespace {
std::mutex walk_mutex;
}

void walk(entt::entity ent, const position_t& src_pos,
          const position_t& target_pos) {
    using engine::reg;
    std::lock_guard guard(walk_mutex);
    if(!world::is_occupied(reg, target_pos)) {
        auto& map = reg.ctx<world::Map>();
        map[src_pos].entities_here.remove(ent);
        map[target_pos].entities_here.push_front(ent);
        reg.get<world::viewshed_t>(ent).dirty = true;
        reg.replace<position_t>(ent, target_pos);
    }
}

bool move_attack(entt::registry& reg, entt::entity& ent,
                 const position_t& delta_pos) {
    using engine::reg;
    // mark as dirty to trigger an screen update
    auto& actual_pos = reg.get<position_t>(ent);
    auto target_pos  = actual_pos + delta_pos;

    // maybe check if occupies vicinity, or add vicinity component
    auto& map                     = reg.ctx<world::Map>();
    const auto& target_tile_chars = map[{target_pos}].characteristics;

    // ## 1. attack if enemy is in the targeted pos
    if(radl::world::is_occupied(reg, target_pos)) {
        // if(is_occupied(reg, target_pos)) {
        // attack
        return false;
    }
    // ## 2. walk if tile is no occupied and walkable
    else if(target_tile_chars.walkable) {
        walk(ent, actual_pos, target_pos);
    }

    // ## 3. do nothing if is wall
    return true;
}

}  // namespace radl
