#include <graphics/renderer.h>
#include <iostream>

using namespace graphics;

/***********************
 * static 멤버 변수 초기화 *
 ***********************/

int Renderer::windowWidth = WINDOW_WIDTH;
int Renderer::windowHeight = WINDOW_HEIGHT;
Camera* Renderer::camera = new Camera;

/***************
 * 멤버 함수 정의 *
 ***************/

Renderer::Renderer()
{
    /* GLFW 초기화 */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /* window 생성 */
    window = glfwCreateWindow(windowWidth, windowHeight, "Playground", NULL, NULL);
    if (!window)
    {
        std::cout << "ERROR::Renderer::Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);

    /* GLAD 초기화 */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "ERROR::Renderer::Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        exit(1);
    }

    /* Depth test 활성화 */
    glEnable(GL_DEPTH_TEST);
    /* Anti-aliasing 활성화 */
    glEnable(GL_LINE_SMOOTH);

    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    /* 배경 VAO 설정 */
    glGenVertexArrays(1, &backgroundVAO);
    glBindVertexArray(backgroundVAO);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * GRID_VERTICES.size(),
        &GRID_VERTICES[0],
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    /* Shader 인스턴스 생성 */
    objectShader = new Shader(
        "./shaders/object_vertex.glsl",
        "./shaders/object_fragment.glsl"
    );

    /* 콜백 함수 등록 */
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
}

Renderer::~Renderer()
{
    /* shapes 원소들 해제 */
    for (auto shape : shapes)
    {
        delete shape.second;
    }
    /* Camera & Shader 인스턴스 해제 */
    delete camera;
    delete objectShader;

    glfwTerminate();
}

Shape* Renderer::addShape(unsigned int id, Geometry geometry)
{
    Shape* newShape;

    if (geometry == SPHERE)
        newShape = new Sphere;
    else if (geometry == BOX)
        newShape = new Box;

    shapes[id] = newShape;
    return newShape;
}

void Renderer::removeShape(unsigned int id)
{
    Shapes::iterator shapeIter = shapes.find(id);

    delete shapeIter->second;
    shapes.erase(shapeIter);
}

void Renderer::renderObject(unsigned int id, glm::vec3 color, float modelMatrix[])
{
    /* 변환 행렬 설정 */
    glm::mat4 model = glm::make_mat4(modelMatrix);
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(camera->getFov()),
        ((float) windowWidth) / windowHeight,
        PERSPECTIVE_NEAR,
        PERSPECTIVE_FAR
    );

    /* 셰이더 설정 */
    objectShader->use();
    objectShader->setMat4("model", model);
    objectShader->setMat4("view", view);
    objectShader->setMat4("projection", projection);
    objectShader->setVec3("objectColor", color);
    objectShader->setVec3("viewPos", camera->getPosition());

    /* 오브젝트 표면 렌더 */
    Shape *objectShape = shapes.find(id)->second;
    // std::cout << "DEBUG::Renderer::size of object's vertices = " << object->vertices.size() << std::endl; // debug
    glBindVertexArray(objectShape->polygonVAO);
    glDrawElements(GL_TRIANGLES, objectShape->polygonIndices.size(), GL_UNSIGNED_INT, (void*)0);

    /* 오브젝트 테두리 렌더 */
    objectShader->setVec3("objectColor", glm::vec3(0.0f, 0.0f, 0.0f));
    glBindVertexArray(objectShape->frameVAO);
    glDrawElements(GL_LINE_STRIP, objectShape->frameIndices.size(), GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(0);
}

void Renderer::renderBackground()
{
    /* 변환 행렬 설정 */
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(camera->getFov()),
        ((float) windowWidth) / windowHeight,
        PERSPECTIVE_NEAR,
        PERSPECTIVE_FAR
    );

    /* 셰이더 설정 */
    objectShader->use();
    objectShader->setMat4("view", view);
    objectShader->setMat4("projection", projection);
    objectShader->setVec3("objectColor", glm::vec3(0.0f, 0.0f, 0.0f));
    objectShader->setVec3("viewPos", camera->getPosition());

    /* 직선을 translate 하며 grid 렌더 */
    glBindVertexArray(backgroundVAO);
    for (float gap = 0.0f; gap < 100.0f; gap += GRID_GAP)
    {
        /* 원점을 지나는 선만 진하게 표시 */
        if (gap == GRID_GAP)
            objectShader->setVec3("objectColor", glm::vec3(0.35f, 0.35f, 0.35f));

        /* +X축 방향 */
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(gap, 0.0f, 0.0f));
        objectShader->setMat4("model", model);
        glDrawArrays(GL_LINES, 0, 2);
        /* -X축 방향 */
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-gap, 0.0f, 0.0f));
        objectShader->setMat4("model", model);
        glDrawArrays(GL_LINES, 0, 2);
        /* +Z축 방향 */
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, gap));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        objectShader->setMat4("model", model);
        glDrawArrays(GL_LINES, 0, 2);
        /* -Z축 방향 */
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -gap));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        objectShader->setMat4("model", model);
        glDrawArrays(GL_LINES, 0, 2);
    }
    glBindVertexArray(0);
}

void Renderer::updateWindowSize()
{
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
}

void Renderer::cursorPosCallback(GLFWwindow *window, double xPos, double yPos)
{
    /* 커서가 윈도우 상에 있지 않으면 종료 */
    if (!glfwGetWindowAttrib(window, GLFW_HOVERED))
        return;

    /* 직전 호출에서의 커서 위치 저장 */
    static double xPosPrev = WINDOW_WIDTH / 2.0f;
    static double yPosPrev = WINDOW_HEIGHT / 2.0f;
    /* 커서의 이동 거리 */
    double xOffset = xPos - xPosPrev;
    double yOffset = yPosPrev - yPos;
    /* 커서가 이동하지 않았다면 종료 */
    if (xOffset == 0 && yOffset == 0)
        return;

    /* 왼클릭 -> 카메라 panning */
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        camera->pan(xOffset, yOffset);
    }
    /* 오른클릭 -> 카메라 회전 */
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        glm::vec3 end = convertScreenToWorld(glm::vec2(xPos, yPos));
        glm::vec3 start = convertScreenToWorld(glm::vec2(xPosPrev, yPosPrev));
        camera->rotate(start, end);
    }

    xPosPrev = xPos;
    yPosPrev = yPos;
}

void Renderer::mouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset)
{
    camera->zoom((float) yOffset);
}

glm::vec3 Renderer::convertScreenToWorld(glm::vec2 screenPt)
{
    /* 스크린 좌표계 -> 뷰 좌표계 */
    glm::vec3 viewPt = glm::vec3(
        (float) screenPt.x / windowWidth * 2 - 1.0f,
		(float) screenPt.y / windowHeight * 2 - 1.0f,
		0.0f
    );
    viewPt.y = -viewPt.y;
    float lengthSquared = viewPt.x * viewPt.x + viewPt.y * viewPt.y;
    if (lengthSquared <= 1.0f)
    {
        viewPt.z = glm::sqrt(1.0f - lengthSquared);
    }
    else
    {
        viewPt = glm::normalize(viewPt);
    }

    /* 뷰 좌표계 -> 월드 좌표계 */
    glm::vec3 worldPt;
    worldPt = glm::inverse(camera->getViewMatrix()) * glm::vec4(viewPt, 1.0f);

    return worldPt;
}
