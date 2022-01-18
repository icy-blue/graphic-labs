#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <cstring>
#include <fstream>
#include <regex>
#include <utility>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void generate();

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

std::map<std::pair<int, int>, int> map;
std::vector<glm::vec3> points;
std::vector<glm::vec2> edges;
struct face {
    std::vector<int> points;
    face(std::vector<int> &rt) :points(rt) {}
    face() = default;
};

std::vector<face> faces;
std::vector<glm::vec2> inputP;
bool showObj = false;

void readObj(std::string filename, glm::vec3 scale, glm::vec3 translation) {
    std::ifstream input(filename);
    std::string tmp;
    
    while (!input.eof() && !input.fail()) {
        std::getline(input, tmp);
        std::regex ws_re("\\s+");
        std::vector<std::string> v(
            std::sregex_token_iterator(tmp.begin(), tmp.end(), ws_re, -1),
            std::sregex_token_iterator());
        if (v[0] == "v") {
            double x, y, z;
            x = std::stod(v[1]);
            y = std::stod(v[2]);
            z = std::stod(v[3]);
            points.push_back(glm::vec3(x, y, z) * scale + translation);
        }
        else if (v[0] == "f") {
            face face;
            int last = 0, first = 0;
            for (int i = 1; i < v.size(); i++) {
                int w = std::stoi(v[i]);
                if (last != 0) {
                    map[std::make_pair(last, w)]++;
                }
                else first = w;
                last = w;
                face.points.push_back(w - 1);
            }
            map[std::make_pair(last, first)]++;
            faces.push_back(face);
        }
    }
    for (auto &it : map) {
        if (it.second <= 0) continue;
        if (map[std::make_pair(it.first.second, it.first.first)] <= 0) {
            edges.emplace_back(it.first.first, it.first.second);
            std::cout << it.first.first << " " << it.first.second << std::endl;
        }
    }
}

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
        for (auto i : faces) {
            glBegin(GL_POLYGON);
            for (auto j : i.points) {
                glVertex3d(points[j].x, points[j].y, points[j].z);
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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "exp303", NULL, NULL);
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
    
    readObj("C:\\Users\\i\\Documents\\GitHub\\experiments-icy-blue\\experiment3\\exp303\\obj\\c.obj", 
        glm::vec3(0.2, 0.5, 0.2), glm::vec3(-0.5, -0.5, -1));
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

void generate() {
    std::ofstream out("out.obj");
    out << "g default" << std::endl;
    int origin = points.size();
    auto dir0 = inputP[inputP.size() - 1] - inputP[0];
    for (int i = 0; i < origin; i++) {
        points.push_back(points[i] + glm::vec3(dir0.x, dir0.y, 0));
    }
    int face_cnt = faces.size();
    for (int i = 0; i < face_cnt; i++) {
        std::vector<int> temp;
        for (int x : faces[i].points) {
            temp.push_back(x + origin);
        }
        std::reverse(faces[i].points.begin(), faces[i].points.end());
        faces.emplace_back(temp);
    }
    for (int i = 0; i < inputP.size() - 1; i++) {
        std::vector<glm::vec3> data;
        auto dir = inputP[i + 1] - inputP[i];
        for (int j = 0; j < edges.size(); j++) {
            points.push_back(points[edges[j].x - 1] + glm::vec3(dir.x, dir.y, 0));
            points.push_back(points[edges[j].y - 1] + glm::vec3(dir.x, dir.y, 0));
            std::vector<int> temp;
            temp.push_back(edges[j].x - 1);
            temp.push_back(edges[j].y - 1);
            edges[j].x = points.size() - 1;
            edges[j].y = points.size() - 2;
            temp.push_back(points.size() - 1);
            temp.push_back(points.size() - 2);
            faces.push_back(face(temp));
        }
    }
    for (auto item : points) {
        out << "v " << item.x << " " << item.y << " " << item.z << std::endl;
    }
    for (auto item : faces) {
        out << "f ";
        for (auto p : item.points) {
            out << p + 1 << " ";
        }
        out << std::endl;
    }
    out.close();
}