#include <graphics/renderer.h>
#include <iostream>

using namespace graphics;

/***********************
 * static 멤버 변수 초기화 *
 ***********************/

int Renderer::sceneWidth = SCENE_WIDTH;
int Renderer::sceneHeight = SCENE_HEIGHT;
Camera Renderer::camera = Camera();

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
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Playground", NULL, NULL);
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

    /* Shader 인스턴스 생성 */
    objectShader = Shader(
        "./shaders/object_vertex.glsl",
        "./shaders/object_fragment.glsl"
    );

    /* 콜백 함수 등록 */
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);

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

    /* 프레임 버퍼 오브젝트 생성 */
    glGenFramebuffers(1, &frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

    /* 텍스처 버퍼 오브젝트 생성 */
    glGenTextures(1, &textureBufferID);
    glBindTexture(GL_TEXTURE_2D, textureBufferID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sceneWidth, sceneHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    /* 프레임 버퍼 오브젝트에 텍스처 버퍼 오브젝트 바인드 */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBufferID, 0);

    /* 렌더 버퍼 오브젝트 생성 */
    unsigned int renderBufferID;
    glGenRenderbuffers(1, &renderBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, sceneWidth, sceneHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    /* 프레임 버퍼 오브젝트에 렌더 버퍼 오브젝트 바인드 */
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferID);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::Renderer::Framebuffer is not complete" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
}

Renderer::~Renderer()
{
    /* shapes 원소들 해제 */
    for (auto& shape : shapes)
    {
        delete shape.second;
    }

    glfwTerminate();
}

GLFWwindow* Renderer::getWindow() const
{
    return window;
}

unsigned int Renderer::getTextureBufferID() const
{
    return textureBufferID;
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
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.getFov()),
        ((float) sceneWidth) / sceneHeight,
        PERSPECTIVE_NEAR,
        PERSPECTIVE_FAR
    );

    /* 셰이더 설정 */
    objectShader.use();
    objectShader.setMat4("model", model);
    objectShader.setMat4("view", view);
    objectShader.setMat4("projection", projection);
    objectShader.setVec3("objectColor", color);
    objectShader.setVec3("viewPos", camera.getPosition());

    /* 오브젝트 표면 렌더 */
    Shape *objectShape = shapes.find(id)->second;
    glBindVertexArray(objectShape->polygonVAO);
    glDrawElements(GL_TRIANGLES, objectShape->polygonIndices.size(), GL_UNSIGNED_INT, (void*)0);

    /* 오브젝트 테두리 렌더 */
    objectShader.setVec3("objectColor", glm::vec3(0.0f, 0.0f, 0.0f));
    glBindVertexArray(objectShape->frameVAO);
    glDrawElements(GL_LINE_STRIP, objectShape->frameIndices.size(), GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(0);
}

void Renderer::renderBackground()
{
    /* 변환 행렬 설정 */
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.getFov()),
        ((float) sceneWidth) / sceneHeight,
        PERSPECTIVE_NEAR,
        PERSPECTIVE_FAR
    );

    /* 셰이더 설정 */
    objectShader.use();
    objectShader.setMat4("view", view);
    objectShader.setMat4("projection", projection);
    objectShader.setVec3("objectColor", glm::vec3(0.0f, 0.0f, 0.0f));
    objectShader.setVec3("viewPos", camera.getPosition());

    /* 직선을 translate 하며 grid 렌더 */
    glBindVertexArray(backgroundVAO);
    for (float gap = 0.0f; gap < 100.0f; gap += GRID_GAP)
    {
        /* 원점을 지나는 선만 진하게 표시 */
        if (gap == GRID_GAP)
            objectShader.setVec3("objectColor", glm::vec3(0.35f, 0.35f, 0.35f));

        /* +X축 방향 */
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(gap, 0.0f, 0.0f));
        objectShader.setMat4("model", model);
        glDrawArrays(GL_LINES, 0, 2);
        /* -X축 방향 */
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-gap, 0.0f, 0.0f));
        objectShader.setMat4("model", model);
        glDrawArrays(GL_LINES, 0, 2);
        /* +Z축 방향 */
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, gap));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        objectShader.setMat4("model", model);
        glDrawArrays(GL_LINES, 0, 2);
        /* -Z축 방향 */
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -gap));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        objectShader.setMat4("model", model);
        glDrawArrays(GL_LINES, 0, 2);
    }
    glBindVertexArray(0);
}

void Renderer::updateWindowSize()
{
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
}

void Renderer::bindFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    glViewport(0, 0, sceneWidth, sceneHeight);
}

void Renderer::unbindFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);
}

void Renderer::cursorPosCallback(GLFWwindow *window, double xPos, double yPos)
{    
    static bool isLeftButtonClickedOutside = false;
    static bool isRightButtonClickedOutside = false;

    /* Scene 외부를 클릭하여 드래그하였는지 검사 */
    if ((xPos > (double) sceneWidth || yPos > (double) sceneHeight)
            && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        isLeftButtonClickedOutside = true;
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        isLeftButtonClickedOutside = false;
    
    if ((xPos > (double) sceneWidth || yPos > (double) sceneHeight)
            && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        isRightButtonClickedOutside = true;
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
        isRightButtonClickedOutside = false;

    if (isLeftButtonClickedOutside || isRightButtonClickedOutside)
        return;

    /* 직전 호출에서의 커서 위치 저장 */
    static double xPosPrev = SCENE_WIDTH / 2.0f;
    static double yPosPrev = SCENE_HEIGHT / 2.0f;
    /* 커서의 이동 거리 */
    double xOffset = xPos - xPosPrev;
    double yOffset = yPosPrev - yPos;
    /* 커서가 이동하지 않았다면 종료 */
    if (xOffset == 0 && yOffset == 0)
        return;

    /* 왼클릭 -> 카메라 panning */
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {        
        camera.pan(xOffset, yOffset);
    }
    /* 오른클릭 -> 카메라 회전 */
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        glm::vec3 end = convertScreenToWorld(glm::vec2(xPos, yPos));
        glm::vec3 start = convertScreenToWorld(glm::vec2(xPosPrev, yPosPrev));
        camera.rotate(start, end);
    }

    xPosPrev = xPos;
    yPosPrev = yPos;
}

void Renderer::mouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset)
{
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    if (xPos > sceneWidth || yPos > sceneHeight)
        return;

    camera.zoom((float) yOffset);
}

glm::vec3 Renderer::convertScreenToWorld(glm::vec2 screenPt)
{
    /* 스크린 좌표계 -> 뷰 좌표계 */
    glm::vec3 viewPt = glm::vec3(
        (float) screenPt.x / sceneWidth * 2 - 1.0f,
		(float) screenPt.y / sceneHeight * 2 - 1.0f,
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
    worldPt = glm::inverse(camera.getViewMatrix()) * glm::vec4(viewPt, 1.0f);

    return worldPt;
}
