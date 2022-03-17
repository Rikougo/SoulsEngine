//
// Created by Sam on 3/11/2022.
//

#include "Layers/ECSLayer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <stb_image.h>

// TODO Move those functions in better place
unsigned int TextureFromFile(const char *path, const std::string &directory) {
    stbi_set_flip_vertically_on_load(true);

    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

struct InputTextCallback_UserData
{
    std::string*            Str;
    ImGuiInputTextCallback  ChainCallback;
    void*                   ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback)
    {
        // Forward to user callback, if any
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}
// TODO

namespace Elys {
    void ECSLayer::OnAttach() {
        {
            float M_PI = 3.14;
            float M_PI_2 = M_PI / 2;
            uint8_t slice = 40, stack = 40;
            std::vector<Vertex> vertices(slice * stack);

            for (unsigned int stackIt = 0; stackIt < stack; ++stackIt) {
                float u = (float)(stackIt) / (float)(stack - 1);
                float theta = u * 2 * M_PI;
                for (unsigned int sliceIt = 0; sliceIt < slice; ++sliceIt) {
                    unsigned int vertexIndex = stackIt + sliceIt * stack;
                    float v = (float)(sliceIt) / (float)(slice - 1);
                    float phi = -M_PI_2 + v * M_PI;
                    vec3 xyz = {cos(theta) * cos(phi), sin(theta) * cos(phi), sin(phi)};
                    vertices[vertexIndex] = {xyz, glm::normalize(xyz), {u, v}};
                }
            }

            for (unsigned int stackIt = 0; stackIt < stack - 1; ++stackIt) {
                for (unsigned int sliceIt = 0; sliceIt < slice - 1; ++sliceIt) {
                    unsigned int vertexuv = stackIt + sliceIt * stack;
                    unsigned int vertexUv = stackIt + 1 + sliceIt * stack;
                    unsigned int vertexuV = stackIt + (sliceIt + 1) * stack;
                    unsigned int vertexUV = stackIt + 1 + (sliceIt + 1) * stack;
                    mTriangles.insert(mTriangles.end(), {vertexuv, vertexUv, vertexUV});
                    mTriangles.insert(mTriangles.end(), {vertexuv, vertexUV, vertexuV});
                }
            }

            glGenVertexArrays(1, &mVAO);
            glGenBuffers(1, &mVBO);
            glGenBuffers(1, &mEBO);

            glBindVertexArray(mVAO);
            glBindBuffer(GL_ARRAY_BUFFER, mVBO);

            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mTriangles.size() * sizeof(uint32_t), mTriangles.data(),
                         GL_STATIC_DRAW);

            // vertex positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
            // vertex normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void *)offsetof(Vertex, normal));
            // vertex texture coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void *)offsetof(Vertex, texCoord));

            glBindVertexArray(0);
        }

        mShader = new Shader("./shaders/model_vertex.glsl", "./shaders/model_fragment.glsl");

        Texture sunTexture = Texture {
            .id = TextureFromFile("sun.jpg", "./assets/textures"),
            .path = "./assets/textures/sun.jpg"
        };

        Texture earthTexture = Texture {
            .id = TextureFromFile("earth.jpg", "./assets/textures"),
            .path = "./assets/textures/earth.jpg"
        };

        Texture moonTexture = Texture {
            .id = TextureFromFile("moon.jpg", "./assets/textures"),
            .path = "./assets/textures/moon.jpg"
        };

        Texture starTexture = Texture {
            .id = TextureFromFile("stars_milky_way.jpg", "./assets/textures"),
            .path = "./assets/textures/stars_milky_way.jpg"
        };

        mSun = mCurrentScene.CreateEntity("Sun");
        ELYS_CORE_INFO("Created sun.");
        mEarth = mSun.CreateChild("Earth");
        ELYS_CORE_INFO("Created earth.");
        mMoon = mEarth.CreateChild("Moon");
        ELYS_CORE_INFO("Created moon.");
        auto mGlobal = mCurrentScene.CreateEntity("Global");
        ELYS_CORE_INFO("Created global.");

        mSun.AddComponent(sunTexture);
        mSun.AddComponent(Material{.ambient={1.0f, 1.0f, 1.0f}});
        mEarth.AddComponent(earthTexture);
        mMoon.AddComponent(moonTexture);
        mGlobal.AddComponent(starTexture);
        mGlobal.AddComponent(Material{.ambient={1.0f, 1.0f, 1.0f}});
        ELYS_CORE_INFO("Added textures.");
    }
    void ECSLayer::OnDetach() {
        glDeleteBuffers(1, &mVBO);
        glDeleteBuffers(1, &mEBO);
    }

    void ECSLayer::OnUpdate(float deltaTime) {
        Render();
        PhysicUpdate(deltaTime);
    }

    void ECSLayer::Render() {
        glClearColor(0.05, 0.05, 0.05, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode( GL_FRONT_AND_BACK, mWireframeMode ? GL_LINE : GL_FILL );

        mShader->Use();
        mCamera.Apply((*mShader));

        mShader->SetVec3("lightPosition", mSun.GetComponent<Transform>().position);

        for(auto &entity : mCurrentScene) {
            DrawEntity(entity);
        }
    }

    void ECSLayer::PhysicUpdate(float deltaTime) {
        auto &earthTransform = mEarth.GetComponent<Transform>().rotation.y += 10 * deltaTime;
    }

    void ECSLayer::OnImGuiRender() {
        static int corner = 1;
        ImGuiIO& io = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration;
        if (corner != -1)
        {
            const float PAD = 0.0f;
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImVec2 window_pos, window_pos_pivot;
            window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
            window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
            window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
            window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            window_flags |= ImGuiWindowFlags_NoMove;
        }
        if(ImGui::Begin("Entities", nullptr, window_flags)) {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Entity"))
                {
                    if (ImGui::MenuItem("New")) mCurrentScene.CreateEntity();
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            // left
            {
                if(ImGui::BeginChild("selector", ImVec2(150, 0), true)) {
                    for(auto const &entity : mCurrentScene) {
                        ImGuiDrawEntityTreeSelector(entity);
                    }
                    ImGui::EndChild();
                }
            }
            ImGui::SameLine();

            // Right
            if (mSelected.IsValid()) {
                ImGui::BeginGroup();
                if (ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()))) // Leave room for 1 line below us
                {
                    auto &name = mSelected.GetComponent<Tag>().name;

                    InputText("##NAME#Selected", &name, 0, nullptr, nullptr);
                    ImGui::Separator();

                    ImGui::Text("Position : "); ImGui::SameLine();
                    ImGui::DragFloat3("##POSITION#Selected", &mSelected.GetComponent<Transform>().position[0], 0.1f, -10.f, 10.f, "%0.2f");
                    ImGui::Text("Rotation : "); ImGui::SameLine();
                    ImGui::DragFloat3("##ROTATION#Selected", &mSelected.GetComponent<Transform>().rotation[0], 1.0f, 0.0f, 360.0f, "%0.2f");
                    ImGui::Text("Echelle : "); ImGui::SameLine();
                    ImGui::DragFloat3("##SCALE#Selected", &mSelected.GetComponent<Transform>().scale[0], 0.1f, 0.1f, 10.f, "%0.2f");

                    ImGui::EndChild();
                }
                ImGui::EndGroup();
            }

            ImGui::End();
        }
    }

    void ECSLayer::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(ECSLayer::OnMouseButtonPressed));
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(ECSLayer::OnKeyPressed));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(ECSLayer::OnWindowResize));
    }

    bool ECSLayer::OnKeyPressed(KeyPressedEvent &event) {
        if (event.GetKeyCode() == GLFW_KEY_Z) {
            mWireframeMode = !mWireframeMode;
        }

        return false;
    }
    bool ECSLayer::OnMouseButtonPressed(MouseButtonPressedEvent &event) { return false; }

    bool ECSLayer::OnWindowResize(WindowResizeEvent &event) {
        glViewport(0, 0, event.GetWidth(), event.GetHeight());
        mCamera.mFOV = (float) event.GetWidth() / (float) event.GetWidth();

        return false;
    }

    void ECSLayer::CreateScene() {
        mCurrentScene = Scene();
    }
    void ECSLayer::LoadScene(const std::filesystem::path &path) {}
    void ECSLayer::SaveScene(const std::filesystem::path &path) {}

    void ECSLayer::ImGuiDrawEntityTreeSelector(Entity const &entity) {
        auto const &tag = entity.GetComponent<Tag>();
        auto &position = entity.GetComponent<Transform>().position;

        std::stringstream treeID, label;
        treeID << "##" << tag.name.c_str() << "#" << entity.ID();
        label << tag.name.c_str() << "#" << entity.ID();

        if (ImGui::Selectable(label.str().c_str(), mSelected == entity, 0, ImVec2(75, 0)))
            mSelected = entity;
        ImGui::SameLine();
        if (ImGui::TreeNode(treeID.str().c_str())) {

            for(auto const &child : entity.Children()) {
                ImGuiDrawEntityTreeSelector(child);
            }
            ImGui::TreePop();
        }
    }

    void ECSLayer::DrawEntity(Entity const &entity, glm::mat4 parent) {
        auto transform = entity.GetComponent<Transform>();

        auto model = glm::mat4{1.0f};

        model = glm::translate(model, transform.position);

        for(uint8_t i = 0; i < 3; i++) {
            vec3 axis{0.0f};
            axis[i] = 1.0f;
            model = glm::rotate(model, glm::radians(transform.rotation[i]), axis);
        }

        model = parent * glm::scale(model, transform.scale);


        for (auto const &child : entity.Children())
            DrawEntity(child, model);

        bool hasTexture = entity.HasComponent<Texture>();
        mShader->SetBool("hasTexture", hasTexture);
        if (hasTexture) {
            auto const &texture = entity.GetComponent<Texture>();
            glActiveTexture(GL_TEXTURE0);
            mShader->SetInt("texture", 0);
            glBindTexture(GL_TEXTURE_2D, texture.id);
        } else {
            mShader->SetVec3("color", {0.7, 0.5, 1.0});
        }

        mShader->SetVec3("uAmbient", entity.HasComponent<Material>() ? entity.GetComponent<Material>().ambient : vec3{0.1f, 0.1f, 0.1f});

        mShader->SetMat4("model", model);

        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, mTriangles.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}