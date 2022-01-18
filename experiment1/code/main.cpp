#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;


// timing
float interval = 2.0f;

class Node {
public:
    Node* father = nullptr;
    std::vector<Node*> sons;
    glm::vec3 pos_0, _pos, color = glm::vec3(1.0f, 1.0f, 1.0f);
    double length = 0.2;
    glm::qua<float> start, now, end;
    glm::vec3 getPos() {
        _pos = father->_pos + glm::vec3(0, 0, father->length) * father->now + pos_0;
    }
    glm::vec3 at() {
        return _pos + now * glm::vec3(0, length, 0);
    }
};

void connect(Node* father, Node* son) {
    father->sons.push_back(son);
    son->father = father;
}

void update(Node* node, float p) {
    node->now = node->father != nullptr ? node->father->now : glm::qua<float>(glm::radians(glm::vec3(0)));
    node->now = node->now * glm::slerp(node->start, node->end, p);
    node->_pos = (node->father != nullptr ? node->father->at() : glm::vec3(0)) + node->pos_0;
    glColor3f(node->color.x, node->color.y, node->color.z);
    glBegin(GL_LINES);
    glVertex3f(node->_pos.x, node->_pos.y, node->_pos.z);
    auto at = node->at();
    glVertex3f(at.x, at.y, at.z);
    glEnd();
    for (auto item : node->sons) {
        update(item, p);
    }
}
int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "exp1", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    Node body, head, armRU, armRD, armLU, armLD, legRU, legRD, legLU, legLD, footL, footR, handL, handR;
    connect(&body, &head);

    connect(&body, &armRU);
    connect(&body, &armLU);
    connect(&body, &legRU);
    connect(&body, &legLU);

    connect(&armRU, &armRD);
    connect(&armLU, &armLD);
    connect(&legRU, &legRD);
    connect(&legLU, &legLD);

    connect(&armRD, &handR);
    connect(&armLD, &handL);
    connect(&legLD, &footL);
    connect(&legRD, &footR);

    body.pos_0 = glm::vec3(0, 0.2, -0.5);
    body.start = glm::qua<float>(glm::radians(glm::vec3(0, 0, 180.0f)));
    body.end = body.start;
    body.length = 0.4;
    body.color = glm::vec3(1.0f, 0, 0);

    armLU.pos_0 = glm::vec3(0, 0.4, 0);
    armLU.start = glm::qua<float>(glm::radians(glm::vec3(-10.0f, 0, -20.0f)));
    armLU.end = glm::qua<float>(glm::radians(glm::vec3(-10.0f, 0, 20.0f)));
    armRU.pos_0 = glm::vec3(0, 0.4, 0);
    armRU.start = glm::qua<float>(glm::radians(glm::vec3(10.0f, 0, 20.0f)));
    armRU.end = glm::qua<float>(glm::radians(glm::vec3(10.0f, 0, -20.0f)));
    armLU.color = glm::vec3(1.0f, 1.0f, 0);
    armRU.color = glm::vec3(1.0f, 1.0f, 0);

    armLD.pos_0 = glm::vec3(0, 0, 0);
    armLD.start = glm::qua<float>(glm::radians(glm::vec3(0, 0, 0)));
    armLD.end = glm::qua<float>(glm::radians(glm::vec3(0, 0, 20.0f)));
    armRD.pos_0 = glm::vec3(0, 0, 0);
    armRD.start = glm::qua<float>(glm::radians(glm::vec3(0, 0, 20.0f)));
    armRD.end = glm::qua<float>(glm::radians(glm::vec3(0, 0, 0)));
    armLD.color = glm::vec3(0, 1.0f, 0);
    armRD.color = glm::vec3(0, 1.0f, 0);

    legLU.pos_0 = glm::vec3(0, 0, 0);
    legLU.start = glm::qua<float>(glm::radians(glm::vec3(10.0f, 0, 30.0f)));
    legLU.end = glm::qua<float>(glm::radians(glm::vec3(10.0f, 0, -30.0f)));
    legRU.pos_0 = glm::vec3(0, 0, 0);
    legRU.start = glm::qua<float>(glm::radians(glm::vec3(-10.0f, 0, -30.0f)));
    legRU.end = glm::qua<float>(glm::radians(glm::vec3(-10.0f, 0, 30.0f)));
    legLU.color = glm::vec3(0, 0, 1.0f);
    legRU.color = glm::vec3(0, 0, 1.0f);

    legLD.pos_0 = glm::vec3(0, 0, 0);
    legLD.start = glm::qua<float>(glm::radians(glm::vec3(0, 0, 0)));
    legLD.end = glm::qua<float>(glm::radians(glm::vec3(0, 0, -20.0f)));
    legRD.pos_0 = glm::vec3(0, 0, 0);
    legRD.start = glm::qua<float>(glm::radians(glm::vec3(0, 0, -20.0f)));
    legRD.end = glm::qua<float>(glm::radians(glm::vec3(0, 0, 0)));
    legLD.color = glm::vec3(0, 1.0f, 1.0f);
    legRD.color = glm::vec3(0, 1.0f, 1.0f);

    head.pos_0 = handL.pos_0 = handR.pos_0 = glm::vec3(0, 0.4, 0);
    head.start = head.end = glm::qua<float>(glm::radians(glm::vec3(0, 0, -180.0f)));
    head.length = handL.length = handR.length = footL.length = footR.length = 0.1;
    handL.start = handL.end = handR.start = handR.end = glm::qua<float>(glm::radians(glm::vec3(0, 0, 0)));
    handL.pos_0 = handR.pos_0 = glm::vec3(0, 0, 0);
    footL.pos_0 = footR.pos_0 = glm::vec3(0, 0, 0);
    footL.start = glm::qua<float>(glm::radians(glm::vec3(0, 0, 20.0f)));
    footL.end = glm::qua<float>(glm::radians(glm::vec3(0, 0, 60.0f)));
    footR.start = glm::qua<float>(glm::radians(glm::vec3(0, 0, 60.0f)));
    footR.end = glm::qua<float>(glm::radians(glm::vec3(0, 0, 20.0f)));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime() / interval;
        currentFrame = (currentFrame - std::floor(currentFrame));
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        /*your draw*/
        glLineWidth(2);
        update(&body, currentFrame);

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
    }
}
