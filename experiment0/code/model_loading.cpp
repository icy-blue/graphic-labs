#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/quaternion_common.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
glm::qua<float> qf = glm::qua<float>(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)));
glm::qua<float> qt = glm::qua<float>(glm::radians(glm::vec3(0.0f, 180.0f, 55.0f)));
glm::vec4 startP(-0.1f), endP(0.1f);

int type = 0; // 123 for initial position xyz, 456 for initial rotation of xyz
// 789 for destination position xyz, 10-12 for destination rotaion of xyz

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// timing
float interval = 6.0f;
bool is_stop = false;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "exp0", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("1.model_loading.vs", "1.model_loading.fs");

    // load models
    // -----------
    Model ourModel(FileSystem::getPath("../obj/a.obj"));

    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime() / interval;
        currentFrame = (currentFrame - std::floor(currentFrame));

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // transformations
        glm::mat4 rotation = glm::mat4_cast(glm::slerp(qf, qt, currentFrame));
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("rotation", rotation);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        float moveX = startP.x + currentFrame * (endP.x - startP.x);
        float moveY = startP.z + currentFrame * (endP.y - startP.y);
        float moveZ = startP.x + currentFrame * (endP.z - startP.z);
        model = glm::translate(model, glm::vec3(moveX, moveY, moveZ)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS) return;
    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_KP_ADD:
        case GLFW_KEY_UP: {
            switch (type) {
                case 0: 
                    break;
                case 1:
                    startP.x += 0.1;
                    break;
                case 2:
                    startP.y += 0.1;
                    break;
                case 3:
                    startP.z += 0.1;
                    break;
                case 4:
                    qf *= glm::qua<float> (glm::radians(glm::vec3(55.0f, 0.0f, 0.0f)));
                    break;
                case 5:
                    qf *= glm::qua<float> (glm::radians(glm::vec3(0.0f, 55.0f, 0.0f)));
                    break;
                case 6:
                    qf *= glm::qua<float> (glm::radians(glm::vec3(0.0f, 0.0f, 55.0f)));
                    break;
                case 7:
                    endP.x += 0.1;
                    break;
                case 8:
                    endP.y += 0.1;
                    break;
                case 9:
                    endP.z += 0.1;
                    break;
                case 10:
                    qt *= glm::qua<float>(glm::radians(glm::vec3(55.0f, 0.0f, 0.0f)));
                    break;
                case 11:
                    qt *= glm::qua<float>(glm::radians(glm::vec3(0.0f, 55.0f, 0.0f)));
                    break;
                case 12:
                    qt *= glm::qua<float>(glm::radians(glm::vec3(0.0f, 0.0f, 55.0f)));
            }
            break;
        }
        case GLFW_KEY_KP_SUBTRACT:
        case GLFW_KEY_DOWN:
            switch (type) {
            case 0:
                break;
            case 1:
                startP.x -= 0.1;
                break;
            case 2:
                startP.y -= 0.1;
                break;
            case 3:
                startP.z -= 0.1;
                break;
            case 4:
                qf *= glm::qua<float> (glm::radians(glm::vec3(-55.0f, 0.0f, 0.0f)));
                break;
            case 5:
                qf *= glm::qua<float> (glm::radians(glm::vec3(0.0f, -55.0f, 0.0f)));
                break;
            case 6:
                qf *= glm::qua<float> (glm::radians(glm::vec3(0.0f, 0.0f, -55.0f)));
                break;
            case 7:
                endP.x -= 0.1;
                break;
            case 8:
                endP.y -= 0.1;
                break;
            case 9:
                endP.z -= 0.1;
                break;
            case 10:
                qt *= glm::qua<float>(glm::radians(glm::vec3(-55.0f, 0.0f, 0.0f)));
                break;
            case 11:
                qt *= glm::qua<float>(glm::radians(glm::vec3(0.0f, -55.0f, 0.0f)));
                break;
            case 12:
                qt *= glm::qua<float>(glm::radians(glm::vec3(0.0f, 0.0f, -55.0f)));
            }
            break;
        case GLFW_KEY_Q:
            type = 1;
            break;
        case GLFW_KEY_W:
            type = 2;
            break;
        case GLFW_KEY_E:
            type = 3;
            break;
        case GLFW_KEY_R:
            type = 4;
            break;
        case GLFW_KEY_T:
            type = 5;
            break;
        case GLFW_KEY_Y:
            type = 6;
            break;
        case GLFW_KEY_A:
            type = 7;
            break;
        case GLFW_KEY_S:
            type = 8;
            break;
        case GLFW_KEY_D:
            type = 9;
            break;
        case GLFW_KEY_F:
            type = 10;
            break;
        case GLFW_KEY_G:
            type = 11;
            break;
        case GLFW_KEY_H:
            type = 12;
            break;
    }
}
