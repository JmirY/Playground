#include <graphics/renderer.h>
#include <iostream>
#include <typeinfo>

using namespace graphics;

Renderer::Renderer()
    : windowWidth(WINDOW_WIDTH), windowHeight(WINDOW_HEIGHT),
        sceneWidth(SCENE_WIDTH), sceneHeight(SCENE_HEIGHT)
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

    /* Shader 인스턴스 생성 */
    objectShader = Shader(
        "./shaders/object_vertex.glsl",
        "./shaders/object_fragment.glsl"
    );

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

    /* 월드 y 축 VAO 설정 */
    glGenVertexArrays(1, &worldYaxisVAO);
    glBindVertexArray(worldYaxisVAO);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * CONTACT_NORMAL_VERTICES.size(),
        &CONTACT_NORMAL_VERTICES[0],
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    /* 프레임 버퍼 오브젝트 생성 */
    glGenFramebuffers(1, &sceneFrameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFrameBufferID);

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

    addShape(0, SPHERE);
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

glm::vec3 Renderer::getCameraPosition() const
{
    return camera.getPosition();
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

void Renderer::renderObject(
    unsigned int id,
    glm::vec3 color,
    float modelMatrix[],
    bool isSelected
)
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
    if (id != 0)  // 충돌점이 아닐 때
    {
        glm::vec3 frameColor(0.1f, 0.1f, 0.1f);
        if (isSelected)
            frameColor = glm::vec3(0.0f, 1.0f, 0.0f);
        objectShader.setVec3("objectColor", frameColor);
        glBindVertexArray(objectShape->frameVAO);
        glDrawElements(GL_LINE_STRIP, objectShape->frameIndices.size(), GL_UNSIGNED_INT, (void*)0);
        if (typeid(*objectShape) == typeid(Sphere))
        {
            for (int i = 0; i < 3; ++i)
            {
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                objectShader.setMat4("model", model);
                glDrawElements(GL_LINE_STRIP, objectShape->frameIndices.size(), GL_UNSIGNED_INT, (void*)0);
            }
            model = glm::make_mat4(modelMatrix);
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            objectShader.setMat4("model", model);
            glDrawElements(GL_LINE_STRIP, objectShape->frameIndices.size(), GL_UNSIGNED_INT, (void*)0);
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            objectShader.setMat4("model", model);
            glDrawElements(GL_LINE_STRIP, objectShape->frameIndices.size(), GL_UNSIGNED_INT, (void*)0);
        }
    }

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

void Renderer::renderContactInfo(ContactInfo* info)
{
    /* 충돌점 렌더 */

    /* 변환 행렬 설정 */
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.getFov()),
        ((float) sceneWidth) / sceneHeight,
        PERSPECTIVE_NEAR,
        PERSPECTIVE_FAR
    );
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(info->pointX, info->pointY, info->pointZ));
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

    /* 셰이더 설정 */
    objectShader.use();
    objectShader.setMat4("model", model);
    objectShader.setMat4("view", view);
    objectShader.setMat4("projection", projection);
    objectShader.setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f));
    objectShader.setVec3("viewPos", camera.getPosition());

    Shape *objectShape = shapes.find(0)->second;
    glBindVertexArray(objectShape->polygonVAO);
    glDrawElements(GL_TRIANGLES, objectShape->polygonIndices.size(), GL_UNSIGNED_INT, (void*)0);
    
    /* 충돌 법선 렌더 */
    glm::vec3 defaultNormal(0.0f, 1.0f, 0.0f);
    glm::vec3 contactNormal(info->normalX, info->normalY, info->normalZ);
    glm::vec3 rotateAxis = glm::cross(contactNormal, defaultNormal);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(info->pointX, info->pointY, info->pointZ));
    if (glm::length(rotateAxis) != 0.0f)
    {
        model = glm::rotate(
            model,
            glm::asin(glm::length(rotateAxis)),
            glm::normalize(rotateAxis)
        );
    }
    objectShader.setMat4("model", model);

    glBindVertexArray(worldYaxisVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void Renderer::renderObjectAxis(int axisIdx, float posX, float posY, float posZ)
{
    /* 변환 행렬 설정 */
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.getFov()),
        ((float) sceneWidth) / sceneHeight,
        PERSPECTIVE_NEAR,
        PERSPECTIVE_FAR
    );
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(posX, posY, posZ));
    glm::vec3 color(0.0f, 0.0f, 0.0f);
    switch (axisIdx)
    {
    case 0:  // x 축
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        color.x = 1.0f;
        break;

    case 1:  // y 축
        color.y = 1.0f;
        break;

    case 2:  // z 축
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        color.z = 1.0f;
        break;

    default:
        break;
    }

    /* 셰이더 설정 */
    objectShader.use();
    objectShader.setMat4("model", model);
    objectShader.setMat4("view", view);
    objectShader.setMat4("projection", projection);
    objectShader.setVec3("objectColor", color);
    objectShader.setVec3("viewPos", camera.getPosition());

    glBindVertexArray(worldYaxisVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void Renderer::updateWindowSize()
{
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
}

void Renderer::bindSceneFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFrameBufferID);
}

void Renderer::bindDefaultFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::setSceneViewport()
{
    glViewport(0, 0, sceneWidth, sceneHeight);
}

void Renderer::setWindowViewport()
{
    glViewport(0, 0, windowWidth, windowHeight);
}

void Renderer::moveCamera(glm::vec3 offset)
{
    camera.pan(offset.x, offset.y, offset.z);
}

void Renderer::rotateCamera(glm::vec3 axis, float angle)
{
    camera.rotate(axis, angle);
}

void Renderer::zoomCamera(float degree)
{
    camera.zoom(degree);
}

glm::vec3 Renderer::convertScreenToWorld(glm::vec2 screenPt)
{
    glm::mat4 projectionMat = glm::perspective(
        glm::radians(camera.getFov()),
        ((float) sceneWidth) / sceneHeight,
        PERSPECTIVE_NEAR,
        PERSPECTIVE_FAR
    );
    glm::mat4 viewMat = camera.getViewMatrix();
    glm::mat4 inverseTransformMat = glm::inverse(projectionMat * viewMat);

    glm::vec4 clickedPt;
    clickedPt.w = 1.0f;
    clickedPt.x = screenPt.x / sceneWidth * 2.0f - 1.0f;
    clickedPt.y = screenPt.y / sceneHeight * 2.0f - 1.0f;
    clickedPt.y *= -1.0f;
    clickedPt.z = -1.0f;

    glm::vec4 worldPt = inverseTransformMat * clickedPt;
    worldPt.w = 1.0f / worldPt.w;
    worldPt.x *= worldPt.w;
    worldPt.y *= worldPt.w;
    worldPt.z *= worldPt.w;

    return glm::vec3(worldPt.x, worldPt.y, worldPt.z);
}
