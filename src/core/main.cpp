#include <iostream>
#include <chrono>
#include <fmt/format.h>


#include "imgui.h"
#include "imgui-SFML.h"


#include "entt/entt.hpp"

#include "rltk/rltk.hpp"
#include "core/engine.hpp"

namespace {

using namespace radl;
using engine::console;
// using engine::main_window;

}  // namespace

void imgui_frame() {
    ImGui::Begin("Hello, world!");
    ImGui::Button("Look at this pretty button");
    ImGui::End();
    ImGui::EndFrame();
}

void run_game(entt::delegate<void(double)> on_game_tick) {
    auto& main_window = *rltk::get_window();
    state::reset_mouse_state();
    double duration_ms = 0.0;

    sf::Clock deltaClock;
    // auto ctximgui = ImGui::CreateContext();
    // ImGui::SetCurrentContext(ctximgui);
    ImGui::SFML::Init(main_window);

    auto clock = std::chrono::steady_clock();
    while(main_window.isOpen()) {
        auto start_time = clock.now();

        sf::Event event;
        while(main_window.pollEvent(event)) {
            switch(event.type) {
            case sf::Event::Closed: {
                main_window.close();
            } break;
            case sf::Event::Resized: {
                if(event.size.width < 1024) {
                    event.size.width = 1024;
                }
                if(event.size.height < 768) {
                    event.size.height = 768;
                }
                main_window.setSize(
                    sf::Vector2u(event.size.width, event.size.height));
                main_window.setView(sf::View(sf::FloatRect(
                    0.f, 0.f, static_cast<float>(event.size.width),
                    static_cast<float>(event.size.height))));
                rltk::gui->on_resize(event.size.width, event.size.height);
                engine::event_queue.push_back(event);
            } break;
            case sf::Event::LostFocus: {
                //
            } break;
            case sf::Event::GainedFocus: {
                //
            } break;
            case sf::Event::MouseMoved: {
                //
                state::set_mouse_position(event.mouseMove.x, event.mouseMove.y);
            } break;
            case sf::Event::MouseButtonPressed: {
                //
            } break;
            case sf::Event::MouseButtonReleased: {
                //
            } break;
            case sf::Event::KeyPressed: {
                engine::event_queue.push_back(event);
                engine::event_dispatcher.enqueue<sf::Event>(event);
            } break;
            case sf::Event::KeyReleased: {
                //
            } break;
            default: break;
            }
        }


        if(on_game_tick) {
            on_game_tick(duration_ms);
        }


        main_window.clear();

        rltk::gui->render(main_window);


        // ## test
        ImGui::SFML::ProcessEvent(event);
        ImGui::SFML::Update(main_window, deltaClock.restart());
        imgui_frame();
        // ImGui::ShowDemoWindow(nullptr);

        // ImGui::Render();
        ImGui::SFML::Render(main_window);
        // ## end test

        main_window.display();


        duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          clock.now() - start_time)
                          .count();
    }

    ImGui::SFML::Shutdown();
}

int main() {
    engine::init();


    entt::delegate<void(double)> delegate_run_game;
    delegate_run_game.connect<&engine::update>();

    run_game(delegate_run_game);

    engine::terminate();

    return 0;
}
