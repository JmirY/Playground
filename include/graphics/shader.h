#ifndef SHADER_H
#define SHADER_H

#include "opengl/glm/gtc/type_ptr.hpp"
#include <string>

namespace graphics
{
    class Shader
    {
    public:
        /* 셰이더 프로그램 ID */
        unsigned int ID;

    public:
        /* 생성자 */
        /* 셰이더 소스들의 경로를 인자로 받아 빌드한다 */
        Shader(const char* vertexPath, const char* fragmentPath);

        /* 셰이더를 활성화 한다 */
        void use();

        /* 유니폼 값을 설정한다 */
        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec3(const std::string& name, glm::vec3 value) const;
        void setMat4(const std::string& name, glm::mat4 value) const;
    };
} // namespace graphics


#endif // SHADER_H
