#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include <iostream>
#include <random>
#include <functional>
#include <cmath>
#include <array>
#include "dependencies/stb_image/stb_image.h"
#include "Renderer.h"
#include "Shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

const std::string vertexShader = R"glsl(
#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

out vec2 outTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(inPosition, 1.0);
    outTexCoord = inTexCoord;
}

)glsl";

const std::string fragmentShader = R"glsl(
#version 330 core

out vec4 FragColor;

in vec2 outTexCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main() {
    vec4 color0 = texture(texture0, outTexCoord);
    vec4 color1 = texture(texture1, outTexCoord);

    float alpha = color1.a; // Use the alpha of the top texture
    vec3 color = mix(color0.rgb, color1.rgb, alpha); // Blend the colors

    FragColor = vec4(color, 1.0);
}

)glsl";

const unsigned int SRC_WIDTH = 800;
const unsigned int SRC_HEIGHT = 600;

int main(void)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    auto rand = std::bind(dis, gen);

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    int version = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (version == 0) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    std::cout << "Loaded OpenGL " << glGetString(GL_VERSION) << std::endl;

    int nrAttributes;
    glCall(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes));
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    stbi_set_flip_vertically_on_load(true);

    //float positions[] {
    //    -0.5f, -0.5f, 0.0f,
    //     0.5f, -0.5f, 0.0f,
    //     0.5f,  0.5f, 0.0f,

    //     0.5f,  0.5f, 0.0f,
    //    -0.5f,  0.5f, 0.0f,
    //    -0.5f, -0.5f, 0.0f
    //};
    std::array<float, 20> positions = {
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
    };

    std::array<unsigned int, 6> indices = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int vao;
    glCall(glGenVertexArrays(1, &vao));
    glCall(glBindVertexArray(vao));

    unsigned int vbo;
    glCall(glGenBuffers(1, &vbo));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    glCall(glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW));

    unsigned int ibo;
    glCall(glGenBuffers(1, &ibo));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));

    glCall(glEnableVertexAttribArray(0));
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0));

    glCall(glEnableVertexAttribArray(1));
    glCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));

    std::array<std::string, 2> textures = { "container.jpg", "awesomeface.png" };
    std::array<unsigned int, textures.size()> textureVBOs = {};
    for (size_t i = 0; i < textures.size(); i++) {
        unsigned int textureVBO;
        glCall(glGenTextures(1, &textureVBO));
        glCall(glActiveTexture(GL_TEXTURE0 + i));
        glCall(glBindTexture(GL_TEXTURE_2D, textureVBO));

        textureVBOs[i] = textureVBO;

        glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        std::string texture = textures[i];
        std::string texturePath = std::string("res/textures/") + texture;
        std::cout << texturePath << std::endl;

        int width, height, nrChannels;
        unsigned char* bytes = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

        unsigned int channel = texture.ends_with("png") ? GL_RGBA : GL_RGB;

        if (bytes) {
            glCall(glTexImage2D(GL_TEXTURE_2D, 0, channel, width, height, 0, channel, GL_UNSIGNED_BYTE, bytes));
        }
        else {
            std::cout << "Failed to load texture: " << texturePath << std::endl;
        }

        stbi_image_free(bytes);
    }

    Shader shader(vertexShader, fragmentShader);
    shader.Bind();
    for (size_t i = 0; i < textureVBOs.size(); i++) {
        auto textureVBO = textureVBOs[i];
        std::string textureLocationName = "texture" + std::to_string(i);
        shader.SetValue(textureLocationName, static_cast<unsigned int>(i));
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SRC_WIDTH / SRC_HEIGHT), 0.1f, 100.0f);

    shader.SetValue("model", model);
    shader.SetValue("view", view);
    shader.SetValue("projection", projection);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        glCall(glClear(GL_COLOR_BUFFER_BIT));
        
        glm::mat4 trans = glm::mat4(1.0f);
        //trans = glm::rotate(trans, -static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));
        //scale = static_cast<float>(glfwGetTime());
        //if (scale > 5.0f) {
        //    scale = 0.0f;
        //}
        //float scale = abs(sin(glfwGetTime())) * 2.0f;
        //trans = glm::scale(trans, glm::vec3(scale, scale, scale));
        //trans = glm::rotate(trans, -static_cast<float>(glfwGetTime()), glm::vec3(1.0f, 0.0f, 0.0f));
        //shader.SetValue("transform", trans);

      /*  float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);*/
     /*   for (size_t i = 0; i < textureVBOs.size(); i++) {
            auto textureVBO = textureVBOs[i];
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textureVBO);
        }*/
        //shader.Use();
   /*     for (size_t i = 0; i < textureVBOs.size(); i++) {
            auto textureVBO = textureVBOs[i];
            std::string textureLocationName = "texture" + std::to_string(i);
            shader.SetValue(textureLocationName, static_cast<unsigned int>(i));

            glCall(glActiveTexture(GL_TEXTURE0 + i)); 
            glCall(glBindTexture(GL_TEXTURE_2D, textureVBO));
        }*/

        //for (size_t i = 0; i < textureVBOs.size(); i++) {
        //    auto textureVBO = textureVBOs[i];
        //    glCall(glActiveTexture(GL_TEXTURE0 + i));
        //    glCall(glBindTexture(GL_TEXTURE_2D, textureVBO));
        //}
        //shader.Use();
        glCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glCall(glDeleteVertexArrays(1, &vao));
    glCall(glDeleteBuffers(1, &vbo));
    glCall(glDeleteBuffers(1, &ibo));
    for (auto& textureVBO : textureVBOs) {
        glCall(glDeleteTextures(1, &textureVBO));
    }
    shader.UnBind();

    glfwTerminate();
    return 0;
}