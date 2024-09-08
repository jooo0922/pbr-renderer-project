#ifndef RENDERABLE_OBJECT_HPP
#define RENDERABLE_OBJECT_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

#include "shader/shader.hpp"

class IRenderableObject
{
public:
  virtual ~IRenderableObject() = default;

  virtual void draw(Shader &shader) = 0;

protected:
  IRenderableObject() = default;
};

#endif // RENDERABLE_OBJECT_HPP
