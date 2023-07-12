#include "Shader.h"
#include <glad/glad.h>
#include <iostream>

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader) :
	m_VertexShader(vertexShader),
	m_FragmentShader(fragmentShader)
{
    unsigned int vs = CreateShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CreateShader(GL_FRAGMENT_SHADER, fragmentShader);

    m_Id = glCreateProgram();
    glAttachShader(m_Id, vs);
    glAttachShader(m_Id, fs);
    glLinkProgram(m_Id);
    glValidateProgram(m_Id);
    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader() {
    glDeleteProgram(m_Id);
}

void Shader::Use() const {
    glUseProgram(m_Id);
}

unsigned int Shader::CreateShader(unsigned int type, const std::string& shaderSource) {
    unsigned int id = glCreateShader(type);
    const char* src = shaderSource.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);

        delete[] message;
        message = nullptr;

        return 0;
    }

    return id;
}

void Shader::SetValue(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(m_Id, name.c_str()), static_cast<int>(value));
}

void Shader::SetValue(const std::string& name, unsigned int value) const {
	glUniform1i(glGetUniformLocation(m_Id, name.c_str()), value);
}

void Shader::SetValue(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(m_Id, name.c_str()), value);
}
