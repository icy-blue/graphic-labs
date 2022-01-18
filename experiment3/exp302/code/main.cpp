#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <cassert>
#include <iostream>
#include <fstream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void generate();
glm::vec3 getPos(glm::vec2 point, double theta);
glm::vec2 getDis(glm::vec2 point);
inline glm::vec2 norm(glm::vec2 p);
inline float length(glm::vec2 p);
inline double dot(glm::vec2 p1, glm::vec2 p2);


// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
const unsigned int NUM = 60;
const double PI = 3.14159265;

bool showObj = false;
std::vector<glm::vec2> inputP;
std::vector<std::vector<glm::vec3>> output;

void draw()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glLineWidth(2);//设置线段宽度


    glColor3f(0.8, 0.8, 0.8);
    if (!showObj) {
        glBegin(GL_LINE_STRIP);
        for (auto v : inputP) {
            glVertex2f(v.x, v.y);
        }
        glEnd();
    }
    else {
        glLineWidth(1);
        for (auto i : output) {
            glBegin(GL_QUAD_STRIP);
            for (auto j : i) {
                glVertex3d(j.x, j.y, j.z);
            }
            glEnd();
        }
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "exp302", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        /*your draw*/

        draw();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS) return;
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GL_TRUE);
        break;
    case GLFW_KEY_SPACE:
        if (showObj) break;
        generate();
        showObj = true;
        break;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action != GLFW_PRESS || showObj) return;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        double xpos, ypos;
        // thanks to https://www.zhihu.com/question/67132881/answer/646860437
        glfwGetCursorPos(window, &xpos, &ypos);
        inputP.emplace_back(xpos / SCR_WIDTH * 2 - 1, ypos / SCR_HEIGHT * -2 + 1);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (inputP.size() > 0) inputP.pop_back();
    }
}

inline double dot(glm::vec2 p1, glm::vec2 p2) {
    return p1.x * p2.x + p1.y * p2.y;
}

inline float length(glm::vec2 p) {
    return (float)std::sqrt(dot(p, p));
}

inline glm::vec2 norm(glm::vec2 p) {
    return p / length(p);
}

glm::vec2 getDis(glm::vec2 point) {
    assert(inputP.size() > 1);
    glm::vec2 to(norm(inputP[inputP.size() - 2] - inputP[0]));
    glm::vec2 v(point - inputP[0]);
    float y = dot(v, to);
    float x = length(v - y * to);
    return glm::vec2(x, y);
}

glm::vec3 getPos(glm::vec2 point, double theta) {
    double y = point.y / 2 - 0.5;
    double x = std::cos(theta) * point.x / 2 - 0.5;
    double z = std::sin(theta) * point.x / 2 - 0.5;
    return glm::vec3(x, y, z);
}

void generate() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    inputP.push_back(inputP[0]);
    std::ofstream out("out.obj");
    out << "g default" << std::endl;
    for (int i = 0; i < inputP.size() - 2; i++) {
        std::vector<glm::vec3> data;
        double interval = 2 * PI / NUM;
        for (double j = 0; j < 2 * PI - 1e-6; j += interval) {
            auto v1 = getPos(getDis(inputP[i]), j);
            auto v2 = getPos(getDis(inputP[i + 1]), j);
            out << "v " << v1.x << " " << v1.y << " " << v1.z << std::endl;
            data.push_back(v1);
            data.push_back(v2);
        }
        data.push_back(getPos(getDis(inputP[i]), 0));
        data.push_back(getPos(getDis(inputP[i + 1]), 0));
        output.push_back(data);
    }
    for (int i = 0; i < inputP.size() - 2; i++) {
        for (int j = 0; j < NUM - 1; j++) {
            out << "f " << i * NUM + j + 1 << " " << (i + 1) * NUM + j + 1 << " " << i * NUM + j + 2 << std::endl;
            out << "f " << i * NUM + j + 2 << " " << (i + 1) * NUM + j + 1 << " " << (i + 1) * NUM + j + 2 << std::endl;
        }
        out << "f " << i * NUM + NUM << " " << (i + 1) * NUM + NUM << " " << i * NUM + 1 << std::endl;
        out << "f " << i * NUM + 1 << " " << (i + 1) * NUM + NUM << " " << (i + 1) * NUM + 1 << std::endl;
    }
    out.close();
}