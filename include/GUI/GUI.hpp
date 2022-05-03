//
// Created by Sam on 3/29/2022.
//

#ifndef ELYS_GUI_HPP
#define ELYS_GUI_HPP

#include <string>
#include <optional>

#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_internal.h>

#include <Core/AssetLoader.hpp>
#include <Render/Material.hpp>

namespace Elys::GUI {
    enum Corner { TOP_LEFT = 0b00, TOP_RIGHT = 0b01, BOTTOM_LEFT = 0b10, BOTTOM_RIGHT = 0b11 };

    /// \Brief
    /// Take a corner value and edit windowPos and windowPosPivot given
    /// to position it to the chosen corner. You can also provide padding value.
    void ConfigureWindowPos(Corner corner, ImVec2 &windowPos, ImVec2 &windowPosPivot,
                            float padding = 0.0f);

    void SliderVec2(std::string const &label, glm::vec2 &data, float speed = 0.1f);

    /// \Brief
    /// Draw a colorized Vec3 editor
    void SliderVec3(std::string const &label, glm::vec3 &data, float speed = 0.1f);

    void TextureInput(std::optional<Texture> &texture, const char* ID);
} // namespace Elys::GUI

#endif // ELYS_GUI_HPP
