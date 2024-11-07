#ifndef LAYOUT_CONSTANTS_HPP
#define LAYOUT_CONSTANTS_HPP

namespace LayoutConstants
{
  constexpr const char WINDOW_NAME[] = "PBR Renderer";
  constexpr int WINDOW_WIDTH_DEFAULT = 1920;
  constexpr int WINDOW_HEIGHT_DEFAULT = 1080;

  constexpr float MARGIN = 20.0f;

  constexpr float EFFECTIVE_WIDTH = static_cast<float>(WINDOW_WIDTH_DEFAULT) - MARGIN * 2.0f;

  constexpr float PANEL_WIDTH = EFFECTIVE_WIDTH * 0.25f;
  constexpr float PANEL_PADDING = 16.0f;

  constexpr float TITLE_PADDING = 8.0f;
}

#endif /* LAYOUT_CONSTANTS_HPP */
