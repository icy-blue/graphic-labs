#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void drawBezierCurves(int num);
glm::vec2 bezierInterpolation(double t);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
const unsigned int NUM = 12345;

std::vector<glm::vec2> inputP;
std::vector<glm::vec2> outputP;
bool showRaw = true;

void draw()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glLineWidth(2);//设置线段宽度

    if (showRaw) {
        glColor3f(1.0, 0.1, 0.2);
        glBegin(GL_LINE_STRIP);
        for (auto v : inputP) {
            glVertex2f(v.x / SCR_WIDTH * 2 - 1, v.y / SCR_HEIGHT * -2 + 1);
        }
        glEnd();
    }

    glColor3f(0.8, 0.8, 0.2);
    glBegin(GL_LINE_STRIP);
    for (auto v : outputP) {
        glVertex2f(v.x / SCR_WIDTH * 2 - 1, v.y / SCR_HEIGHT * -2 + 1);
    }
    glEnd();

}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "exp301", NULL, NULL);
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
        showRaw ^= true;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action != GLFW_PRESS) return;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        double xpos, ypos;
        // thanks to https://www.zhihu.com/question/67132881/answer/646860437
        glfwGetCursorPos(window, &xpos, &ypos);
        inputP.emplace_back(xpos, ypos);
        if (inputP.size() > 2) {
            outputP.clear();
            drawBezierCurves(NUM);
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (inputP.size() > 0) inputP.pop_back();
        outputP.clear();
        drawBezierCurves(NUM);
    }
}

glm::vec2 bezierInterpolation(double t) {
    glm::vec2* tmp = new glm::vec2[inputP.size() + 1];
    for (int i = 1; i < inputP.size(); i++) {
        for (int j = 0; j < inputP.size() - i; j++) {
            if (i == 1) {
                tmp[j].x = (float)(inputP[j].x * (1 - t) + inputP[j + 1].x * t);
                tmp[j].y = (float)(inputP[j].y * (1 - t) + inputP[j + 1].y * t);
                continue;
            }
            tmp[j].x = (float)(tmp[j].x * (1 - t) + tmp[j + 1].x * t);
            tmp[j].y = (float)(tmp[j].y * (1 - t) + tmp[j + 1].y * t);
        }
    }
    glm::vec2 result(tmp[0].x, tmp[0].y);
    delete tmp;
    return result;
}

void drawBezierCurves(int num) {
    float step = 1.0 / num;
    float t = 0;
    for (int i = 0; i < num; i++) {
        glm::vec2 point = bezierInterpolation(t);
        t += step;
        outputP.push_back(point);
    }
}