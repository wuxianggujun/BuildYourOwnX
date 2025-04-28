//
// Created by wuxianggujun on 2025/4/18.
//
#include "Config.h"
#include "triangle_mesh.h"
#include "material.h"


unsigned int make_module(const std::string& filePath, unsigned int module_type);
unsigned int make_shader(const std::string& vertex_filePath, const std::string& fragment_filePath);

int main()
{
    GLFWwindow* window;
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW3" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    

    window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        glfwTerminate();
        return -1;
    }

    glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

    int w,h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);

    TriangleMesh* triangle = new TriangleMesh();
    auto* material = new Material("img/marika_matsumoto.jpg");
    
    unsigned int shaderProgram = make_shader("shaders/vertex.vert", "shaders/fragment.frag");


    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        material->use();

        triangle->draw();

        glfwSwapBuffers(window);
    }
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}

unsigned int make_shader(const std::string& vertex_filePath, const std::string& fragment_filePath)
{
    std::vector<unsigned int> shaderModules;
    shaderModules.push_back(make_module(vertex_filePath,GL_VERTEX_SHADER));
    shaderModules.push_back(make_module(fragment_filePath,GL_FRAGMENT_SHADER));

    unsigned int shaderProgram = glCreateProgram();

    for (unsigned int shaderModule : shaderModules)
    {
        glAttachShader(shaderProgram, shaderModule);
    }

    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char errorLog[1024];
        glGetProgramInfoLog(shaderProgram, 1024, nullptr, errorLog);
        std::cout << "Shader Linking failed:\n" << errorLog << std::endl;
    }

    for (unsigned int shaderModule : shaderModules)
    {
        glDeleteShader(shaderModule);
    }
    return shaderProgram;
}

unsigned int make_module(const std::string& filePath, unsigned int module_type)
{
    std::ifstream file;
    std::stringstream bufferedLines;
    std::string line;

    file.open(filePath);

    while (std::getline(file, line))
    {
        bufferedLines << line << "\n";
    }

    std::string shaderSource = bufferedLines.str();
    const char* shaderSrc = shaderSource.c_str();
    bufferedLines.str("");

    file.close();

    unsigned int shaderModule = glCreateShader(module_type);
    glShaderSource(shaderModule, 1, &shaderSrc, nullptr);
    glCompileShader(shaderModule);

    int success;
    glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char errorLog[1024];

        glGetShaderInfoLog(shaderModule, 1024, nullptr, errorLog);
        std::cout << "Shader Module compilation failed:\n" << errorLog << std::endl;
    }
    return shaderModule;
}
