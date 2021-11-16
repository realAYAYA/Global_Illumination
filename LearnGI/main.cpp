#include<iostream>

#include<CppUtil/Basic/Math.h>
#include<CppUtil/OpenGL/Window.h>
#include<CppUtil/OpenGL/Shader.h>
#include<CppUtil/OpenGL/Model.h>
#include<CppUtil/OpenGL/TfmObject.h>
#include<CppUtil/OpenGL/Scene.h>
#include<CppUtil/OpenGL/VAO.h>
#include<CppUtil/OpenGL/FBO.h>
#include<CppUtil/OpenGL/Texture.h>
#include<CppUtil/OpenGL/Skybox.h>

#include<CppUtil/RTX/Model.h>

#include"GlobalDefines.h"
#include"CommonDefine.h"

using std::cout;
using std::endl;
//using namespace CppUtil::Basic::Math;
using namespace CppUtil::OpenGL;
int Nanosuit(int argc, char* argv[]);
int CornellBox(int argc, char* argv[]);
int Raymarching(int argc, char* argv[]);
int Blackhole(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    // 选择一个课题运行
    //Blackhole(argc, argv);
    Raymarching(argc, argv);
    //Nanosuit(argc, argv);
    //CornellBox(argc, argv);

    return 1;
}



int Nanosuit(int argc, char* argv[]) {
    Window myWindow(SCR_WIDTH, SCR_HEIGHT, "My RTX Game", &mainCamera);

    myWindow.MakeContextCurrent();
    cout << "Hello OpenGL!\n";

    // configure global opengl state
    // -----------------------------
    //glEnable(GL_CULL_FACE);// Face culling
    //glCullFace(GL_BACK);// GL_FRONT  
    //glEnable(GL_STENCIL_TEST);
    //glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_FRAMEBUFFER_SRGB);// OpenGL auto Gamma Correction
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    myWindow.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);    
    myWindow.SetCursorPosCallback(mouse_callback);
    myWindow.SetScrollCallback(scroll_callback);
    //myWindow.SetFramebufferSizeCallback(framebuffer_size_callback);
    //myWindow.SetKeyCallback(key_callback);

    // Init workspace path
    // -------------------------
    std::string exePath = argv[0];
    shaderPath = exePath.substr(0, exePath.find_last_of('\\')) + "\\..\\LearnGI\\assets\\shaders\\";
    texturePath = exePath.substr(0, exePath.find_last_of('\\')) + "\\..\\LearnGI\\assets\\textures\\";
    modelPath = exePath.substr(0, exePath.find_last_of('\\')) + "\\..\\LearnGI\\assets\\model\\";

    VAO VAO_Screen(&(data_ScreenVertices[0]), sizeof(data_ScreenVertices), { 2,2 });// Screen
    VAO cube(&(cubeData_PNT[0]), sizeof(cubeData_PNT), { 3,3,2 });
    FBO gBuffer(SCR_WIDTH, SCR_HEIGHT, FBO::ENUM_TYPE_GBUFFER);

    Shader shaderGeometryPass(shaderPath + "advenced\\gBuffer.vs", shaderPath + "advenced\\gBuffer.fs");
    Shader shaderRTX(shaderPath + "advenced\\deferredShading.vs", shaderPath + "advenced\\deferredShading.fs");
    //Shader shaderRTX(shaderPath + "advenced\\deferredShading.vs", shaderPath + "advenced\\tempFile.frag");// temp test
    Shader shaderLightBox(shaderPath + "advenced\\deferredLightBox.vs", shaderPath + "advenced\\deferredLightBox.fs");
    if (!shaderGeometryPass.IsValid() && !shaderRTX.IsValid() && !shaderLightBox.IsValid()) {
        printf("ERROR: Shader load fail.\n");
        return 1;
    }
    // shader configuration
    // --------------------
    shaderRTX.Use();
    shaderRTX.SetInt("gPosition", 0);
    shaderRTX.SetInt("gNormal", 1);
    shaderRTX.SetInt("gAlbedoSpec", 2);

    // CreateScene
    // -----------
    Scene scene;   
    Model nanosuitModel(modelPath + "nanosuit\\nanosuit.obj");
    glm::mat4 tfmModel(1.0f);
    tfmModel= glm::translate(tfmModel, glm::vec3(-3.0, -0.5, -3.0));
    tfmModel= glm::scale(tfmModel, glm::vec3(0.25f));
    TfmObject nanosuit(&nanosuitModel, tfmModel);
    scene.Push(&nanosuit);

    // lighting info
    // -------------
    const unsigned int NR_LIGHTS = 32;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    srand(13);
    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        // calculate slightly random offsets
        float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
        float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
        // also calculate random color
        float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        lightColors.push_back(glm::vec3(rColor, gColor, bColor));
    }
    
    while (!glfwWindowShouldClose(myWindow.GetGLFWwindow())) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        myWindow.ProcessKeyboard(deltaTime);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);// Clear Screen0.1f, 0.1f, 0.1f, 1.0f
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.GetID());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        shaderGeometryPass.Use();
        shaderGeometryPass.SetMat4f("view", mainCamera.GetViewMatrix());
        shaderGeometryPass.SetMat4f("projection", mainCamera.GetProjectionMatrix());

        /* Draw scene*/
        scene.Draw(shaderGeometryPass);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2.lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
        // ---------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderRTX.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorTexture(0).GetID());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorTexture(1).GetID());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorTexture(2).GetID());

        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            string tempVec3 = "lights[" + std::to_string(i) + "].Position";
            shaderRTX.SetVec3f(tempVec3.c_str(), lightPositions[i]);
            tempVec3 = "lights[" + std::to_string(i) + "].Color";
            shaderRTX.SetVec3f(tempVec3.c_str(), lightColors[i]);
            // update attenuation parameters and calculate radius
            const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
            const float linear = 0.7;
            const float quadratic = 1.8;
            tempVec3 = "lights[" + std::to_string(i) + "].Linear";
            shaderRTX.SetFloat(tempVec3.c_str(), linear);
            tempVec3 = "lights[" + std::to_string(i) + "].Quadratic";
            shaderRTX.SetFloat(tempVec3.c_str(), quadratic);
            // then calculate radius of light volume/sphere
            const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
            float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
            tempVec3 = "lights[" + std::to_string(i) + "].Radius";
            shaderRTX.SetFloat(tempVec3.c_str(), radius);
        }
        shaderRTX.SetVec3f("viewPos", mainCamera.GetPos());

        VAO_Screen.Draw(shaderRTX);// Draw Screen-gBuffer

        // Copy content of geometry's depth buffer to default framebuffer's depth buffer
            // ----------------------------------------------------------------------------------
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.GetID());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);// write to default framebuffer
        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. render lights on top of scene
        // --------------------------------
        shaderLightBox.SetMat4f("projection", mainCamera.GetProjectionMatrix());
        shaderLightBox.SetMat4f("view", mainCamera.GetViewMatrix());
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(0.125f));
            shaderLightBox.SetMat4f("model", model);
            shaderLightBox.SetVec3f("lightColor", lightColors[i]);
            cube.Draw(shaderLightBox);
        }

        /* Clean up,prepare for next render loop*/
        glfwPollEvents();
        glfwSwapBuffers(myWindow.GetGLFWwindow());// Update window
    }

    // Exit Program
    glfwTerminate();// End
    return 0;
}


int CornellBox(int argc, char* argv[]) {
    Window myWindow(SCR_WIDTH, SCR_HEIGHT, "My RTX Game", &mainCamera);

    myWindow.MakeContextCurrent();
    cout << "Hello OpenGL!\n";

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    myWindow.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    myWindow.SetCursorPosCallback(mouse_callback);
    myWindow.SetScrollCallback(scroll_callback);
    //myWindow.SetFramebufferSizeCallback(framebuffer_size_callback);
    //myWindow.SetKeyCallback(key_callback);

    // Init workspace path
    // -------------------------
    std::string exePath = argv[0];
    shaderPath = exePath.substr(0, exePath.find_last_of('\\')) + "\\..\\LearnGI\\assets\\shaders\\";
    texturePath = exePath.substr(0, exePath.find_last_of('\\')) + "\\..\\LearnGI\\assets\\textures\\";
    modelPath = exePath.substr(0, exePath.find_last_of('\\')) + "\\..\\LearnGI\\assets\\model\\";

    VAO VAO_Screen(&(data_ScreenVertices[0]), sizeof(data_ScreenVertices), { 2,2 });// Screen
    FBO gBuffer(SCR_WIDTH, SCR_HEIGHT, FBO::ENUM_TYPE_GBUFFER);

    Shader shaderGeometryPass(shaderPath + "advenced\\gBuffer.vs", shaderPath + "advenced\\gBuffer.fs");
    //Shader shaderRTX(shaderPath + "deferredShading.vs", shaderPath + "deferredShading.fs");
    Shader shaderRTX(shaderPath + "advenced\\deferredShading.vs", shaderPath + "advenced\\tempFile.frag");// temp test
    Shader shaderLightBox(shaderPath + "advenced\\deferredLightBox.vs", shaderPath + "advenced\\deferredLightBox.fs");
    if (!shaderGeometryPass.IsValid() && !shaderRTX.IsValid() && !shaderLightBox.IsValid()) {
        printf("ERROR: Shader load fail.\n");
        return 1;
    }
    // shader configuration
    // --------------------
    shaderRTX.Use();
    shaderRTX.SetInt("gPosition", 0);
    shaderRTX.SetInt("gNormal", 1);
    shaderRTX.SetInt("gAlbedoSpec", 2);

    // CreateScene
    // -----------
    Scene* scene = CreateScene_CornellBox();


    // Test texture
    // ------------
    float _data[16] = { 0.0f,1.0f,2.0f,3.0f,4.0f,5.0f,6.0f,7.0f,8.0f,9.0f,10.0f,11.0f,12.0f,13.0f,14.0f,15.0f };
    vector<float> data; for (int i = 0; i < 16; i++) { data.push_back(_data[i]); }
    int sceneDataSize = DataToMap(data);
    CppUtil::OpenGL::Texture sceneDataTex((sceneDataSize + 1) / 2, (sceneDataSize + 1) / 2, data.data(), GL_FLOAT, GL_RGBA, GL_RGBA32F);// So important

    shaderRTX.SetInt("PackData", 3);// Test

    while (!glfwWindowShouldClose(myWindow.GetGLFWwindow())) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        myWindow.ProcessKeyboard(deltaTime);
        printf("\rfps: %.2f     ", 1 / deltaTime);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);// Clear Screen0.1f, 0.1f, 0.1f, 1.0f
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.GetID());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        shaderGeometryPass.Use();
        shaderGeometryPass.SetMat4f("view", mainCamera.GetViewMatrix());
        shaderGeometryPass.SetMat4f("projection", mainCamera.GetProjectionMatrix());

        /* Draw scene*/
        scene->Draw(shaderGeometryPass);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2.lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
        // ---------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderRTX.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorTexture(0).GetID());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorTexture(1).GetID());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gBuffer.GetColorTexture(2).GetID());
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, sceneDataTex.GetID());// Test

        VAO_Screen.Draw(shaderRTX);// Draw Screen-gBuffer

        /* Clean up,prepare for next render loop*/
        glfwPollEvents();
        glfwSwapBuffers(myWindow.GetGLFWwindow());// Update window
    }

    // Exit Program
    glfwTerminate();// End
    return 0;
}

int Raymarching(int argc, char* argv[]) {
    Window myWindow(SCR_WIDTH, SCR_HEIGHT, "My RTX Game", &mainCamera);

    myWindow.MakeContextCurrent();
    cout << "Hello OpenGL!\n";

    // configure global opengl state
    // -----------------------------
    glDisable(GL_DEPTH_TEST);


    myWindow.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    myWindow.SetCursorPosCallback(mouse_callback);
    myWindow.SetScrollCallback(scroll_callback);
    //myWindow.SetFramebufferSizeCallback(framebuffer_size_callback);
    //myWindow.SetKeyCallback(key_callback);

    // Init workspace path
    // -------------------------
    std::string exePath = argv[0];
    shaderPath = exePath.substr(0, exePath.find_last_of('\\')) + "\\..\\LearnGI\\assets\\shaders\\";
    texturePath = exePath.substr(0, exePath.find_last_of('\\')) + "\\..\\LearnGI\\assets\\textures\\";
    modelPath = exePath.substr(0, exePath.find_last_of('\\')) + "\\..\\LearnGI\\assets\\model\\";

    //Shader shaderGeometryPass(shaderPath + "base\\model.vs", shaderPath + "base\\model.fs");

    Shader raymarchingShader(shaderPath + "RayMarching\\raymarch.vs", shaderPath + "RayMarching\\raymarch.fs");

    glm::mat4 model(1.0);
    model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
    model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));

    //Model bunny(modelPath + "bunny.obj");
    //TfmObject TfmBunny(&bunny, model);
    VAO VAO_Screen(&(data_ScreenVertices[0]), sizeof(data_ScreenVertices), { 2,2 });// Screen

    //auto bunny2 = CppUtil::Basic::ToCPtr(new RTX::Model(modelPath + "dragon.obj"));
    //RTX::Model* B = new RTX::Model(modelPath + "dragon.obj");
    //RTX::Model C(modelPath + "bunny.obj");

    while (!glfwWindowShouldClose(myWindow.GetGLFWwindow())) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        iTime += currentFrame;

        myWindow.ProcessKeyboard(deltaTime);
        printf("\rfps: %.2f     ", 1 / deltaTime);
       
        // render
        // ------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);// Clear Screen0.1f, 0.1f, 0.1f, 1.0f
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        //shaderGeometryPass.Use();
        //shaderGeometryPass.SetMat4f("view", mainCamera.GetViewMatrix());
        //shaderGeometryPass.SetMat4f("projection", mainCamera.GetProjectionMatrix());

        // ide
        //TfmBunny.Draw(shaderGeometryPass);
        VAO_Screen.Draw(raymarchingShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        /* Clean up,prepare for next render loop*/
        glfwPollEvents();
        glfwSwapBuffers(myWindow.GetGLFWwindow());// Update window
    }

    // Exit Program
    glfwTerminate();// End
    return 0;
}

#include"Render.h"
int Blackhole(int argc, char* argv[]) {
    Window myWindow(SCR_WIDTH, SCR_HEIGHT, "Blackhole", &mainCamera);

    myWindow.MakeContextCurrent();
    cout << "Hello OpenGL!\n";

    myWindow.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    myWindow.SetCursorPosCallback(mouse_callback);
    myWindow.SetScrollCallback(scroll_callback);
    //myWindow.SetFramebufferSizeCallback(framebuffer_size_callback);
    //myWindow.SetKeyCallback(key_callback);

    // configure global opengl state
    // -----------------------------

    // Init workspace path
    // -------------------------
    std::string exePath = argv[0];
    shaderPath = exePath.substr(0, exePath.find_last_of('\\')) + "\\..\\LearnGI\\assets\\shaders\\";
    texturePath = exePath.substr(0, exePath.find_last_of('\\')) + "\\..\\LearnGI\\assets\\textures\\";
    modelPath = exePath.substr(0, exePath.find_last_of('\\')) + "\\..\\LearnGI\\assets\\model\\";

    // create shader program
    Shader passthrough(shaderPath + "Blackhole\\blackhole\\simple.vert", shaderPath + "Blackhole\\blackhole\\passthrough.frag");
    passthrough.SetInt("texture0", 0);
    glUseProgram(0);
       
    // texture using by blackhole
    std::vector<string> faces = {
        texturePath + "\\skybox\\nebula_dark\\right.png",
        texturePath + "\\skybox\\nebula_dark\\left.png",
        texturePath + "\\skybox\\nebula_dark\\top.png",
        texturePath + "\\skybox\\nebula_dark\\bottom.png",
        texturePath + "\\skybox\\nebula_dark\\front.png",
        texturePath + "\\skybox\\nebula_dark\\back.png"
    };
    Skybox galaxy(faces);
    Texture colorMap(texturePath+"color_map.png");
    Texture uvChecker(texturePath + "uv_checker.png");

    // final results on screen space
    VAO VAO_Screen(&(data_ScreenVertices[0]), sizeof(data_ScreenVertices), { 2,2 });// Screen

    while (!glfwWindowShouldClose(myWindow.GetGLFWwindow())) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        myWindow.ProcessKeyboard(deltaTime);
        printf("\rfps: %.2f     ", 1 / deltaTime);

        // render
        // ------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);// Clear Screen0.1f, 0.1f, 0.1f, 1.0f
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static GLuint texBlackhole = createColorTexture(SCR_WIDTH, SCR_HEIGHT);
        {
            RenderToTextureInfo rtti;
            rtti.vertexShader = shaderPath + "Blackhole\\blackhole\\simple.vert";
            rtti.fragShader = shaderPath + "Blackhole\\blackhole\\blackhole_main.frag";
            rtti.cubemapUniforms["galaxy"] = galaxy.GetID();
            rtti.textureUniforms["colorMap"] = colorMap.GetID();
            //rtti.textureUniforms["colorMap"] = 0;// 取消吸积盘材质效果
            rtti.floatUniforms["mouseX"] = lastX;
            rtti.floatUniforms["mouseY"] = lastY;
            rtti.targetTexture = texBlackhole;
            rtti.width = SCR_WIDTH;
            rtti.height = SCR_HEIGHT;
            renderToTexture(rtti);
        }

        static GLuint texBrightness = createColorTexture(SCR_WIDTH, SCR_HEIGHT);
        {
            RenderToTextureInfo rtti;
            rtti.vertexShader = shaderPath + "Blackhole\\blackhole\\simple.vert";
            rtti.fragShader = shaderPath + "Blackhole\\blackhole\\bloom_brightness_pass.frag";
            rtti.textureUniforms["texture0"] = texBlackhole;
            rtti.targetTexture = texBrightness;
            rtti.width = SCR_WIDTH;
            rtti.height = SCR_HEIGHT;
            renderToTexture(rtti);
        }

        const int MAX_BLOOM_ITER = 8;
        static GLuint texDownsampled[MAX_BLOOM_ITER];
        static GLuint texUpsampled[MAX_BLOOM_ITER];
        if (texDownsampled[0] == 0) {
            for (int i = 0; i < MAX_BLOOM_ITER; i++) {
                texDownsampled[i] =
                    createColorTexture(SCR_WIDTH >> (i + 1), SCR_HEIGHT >> (i + 1));
                texUpsampled[i] = createColorTexture(SCR_WIDTH >> i, SCR_HEIGHT >> i);
            }
        }

        static int bloomIterations = MAX_BLOOM_ITER;
        for (int level = 0; level < bloomIterations; level++) {
            RenderToTextureInfo rtti;
            rtti.vertexShader = shaderPath + "Blackhole\\blackhole\\simple.vert";
            rtti.fragShader = shaderPath + "Blackhole\\blackhole\\bloom_downsample.frag";
            rtti.textureUniforms["texture0"] =
                level == 0 ? texBrightness : texDownsampled[level - 1];
            rtti.targetTexture = texDownsampled[level];
            rtti.width = SCR_WIDTH >> (level + 1);
            rtti.height = SCR_HEIGHT >> (level + 1);
            renderToTexture(rtti);
        }

        for (int level = bloomIterations - 1; level >= 0; level--) {
            RenderToTextureInfo rtti;
            rtti.vertexShader = shaderPath + "Blackhole\\blackhole\\simple.vert";
            rtti.fragShader = shaderPath + "Blackhole\\blackhole\\bloom_upsample.frag";
            rtti.textureUniforms["texture0"] = level == bloomIterations - 1
                ? texDownsampled[level]
                : texUpsampled[level + 1];
            rtti.textureUniforms["texture1"] =
                level == 0 ? texBrightness : texDownsampled[level - 1];
            rtti.targetTexture = texUpsampled[level];
            rtti.width = SCR_WIDTH >> level;
            rtti.height = SCR_HEIGHT >> level;
            renderToTexture(rtti);
        }

        static GLuint texBloomFinal = createColorTexture(SCR_WIDTH, SCR_HEIGHT);
        {
            RenderToTextureInfo rtti;
            rtti.vertexShader = shaderPath + "Blackhole\\blackhole\\simple.vert";
            rtti.fragShader = shaderPath + "Blackhole\\blackhole\\bloom_composite.frag";
            rtti.textureUniforms["texture0"] = texBlackhole;
            rtti.textureUniforms["texture1"] = texUpsampled[0];
            rtti.targetTexture = texBloomFinal;
            rtti.width = SCR_WIDTH;
            rtti.height = SCR_HEIGHT;

            renderToTexture(rtti);
        }

        static GLuint texTonemapped = createColorTexture(SCR_WIDTH, SCR_HEIGHT);
        {
            RenderToTextureInfo rtti;
            rtti.vertexShader = shaderPath + "Blackhole\\blackhole\\simple.vert";
            rtti.fragShader = shaderPath + "Blackhole\\blackhole\\tonemapping.frag";
            rtti.textureUniforms["texture0"] = texBloomFinal;
            rtti.targetTexture = texTonemapped;
            rtti.width = SCR_WIDTH;
            rtti.height = SCR_HEIGHT;

            renderToTexture(rtti);
        }
        
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glDisable(GL_DEPTH_TEST);

            glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(passthrough.GetID());

            glUniform2f(glGetUniformLocation(passthrough.GetID(), "resolution"),
                (float)SCR_WIDTH, (float)SCR_HEIGHT);

            glUniform1f(glGetUniformLocation(passthrough.GetID(), "time"),
                (float)glfwGetTime());

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texTonemapped);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glUseProgram(0);
        }

        //VAO_Screen.Draw(texShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        /* Clean up,prepare for next render loop*/
        glfwPollEvents();
        glfwSwapBuffers(myWindow.GetGLFWwindow());// Update window
    }

    // Exit Program
    glfwTerminate();// End
    return 0;
}