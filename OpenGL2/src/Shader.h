#pragma once

#include <string>
#include "glm/glm.hpp"

class Shader {
private:
	unsigned int m_Id;
	const std::string& m_VertexShader;
	const std::string& m_FragmentShader;
public:
	Shader(const std::string& vertexShader, const std::string& fragmentShader);

private:
	unsigned int CreateShader(unsigned int type, const std::string& shaderSource);
	int GetLocation(const std::string& name) const;

public:
	void Bind() const;
	void UnBind() const;
	void SetValue(const std::string& name, bool value) const;
	void SetValue(const std::string& name, unsigned int value) const;
	void SetValue(const std::string& name, float value) const;
	void SetValue(const std::string& name, glm::mat4 value) const;
};