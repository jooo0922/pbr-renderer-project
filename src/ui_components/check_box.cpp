#include "ui_components/check_box.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

CheckBox::CheckBox()
    : label_(nullptr),
      value_(false)
{
}

bool CheckBox::onUiComponent()
{
  if (ImGui::Checkbox(label_, &value_))
  {
    return true;
  }

  return false;
}

void CheckBox::setLabel(const char *label)
{
  label_ = label;
}

void CheckBox::setValue(const bool value)
{
  value_ = value;
}

bool CheckBox::getValue() const
{
  return value_;
}
