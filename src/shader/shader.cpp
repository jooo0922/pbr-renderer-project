#include "shader/shader.hpp"

// Shader 클래스 생성자
Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath)
{
  // 쉐이더 코드를 std::string 타입으로 파싱하여 저장할 변수 선언
  std::string vertexCode;
  std::string fragmentCode;

  // std::ifstream을 사용하여 파일 읽기
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;

  // 파일 열기와 예외 처리
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try
  {
    // 파일 열기
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);

    // 파일 스트림을 문자열로 읽기
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    // 파일 스트림 닫기
    vShaderFile.close();
    fShaderFile.close();

    // 문자열로 파싱
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  }
  catch (std::ifstream::failure &e)
  {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
  }

  // C 스타일 문자열로 변환
  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  // 쉐이더 객체 생성 및 컴파일
  unsigned int vertex, fragment;

  // 버텍스 쉐이더 생성 및 컴파일
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");

  // 프래그먼트 쉐이더 생성 및 컴파일
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");

  // 쉐이더 프로그램 객체 생성 및 쉐이더 객체 연결
  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);
  checkCompileErrors(ID, "PROGRAM");

  // 쉐이더 객체 삭제
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

// Shader 클래스 소멸자
Shader::~Shader()
{
  // 쉐이더 프로그램 객체 메모리 반납
  glDeleteProgram(ID);
}

// ShaderProgram 객체 활성화
void Shader::use()
{
  glUseProgram(ID);
}

// 유니폼 변수 관련 유틸리티
void Shader::setBool(const std::string &name, bool value) const
{
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// 쉐이더 객체 및 쉐이더 프로그램 객체의 컴파일 및 링킹 에러 대응
void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
  int success;
  char infoLog[1024];

  if (type != "PROGRAM")
  {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
  }
  else
  {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
  }
}
