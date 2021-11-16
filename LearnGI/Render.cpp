#include"Render.h"
#include <GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<iostream>
#include<string>

#include<CppUtil/OpenGL/FBO.h>
#include<CppUtil/OpenGL/Shader.h>

GLuint createColorTexture(int width, int height, bool hdr) {
    GLuint colorTexture;
    glGenTextures(1, &colorTexture);

    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, hdr ? GL_RGB16F : GL_RGB, width, height, 0,
        GL_RGB, hdr ? GL_FLOAT : GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return colorTexture;
}

GLuint createFramebuffer(const FramebufferCreateInfo& info) {
    GLuint framebuffer;

    // Create new framebuffer object.
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Bind color attachement.
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        info.colorTexture, 0);

    if (info.createDepthBuffer) {
        // Single renderbuffer object for both depth and stencil.
        GLuint rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, info.width,
            info.height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER, rbo);
    }

    // Check the completeness of the framebuffer.
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Blackhole-ERROR: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return framebuffer;
}

// screen
GLuint createQuadVAO() {
    std::vector<glm::vec3> vertices;

    vertices.push_back(glm::vec3(-1, -1, 0));
    vertices.push_back(glm::vec3(-1, 1, 0));
    vertices.push_back(glm::vec3(1, 1, 0));

    vertices.push_back(glm::vec3(1, 1, 0));
    vertices.push_back(glm::vec3(1, -1, 0));
    vertices.push_back(glm::vec3(-1, -1, 0));

    // Create VBO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
        &vertices[0], GL_STATIC_DRAW);

    // 1st attribute buffer: positions
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0,        // attribute
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void*)0 // array buffer offset
    );

    glBindVertexArray(0);

    return vao;
}

// use texture
static bool bindToTextureUnit(GLuint program, const std::string& name,
    GLenum textureType, GLuint texture,
    int textureUnitIndex) {
    GLint loc = glGetUniformLocation(program, name.c_str());
    if (loc != -1) {
        glUniform1i(loc, textureUnitIndex);

        // Set up the texture units.
        glActiveTexture(GL_TEXTURE0 + textureUnitIndex);
        glBindTexture(textureType, texture);
        return true;
    }
    else {
        std::cout << "WARNING: uniform " << name << " is not found in shader"
            << std::endl;
        return false;
    }
}

// begin to render
void renderToTexture(const RenderToTextureInfo& rtti) {
    // Lazy creation of a framebuffer as the render target and attach the texture
    // as the color attachment.
    static std::map<GLuint, GLuint> textureFramebufferMap;
    GLuint targetFramebuffer;
    if (!textureFramebufferMap.count(rtti.targetTexture)) {
        FramebufferCreateInfo createInfo;
        createInfo.colorTexture = rtti.targetTexture;
        targetFramebuffer = createFramebuffer(createInfo);
        textureFramebufferMap[rtti.targetTexture] = targetFramebuffer;
    }
    else {
        targetFramebuffer = textureFramebufferMap[rtti.targetTexture];
    }

    // Lazy-load the shader program.
    static std::map<std::string, GLuint> shaderProgramMap;
    GLuint program;
    if (!shaderProgramMap.count(rtti.fragShader)) {
        CppUtil::OpenGL::Shader shader(rtti.vertexShader, rtti.fragShader);
        program = shader.GetID();
        shaderProgramMap[rtti.fragShader] = program;
    }
    else {
        program = shaderProgramMap[rtti.fragShader];
    }

    // Rendering a quad.
    {
        glBindFramebuffer(GL_FRAMEBUFFER, targetFramebuffer);

        glViewport(0, 0, rtti.width, rtti.height);

        glDisable(GL_DEPTH_TEST);

        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);

        // Set up the uniforms.
        {
            glUniform2f(glGetUniformLocation(program, "resolution"),
                (float)rtti.width, (float)rtti.height);

            glUniform1f(glGetUniformLocation(program, "time"), (float)glfwGetTime());

            // Update float uniforms
            for (auto const& iter : rtti.floatUniforms) {
                GLint loc = glGetUniformLocation(program, iter.first.c_str());
                if (loc != -1) {
                    glUniform1f(loc, iter.second);
                }
                else {
                    std::cout << "WARNING: uniform " << iter.first << " is not found"
                        << std::endl;
                }
            }

            // Update texture uniforms
            int textureUnit = 0;
            for (auto const& iter : rtti.textureUniforms) {
                bindToTextureUnit(program, iter.first, GL_TEXTURE_2D, iter.second, textureUnit++);
            }
            for (auto const& iter : rtti.cubemapUniforms) {
                bindToTextureUnit(program, iter.first, GL_TEXTURE_CUBE_MAP, iter.second,
                    textureUnit++);
            }
        }

        /*GLuint screen = createQuadVAO();

        glBindVertexArray(screen);*/
        glDrawArrays(GL_TRIANGLES, 0, 6);
        /*glBindVertexArray(0);

        glDeleteVertexArrays(1, &screen);
        glDeleteBuffers(1, &screen);*/

        glUseProgram(0);
    }
}
