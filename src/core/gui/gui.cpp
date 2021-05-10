#include <algorithm>
#include <map>

#include "spdlog/spdlog.h"

#include "core/gui/gui.hpp"
#include "core/engine.hpp"

#include "component/component.hpp"
#include "system/factories.hpp"
#include "system/system.hpp"
#include "system/game_state.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui-SFML.h"


namespace radl::gui {

namespace {

using namespace rltk::colors;
using rltk::gui;

}  // namespace

void resize_main(rltk::layer_t* l, int w, int h) {
    // Simply set the width to the whole window width
    l->w = w;
    l->h = h;
}

void resize_inventory_popup(rltk::layer_t* l, int w, int h) {
    // Simply set the width to the whole window width
    l->w = l->w;
    l->h = l->h;
    l->x = 32;
    l->y = 32;
}

void resize_status(rltk::layer_t* l, int w, int h) {
    auto [fw, fh] = term(UI_STATUS)->get_font_size();

    l->w = (w / fw) * fw - (20 + 3) * fw;
    l->h = (2 + 8) * fh;
    l->y = ((h / fh) - (l->h / fh)) * fh;
}

void resize_inventory(rltk::layer_t* l, int w, int h) {
    auto [fw, fh] = term(UI_INVENTORY)->get_font_size();

    l->w = (20 + 2) * fw;
    l->h = (h / fh) * fh;
    l->x = ((w / fw) - (l->w / fw)) * fw;
    l->y = (h / fh - l->h / fh) * fh;
}


void init() {
    rect_t map_rect = {
        0,
        0,
        1024,
        768 - 16 * 10,
    };

    gui->add_layer(UI_MAP, map_rect.x1, map_rect.y1, map_rect.x2, map_rect.y2,
                   "16x16", resize_main, true);
    gui->add_layer(UI_ENTITIES, map_rect.x1, map_rect.y1, map_rect.x2,
                   map_rect.y2, "16x16", resize_main, false);
    gui->add_sparse_layer(UI_PARTICLES, map_rect.x1, map_rect.y1, map_rect.x2,
                          map_rect.y2, "16x16", resize_main);

    engine::console = term(UI_MAP);
}


void RadlUI::update() {
    if(!reg.valid(player) || !reg.all_of<player_t, combat_stats_t>(player)) {
        return;
    }
    static sf::Clock deltaClock;
    auto& main_window = *rltk::get_window();
    ImGui::SFML::Update(main_window, deltaClock.restart());
}

void RadlUI::render_ui() {
    if(!reg.valid(player) || !reg.all_of<player_t, combat_stats_t>(player)) {
        return;
    }
    static sf::Clock deltaClock;
    auto& main_window = *rltk::get_window();
    ImGui::SFML::Update(main_window, deltaClock.restart());


    render_game_window();
}

void RadlUI::display() {
    ImGui::EndFrame();
    ImGui::SFML::Render(*rltk::get_window());
}


namespace {

[[maybe_unused]] void resize_callback(ImGuiSizeCallbackData* data) {
    system::system_camera();
    reg.ctx<camera_t>().dirty = true;
}

}  // namespace

void RadlUI::render_game_window() {
    m_game_window_texture.clear();
    rltk::gui->render(m_game_window_texture);
    m_game_window_texture.display();


    static ImGuiWindowFlags game_window_flags
        = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
          | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration
          | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse;

    // static ImGuiWindowClass game_window_class;
    // game_window_class.DockNodeFlagsOverrideSet
    //     = game_window_class.DockNodeFlagsOverrideClear
    //     = ImGuiDockNodeFlags_AutoHideTabBar | ImGuiDockNodeFlags_NoTabBar;
    // ImGui::SetNextWindowClass(&game_window_class);
    // ImGui::SetNextWindowSizeConstraints()

    auto& io = ImGui::GetIO();
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
    ImGui::SetNextWindowPos({0, 0});

    ImGui::Begin("Game Window", nullptr, game_window_flags);
    // {
    //     static bool first_run = true;
    //     static auto winsize   = ImGui::GetWindowSize();
    //     auto [w, h]           = ImGui::GetWindowSize();
    //     if(winsize.x != w || winsize.y != h || first_run) {
    //         first_run = false;
    //         winsize   = {w, h};
    //         // rltk::gui->on_resize(w, h);
    //     }
    // }

    ImGui::Image(m_game_window_texture);
    // ImGui::SetNextWindowSizeConstraints

    // TODO refresh image when window is resized


    static float pad = 0.0;
    {
        auto wpos = ImGui::GetWindowPos();
        wpos.x += pad;
        wpos.y += pad;
        ImGui::SetCursorScreenPos(wpos);
        ImGui::SetCursorPosX(pad);
        // ImGui::SetNextWindowPos(wpos, ImGuiCond_Always);


        // ImGuiWindowFlags window_flags
        //     = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground;
        // ImGui::Begin("ChildL", nullptr, window_flags);
        ImGui::Text("wtf");

        ImGui::SetCursorPosX(pad);
        static bool show_metrics = false;
        ImGui::Checkbox("metric", &show_metrics);
        if(show_metrics) {
            ImGui::ShowMetricsWindow();
        }

        ImGui::SetCursorPosX(pad);
        static bool show_demo_window = false;
        ImGui::Checkbox("demo", &show_demo_window);
        if(show_demo_window) {
            ImGui::ShowDemoWindow();
        }

        ImGui::SetCursorPosX(pad);
        ImGui::InputFloat("padding", &pad);

        // ImGui::End();
    }

    auto wsize = ImGui::GetWindowSize();
    auto wpos  = ImGui::GetWindowPos();
    ImGui::SetNextWindowPos({wpos.x + wsize.x, wpos.y}, ImGuiCond_Once, {1, 0});
    ImGui::BeginChild("Stats", {pad, 128}, true);
    {
        // change defense
        {
            auto& player_stats = reg.get<combat_stats_t>(player);
            ImGui::Text("Set def:");
            ImGui::SameLine();
            if(ImGui::ArrowButton("##def_up", ImGuiDir_Up)) {
                ++player_stats.defense;
            }
            ImGui::SameLine(0.0, 0.0);
            if(ImGui::ArrowButton("##def_down", ImGuiDir_Down)) {
                --player_stats.defense;
            }
            ImGui::SameLine();
            ImGui::Text("%d", player_stats.defense);
        }
        // show position
        {
            const auto& [px, py] = reg.get<position_t>(player);
            auto pos_str         = fmt::format("position: ({}, {})", px, py);
            ImGui::Text(pos_str.c_str());
        }
        // camera
        {
            static auto& camera = reg.ctx<camera_t>();
            auto fps            = 1000.0 / camera.frame_time;
            ImGui::Text(
                fmt::format("time: {:.2f}, fps: {:.2f}", camera.frame_time, fps)
                    .c_str());
            ImGui::Text(fmt::format("window size: ({:.2f}, {:.2f})",
                                    io.DisplaySize.x, io.DisplaySize.y)
                            .c_str());

            auto [tsizex, tsizey] = m_game_window_texture.getSize();
            ImGui::Text(
                fmt::format("texture size: ({}, {})", tsizex, tsizey).c_str());
            if(ImGui::Checkbox("reveal map", &camera.reveal_map)) {
                camera.dirty = true;
            }
        }
    }
    ImGui::EndChild();

    ImGui::SetNextWindowPos({0 + 20, wpos.y + wsize.y - 20}, ImGuiCond_Always,
                            {0, 1});
    {
        ImGui::BeginChild("Add Items", {256, 128}, true,
                          ImGuiWindowFlags_NoDecoration);
        auto pressed = ImGui::Button("+ healing potion.");
        if(pressed) {
            reg.get<inventory_t>(player).add_item(
                factory::items::potion_healing(true));
        }
        ImGui::EndChild();
    }

    ImGui::End();
}

void RadlUI::process_event(const sf::Event& event) {
    if(event.type == sf::Event::EventType::KeyPressed
       || event.type == sf::Event::EventType::KeyReleased) {
        if(event.key.code < 0) {
            return;
        }
    }
    ImGui::SFML::ProcessEvent(event);
}

RadlUI::RadlUI() {
    ImGui::SFML::Init(*rltk::get_window());
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    m_game_window_texture.create(3840, 2160);
}

RadlUI::~RadlUI() {
    ImGui::SFML::Shutdown();
}


}  // namespace radl::gui
