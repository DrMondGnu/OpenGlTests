#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLFWwindow* window;
void init_window() {
    glewExperimental = true; // Needed for core profile
    if (!glfwInit())
    {
        printf("Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow((int) 1366, (int) 768, "Game", nullptr,
                              nullptr);
    if (window == nullptr)
    {
        printf("Failed to open GLFW window.");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        printf("Failed to initialize GLFW");
        return;
    }
}

float vertices[] = {
        // Triangle 1
        -1.0f, -1.0f, // Bottom-Right
        1.0f, -1.0f, // Bottom-Left
        1.0f, 1.0f, // Top-Left
        // Triangle 2
        -1.0f, 1.0f, // Top-Right
        1.0f, 1.0f, // Top-Left
        -1.0f, -1.0f, // Bottom-Right
};

GLuint vbo;
void init_vbo() {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint vao;
void init_vao() {
    init_vbo();
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof (float), nullptr);
    glEnableVertexAttribArray(0);
}

const char* vShader = "#version 430 core\n"
                      "\n"
                      "layout(location = 0) in vec2 vertex;\n"
                      "\n"
                      "uniform mat4 model;\n"
                      "uniform mat4 projection;\n"
                      "\n"
                      "void main() {\n"
                      "    gl_Position = projection * model * vec4(vertex.xy, 0.0f, 1.0f);\n"
                      "}";
const char* fShader = "#version 430 core\n"
                      "\n"
                      "out vec4 outColor;\n"
                      "\n"
                      "uniform vec3 color;\n"
                      "\n"
                      "void main() {\n"
                      "        outColor = vec4(color, 1.0);\n"
                      "}";
GLuint shader;
void init_shader() {
    GLint Result = GL_FALSE;
    int InfoLogLength = 0;

    GLuint vShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vShaderId, 1, &vShader, nullptr);
    glCompileShader(vShaderId);

    // Check vertex shader
    glGetShaderiv(vShaderId, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(vShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        char* msg = new char[InfoLogLength+1];
        glGetShaderInfoLog(vShaderId, InfoLogLength, nullptr, msg);
        printf("%s\n", msg);
        delete[] msg;
    }


    glShaderSource(fShaderId, 1, &fShader, nullptr);
    glCompileShader(fShaderId);

    // Check fragment shader
    glGetShaderiv(fShaderId, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(fShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        char* msg = new char[InfoLogLength+1];
        glGetShaderInfoLog(fShaderId, InfoLogLength, nullptr, msg);
        printf("%s\n", msg);
        delete[] msg;
    }

    shader = glCreateProgram();
    glAttachShader(shader, vShaderId);
    glAttachShader(shader, fShaderId);
    glLinkProgram(shader);



    glGetProgramiv(shader, GL_LINK_STATUS, &Result);
    glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        char* msg = new char[InfoLogLength+1];
        glGetProgramInfoLog(shader, InfoLogLength, nullptr, msg);
        printf("%s\n", msg);
        delete[] msg;
    }

    glDeleteShader(vShaderId);
    glDeleteShader(fShaderId);

    // Set projection matrix
    glUseProgram(shader);
    auto projection = glm::ortho(0.0f, 1366.0f, 768.0f, 0.0f, -1.0f, 1.0f);
    GLint loc = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));


}

glm::mat4 gen_matrix(glm::vec2 position, glm::vec2 size, float rotation) {
    glUseProgram(shader);
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));
    return model;
}

[[noreturn]] void loop() {
    glUseProgram(shader);
    // Set color
    glm::vec3 color(1.0f, 0.0f, 0.0f);
    GLint colorPos = glGetUniformLocation(shader, "color");
    glUniform3f(colorPos, color.x, color.y, color.z);


    GLint modelMatPos = glGetUniformLocation(shader, "model");
    glm::vec2 pos(200.0f, 200.0f);
    glm::vec2 size(40.0f, 40.0f);

    glBindVertexArray(vao);
    while(true) {
        glfwPollEvents();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniformMatrix4fv(modelMatPos, 1, GL_FALSE, glm::value_ptr(gen_matrix(pos, size, 0)));
        pos += 1;
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(window);
    }
}

int main()
{

    init_window();
    init_vao();
    init_shader();
    loop();
    return 0;
}
