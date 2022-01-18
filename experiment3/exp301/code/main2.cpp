#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void drawBezierCurves(int num);
void drawBezierCurvesA(int num);
void drawBezierCurvesB(int num);
glm::vec2 bezierInterpolation(double t);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
const unsigned int NUM = 1000;

std::vector<glm::vec2> inputP;
std::vector<glm::vec2> outputP;
std::vector<glm::vec2> tmpP;
bool showRaw = true;
bool circle = false;

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
        break;
   
    case GLFW_KEY_A:
        circle ^= true;
        outputP.clear();
        if (inputP.size() > 2) {
            drawBezierCurves(NUM);
        }
        break;
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
        if (inputP.size() > 2) {
            drawBezierCurves(NUM);
        }
    }
}

glm::vec2 bezierInterpolation(double t) {
    glm::vec2* tmp = new glm::vec2[tmpP.size() + 1];
    for (int i = 1; i < tmpP.size(); i++) {
        for (int j = 0; j < tmpP.size() - i; j++) {
            if (i == 1) {
                tmp[j].x = (float)(tmpP[j].x * (1 - t) + tmpP[j + 1].x * t);
                tmp[j].y = (float)(tmpP[j].y * (1 - t) + tmpP[j + 1].y * t);
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

void drawBezierCurvesA(int num) {
    float step = 1.0 / num;
    glm::vec2 this1(inputP[1] - 0.25f * (inputP[2] - inputP[0]));
    for (int k = 1; k < inputP.size(); k++) {
        float t = 0;
        tmpP.clear();
        tmpP.push_back(inputP[k - 1]);
        tmpP.push_back(this1);
        if (k != inputP.size() - 1) tmpP.emplace_back(inputP[k] - 0.25f * (inputP[k + 1] - inputP[k - 1]));
        else tmpP.push_back(this1);
        tmpP.push_back(inputP[k]);
        if (k != inputP.size() - 1)this1 = inputP[k] + 0.25f * (inputP[k + 1] - inputP[k - 1]);
        for (int i = 0; i < num; i++) {
            glm::vec2 point = bezierInterpolation(t);
            t += step;
            outputP.push_back(point);
        }
    }

}

void drawBezierCurvesB(int num) {
    float step = 1.0 / num;
    glm::vec2 this1(inputP[0] + 0.25f * (inputP[1] - inputP[inputP.size() - 1]));
    inputP.push_back(inputP[0]);
    for (int k = 1; k < inputP.size(); k++) {
        float t = 0;
        tmpP.clear();
        tmpP.push_back(inputP[k - 1]);
        tmpP.push_back(this1);
        if (k != inputP.size() - 1) tmpP.emplace_back(inputP[k] - 0.25f * (inputP[k + 1] - inputP[k - 1]));
        else  tmpP.emplace_back(inputP[k] - 0.25f * (inputP[1] - inputP[k - 1]));
        tmpP.push_back(inputP[k]);
        if (k != inputP.size() - 1)this1 = inputP[k] + 0.25f * (inputP[k + 1] - inputP[k - 1]);
        for (int i = 0; i < num; i++) {
            glm::vec2 point = bezierInterpolation(t);
            t += step;
            outputP.push_back(point);
        }
    }
    inputP.pop_back();
}

void drawBezierCurves(int num) {
    if (circle) drawBezierCurvesB(num);
    else drawBezierCurvesA(num);
}