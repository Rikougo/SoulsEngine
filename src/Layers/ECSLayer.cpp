//
// Created by Sam on 3/11/2022.
//

#include "Layers/ECSLayer.hpp"

#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <stb_image.h>

#include <Application.hpp>

struct InputTextCallback_UserData {
    std::string*            Str;
    ImGuiInputTextCallback  ChainCallback;
    void*                   ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
    auto user_data = (InputTextCallback_UserData *)data->UserData;
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

bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr) {
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data{};
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return ImGui::InputText(label, (char *)str->c_str(), str->capacity() + 1, flags,
                            InputTextCallback, &cb_user_data);
}

float sign(vec2 first, vec2 sec, vec2 third) {
    return (first.x - third.x) * (sec.y - third.y) - (sec.x - third.x) * (first.y - third.y);
}

bool InTriangle(vec3 point, vec3 t1, vec3 t2, vec3 t3) {
    vec2 point2 = {point.x, point.z};
    float d1 = sign(point2, vec2{t1.x, t1.z}, vec2{t2.x, t2.z});
    float d2 = sign(point2, vec2{t2.x, t2.z}, vec2{t3.x, t3.z});
    float d3 = sign(point2, vec2{t3.x, t3.z}, vec2{t1.x, t1.z});

    bool neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(neg && pos);
}

namespace Elys {
    void ECSLayer::OnAttach() {
        mShader = new Shader("./shaders/model_vertex.glsl", "./shaders/model_fragment.glsl");

        Texture sunTexture = GenerateTexture("sun.jpg", "./assets/textures");

        auto playerModelPath = std::filesystem::path("./assets/model/elephant_n.off");

        mWalking = mCurrentScene.CreateEntity("Player");
        mWalking.AddComponent(Mesh::LoadOFF(playerModelPath, true));
        ELYS_CORE_INFO("Created player.");

        mTerrain = mCurrentScene.CreateEntity("Terrain");
        mTerrain.AddComponent<Texture>(GenerateTexture("grass.png", "./assets/textures"));
        mTerrain.AddComponent(Mesh::Plane(512, 512));
        mTerrain.GetComponent<Transform>().scale = {100.0f, 1.0f, 100.0f};
        ELYS_CORE_INFO("Created terrain.");

        mSelected = mWalking;
    }
    void ECSLayer::OnDetach() {}

    void ECSLayer::OnUpdate(float deltaTime) {
        Render();
        PhysicUpdate(deltaTime);
    }

    void ECSLayer::Render() {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, mWireframeMode ? GL_LINE : GL_FILL);

        mShader->Use();
        mCamera.Apply((*mShader));

        mShader->SetVec3("lightPosition", mCamera.GetPosition());

        for (auto &entity : mCurrentScene) {
            DrawEntity(entity);
        }
    }

    void ECSLayer::PhysicUpdate(float deltaTime) {
        auto &playerTransform = mWalking.GetComponent<Transform>();
        auto const &terrainTransform = mTerrain.GetComponent<Transform>();
        auto const &terrainMesh = mTerrain.GetComponent<Mesh>();
        auto terrainVertices = terrainMesh.Vertices();
        auto terrainIndices = terrainMesh.Indices();

        for(size_t i = 0; i < terrainIndices.size(); i+=3) {
            auto i1 = terrainIndices[i];
            auto i2 = terrainIndices[i+1];
            auto i3 = terrainIndices[i+2];

            auto t1 = terrainVertices[i1].position * terrainTransform.scale;
            auto t2 = terrainVertices[i2].position * terrainTransform.scale;
            auto t3 = terrainVertices[i3].position * terrainTransform.scale;

            if (InTriangle(playerTransform.position, t1, t2, t3)) {
                auto height = t1.y + t2.y + t3.y;
                playerTransform.position.y = height / 3.0f + mTerrainOffset;
                break;
            }
        }

        if (mForward || mBackward || mRight || mLeft) {
            vec2 direction{0.0f, 0.0f};

            if (mForward)  direction.y += 1.0f;
            if (mBackward) direction.y -= 1.0f;
            if (mRight)    direction.x += 1.0f;
            if (mLeft)     direction.x -= 1.0f;

            direction = glm::normalize(direction);

            playerTransform.position.x += direction.x * deltaTime;
            playerTransform.position.z += direction.y * deltaTime;
        }
    }

    void ECSLayer::OnImGuiRender() {
        static int corner = 1;
        ImGuiIO &io = ImGui::GetIO();
        ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse;
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
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * mRightPanelWidth, io.DisplaySize.y));
        if(ImGui::Begin("Entities", nullptr, window_flags)) {
            Application::Get().GetImGUILayer().Blocking(
                ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows));

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Entity")) {
                    if (ImGui::MenuItem("New"))
                        ELYS_CORE_INFO(
                            "Create entity (not implemented yet."); // mCurrentScene.CreateEntity();
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            // left
            {
                if(ImGui::BeginChild("selector", ImVec2(150, 0), true)) {
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

                    auto &transform = mSelected.GetComponent<Transform>();

                    ImGui::Text("Position : ");
                    ImGui::SameLine();
                    ImGui::DragFloat3("##POSITION#Selected", &transform.position[0], 0.1f, -10.f,
                                      10.f, "%0.2f");
                    ImGui::Text("Rotation : ");
                    ImGui::SameLine();
                    ImGui::DragFloat3("##ROTATION#Selected", &transform.rotation[0], 1.0f, 0.0f,
                                      360.0f, "%0.2f");
                    ImGui::Text("Echelle : ");
                    ImGui::SameLine();
                    ImGui::DragFloat3("##SCALE#Selected", &transform.scale[0], 0.1f, 0.1f, 10.f,
                                      "%0.2f");

                    ImGui::Text("Offset : ");
                    ImGui::SameLine();
                    ImGui::DragFloat("##OFFSET#Selected", &mTerrainOffset, 0.1f, 0.1f, 10.0f);

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
        dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(ECSLayer::OnMouseButtonReleased));
        dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(ECSLayer::OnMouseMove));
        dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(ECSLayer::OnMouseScroll));
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(ECSLayer::OnKeyPressed));
        dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(ECSLayer::OnKeyReleased));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(ECSLayer::OnWindowResize));
    }

    bool ECSLayer::OnKeyPressed(KeyPressedEvent &event) {
        if (event.GetKeyCode() == GLFW_KEY_Z) {
            mWireframeMode = !mWireframeMode;
        }

        if (event.GetKeyCode() == GLFW_KEY_W) mForward = true;
        if (event.GetKeyCode() == GLFW_KEY_S) mBackward = true;
        if (event.GetKeyCode() == GLFW_KEY_D) mRight = true;
        if (event.GetKeyCode() == GLFW_KEY_A) mLeft = true;

        return false;
    }

    bool ECSLayer::OnKeyReleased(KeyReleasedEvent &event) {
        if (event.GetKeyCode() == GLFW_KEY_W) mForward = false;
        if (event.GetKeyCode() == GLFW_KEY_S) mBackward = false;
        if (event.GetKeyCode() == GLFW_KEY_D) mRight = false;
        if (event.GetKeyCode() == GLFW_KEY_A) mLeft = false;

        return false;
    }

    bool ECSLayer::OnMouseButtonPressed(MouseButtonPressedEvent &event) {
        mCamera.StartCapture(event.GetMouseButton());
        return false;
    }

    bool ECSLayer::OnMouseButtonReleased(MouseButtonReleasedEvent &event) {
        mCamera.EndCapture();
        return false;
    }

    bool ECSLayer::OnMouseMove(MouseMovedEvent &event) {
        mCamera.MouseInput(event.GetX(), event.GetY());
        return false;
    }

    bool ECSLayer::OnMouseScroll(MouseScrolledEvent &event) {
        mCamera.Zoom(event.GetYOffset());
        return false;
    }

    bool ECSLayer::OnWindowResize(WindowResizeEvent &event) {
        glViewport(0, 0, (GLsizei)((float)event.GetWidth() * (1 - mRightPanelWidth)),
                   (GLsizei)(event.GetHeight()));
        mCamera.SetViewSize((float)event.GetWidth() * (1 - mRightPanelWidth),
                            (float)event.GetHeight());

        return false;
    }

    void ECSLayer::CreateScene() { mCurrentScene = Scene(); }

    void ECSLayer::LoadScene(const std::filesystem::path &path) {}
    void ECSLayer::SaveScene(const std::filesystem::path &path) {}

    void ECSLayer::DrawEntity(Entity const &entity, glm::mat4 parent) {
        if (!entity.HasComponent<Mesh>())
            return;

        auto &transform = entity.GetComponent<Transform>();

        auto model = transform.ModelMatrix();

        bool hasTexture = entity.HasComponent<Texture>();
        mShader->SetBool("hasTexture", hasTexture);
        if (hasTexture) {
            auto const &texture = entity.GetComponent<Texture>();
            glActiveTexture(GL_TEXTURE0);
            mShader->SetInt("texture", 0);
            glBindTexture(GL_TEXTURE_2D, texture.id);
        } else {
            mShader->SetVec3("uColor", {0.7, 0.5, 1.0});
        }

        mShader->SetVec3("uAmbient", entity.HasComponent<Material>()
                                         ? entity.GetComponent<Material>().ambient
                                         : vec3{0.1f, 0.1f, 0.1f});

        mShader->SetMat4("model", model);

        auto cameraDist = glm::length(transform.position - mCamera.GetPosition());
        LODLevel detail = NORMAL;

        if (cameraDist > 5.0f) {
            detail = LOW;
        } else if (cameraDist > 3.0f) {
            detail = MEDIUM;
        } else if (cameraDist > 1.5f) {
            detail = HIGH;
        }

        auto const &mesh = entity.GetComponent<Mesh>();
        glBindVertexArray(mesh.VAO(detail));
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh.IndicesSize(detail), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}