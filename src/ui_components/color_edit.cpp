#include "ui_components/color_edit.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

ColorEdit::ColorEdit()
    : label_(nullptr),
      color_(0.0f, 0.0f, 0.0f)
{
}

bool ColorEdit::onUiComponent()
{
  if (ImGui::ColorEdit3(label_, &color_[0]))
  {
    return true;
  }

  return false;
}

void ColorEdit::setLabel(const char *label)
{
  label_ = label;
}

void ColorEdit::setColor(const glm::vec3 &color)
{
  color_ = color;
}

glm::vec3 ColorEdit::getColor() const
{
  return color_;
}
