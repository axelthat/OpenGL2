#pragma once

#include <string>

class Shader {
private:
	unsigned int m_Id;
	const std::string& m_VertexShader;
	const std::string& m_FragmentShader;
public:
	Shader(const std::string& vertexShader, const std::string& fragmentShader);
	~Shader();

private:
	unsigned int CreateShader(unsigned int type, const std::string& shaderSource);

public:
	void Use() const;
	void SetValue(const std::string& name, bool value) const;
	void SetValue(const std::string& name, unsigned int value) const;
	void SetValue(const std::string& name, float value) const;
};