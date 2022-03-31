//
// Created by Sam on 3/14/2022.
//

#ifndef ELYS_SHADER_HPP
#define ELYS_SHADER_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <Core/Logger.hpp>

namespace Elys {
    class Shader {
      public:
        // constructor generates the shader on the fly
        // ------------------------------------------------------------------------
        Shader(const char *vertexPath, const char *fragmentPath,
               const char *geometryPath = nullptr);
        // activate the shader
        // ------------------------------------------------------------------------
        inline void Use() const { glUseProgram(mID); }
        // utility uniform functions
        // ------------------------------------------------------------------------
        inline void SetBool(const std::string &name, bool value) const {
            glUniform1i(glGetUniformLocation(mID, name.c_str()), (int)value);
        }
        // ------------------------------------------------------------------------
        inline void SetInt(const std::string &name, int value) const {
            glUniform1i(glGetUniformLocation(mID, name.c_str()), value);
        }
        // ------------------------------------------------------------------------
        inline void SetFloat(const std::string &name, float value) const {
            glUniform1f(glGetUniformLocation(mID, name.c_str()), value);
        }
        // ------------------------------------------------------------------------
        inline void SetVec2(const std::string &name, const glm::vec2 &value) const {
            glUniform2fv(glGetUniformLocation(mID, name.c_str()), 1, &value[0]);
        }
        inline void SetVec2(const std::string &name, float x, float y) const {
            glUniform2f(glGetUniformLocation(mID, name.c_str()), x, y);
        }
        // ------------------------------------------------------------------------
        inline void SetVec3(const std::string &name, const glm::vec3 &value) const {
            glUniform3fv(glGetUniformLocation(mID, name.c_str()), 1, &value[0]);
        }
        inline void SetVec3(const std::string &name, float x, float y, float z) const {
            glUniform3f(glGetUniformLocation(mID, name.c_str()), x, y, z);
        }
        // ------------------------------------------------------------------------
        inline void SetVec4(const std::string &name, const glm::vec4 &value) const {
            glUniform4fv(glGetUniformLocation(mID, name.c_str()), 1, &value[0]);
        }
        inline void SetVec4(const std::string &name, float x, float y, float z, float w) const {
            glUniform4f(glGetUniformLocation(mID, name.c_str()), x, y, z, w);
        }
        // ------------------------------------------------------------------------
        inline void SetMat2(const std::string &name, const glm::mat2 &mat) const {
            glUniformMatrix2fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // ------------------------------------------------------------------------
        inline void SetMat3(const std::string &name, const glm::mat3 &mat) const {
            glUniformMatrix3fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // ------------------------------------------------------------------------
        inline void SetMat4(const std::string &name, const glm::mat4 &mat) const {
            glUniformMatrix4fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

        [[nodiscard]] inline unsigned int GetID() const { return mID; }

      private:
        unsigned int mID;
        // utility function for checking shader compilation/linking errors.
        // ------------------------------------------------------------------------
        void CheckCompileErrors(GLuint shader, std::string type);
    };
}

#endif // ELYS_SHADER_HPP
