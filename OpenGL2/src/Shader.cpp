#include "Shader.h"
#include <glad/glad.h>
#include <iostream>
#include "Renderer.h"

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader) :
	m_VertexShader(vertexShader),
	m_FragmentShader(fragmentShader)
{
    unsigned int vs = CreateShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CreateShader(GL_FRAGMENT_SHADER, fragmentShader);

    glCall(m_Id = glCreateProgram());
    glCall(glAttachShader(m_Id, vs));
    glCall(glAttachShader(m_Id, fs));
    glCall(glLinkProgram(m_Id));
    glCall(glValidateProgram(m_Id));
    glCall(glDeleteShader(vs));
    glCall(glDeleteShader(fs));
}

void Shader::Bind() const {
    glCall(glUseProgram(m_Id));
}

void Shader::UnBind() const {
    glCall(glDeleteProgram(m_Id));
}

unsigned int Shader::CreateShader(unsigned int type, const std::string& shaderSource) {
    glCall(unsigned int id = glCreateShader(type));
    const char* src = shaderSource.c_str();
    glCall(glShaderSource(id, 1, &src, nullptr));
    glCall(glCompileShader(id));

    int result;
    glCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        int length;
        glCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = new char[length];
        glCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glCall(glDeleteShader(id));

        delete[] message;
        message = nullptr;

        return 0;
    }

    return id;
}

int Shader::GetLocation(const std::string& name) const {
    glCall(int location = glGetUniformLocation(m_Id, name.c_str()));
    if (location < 0) {
        std::cout << "Failed to get uniform location: " << name << std::endl;
    }

    return location;
}

void Shader::SetValue(const std::string& name, bool value) const {
	glCall(glUniform1i(GetLocation(name), static_cast<int>(value)));
}

void Shader::SetValue(const std::string& name, unsigned int value) const {
	glCall(glUniform1i(GetLocation(name), value));
}

void Shader::SetValue(const std::string& name, float value) const {
	glCall(glUniform1f(GetLocation(name), value));
}
