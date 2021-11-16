#pragma once
/**
 * @file render.h
 * @author LiuDong (l1206776367@gmail.com)
 * @brief Utility functions for GL rendering including framebuffer creation,
 * render to texture, etc.
 * @version 0.1
 * @date 2021-09-29
 *
 * 此文件为渲染黑洞的专用简易渲染器，定制FBO与colorTexture缓冲
 * 未来时段将会被重新封装进此项目，届时此文件则可以被替代
 * 
 * @copyright Copyright (c) 2021
 *
 */

#ifndef RENDER_H
#define RENDER_H

#include <map>
#include <string>
#include <vector>

#include<GL/glew.h>

GLuint createColorTexture(int width, int height, bool hdr = true);

struct FramebufferCreateInfo {
	GLuint colorTexture = 0;
	int width = 256;
	int height = 256;
	bool createDepthBuffer = false;
};

GLuint createFramebuffer(const FramebufferCreateInfo& info);

GLuint createQuadVAO();

struct RenderToTextureInfo {
	std::string vertexShader = "shader/simple.vert";
	std::string fragShader;
	std::map<std::string, float> floatUniforms;
	std::map<std::string, GLuint> textureUniforms;
	std::map<std::string, GLuint> cubemapUniforms;
	GLuint targetTexture;
	int width;
	int height;
};

void renderToTexture(const RenderToTextureInfo& rtti);

#endif // ! RENDER_H