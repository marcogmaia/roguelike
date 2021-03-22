#include <iostream>
#include <fmt/format.h>

#include "actor.hpp"
#include "engine.hpp"

Actor::Actor(const position_t& position, int ch, const char* src_name,
             uint32_t fov_rad, const TCODColor& color)
    : ch(ch)
    , position(position)
    , fov_radius(fov_rad)
    , color(color) {
    strncpy(name, src_name, sizeof name - 1);
    std::cout << fmt::format("Actor CTOR {}\n", name);
}

Actor::~Actor() {
    std::cout << "Actor DTOR\n";
}

bool Actor::update() {
    auto updated = false;
    if(ai) {
        updated = ai->update(this);
        if(updated) {
            std::cout << fmt::format("updating {}\n", name);
        }
    }
    return updated;
}

void Actor::render() const {
    TCODConsole::root->setChar(position.x, position.y, ch);
    TCODConsole::root->setCharForeground(position.x, position.y, color);
}
