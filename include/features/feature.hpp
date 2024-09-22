#ifndef FEATURE_HPP
#define FEATURE_HPP

/*
  #ifndef ~ #endif 전처리기는
  헤더파일의 헤더가드를 처리하여 중복 include 방지해 줌!
*/

/**
 * IFeature 인터페이스 클래스
 *
 * 각 Feature 클래스들이 상속받는 인터페이스 클래스.
 */
class IFeature
{
public:
  virtual void initialize() = 0;
  virtual void process() = 0;
  virtual void finalize() = 0;
};

#endif // FEATURE_HPP
