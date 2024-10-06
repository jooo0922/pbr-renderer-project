#include "ui_components/combo.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

Combo::Combo()
    : label_(nullptr),
      currentIndex_(0)
{
}

bool Combo::onUiComponent()
{
  if (ImGui::Combo(label_, &currentIndex_, items_.data(), static_cast<int>(items_.size())))
  {
    return true;
  }

  return false;
}

void Combo::setLabel(const char *label)
{
  label_ = label;
}

void Combo::setItems(const std::vector<const char *> &items)
{
  items_ = items;
}

void Combo::setCurrentIndex(int index)
{
  currentIndex_ = index;
}

int Combo::getCurrentIndex() const
{
  return currentIndex_;
}
