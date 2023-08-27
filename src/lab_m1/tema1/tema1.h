#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class MapS : public gfxc::SimpleScene
    {
    public:
        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {}
            float x;
            float y;
            float width;
            float height;
        };

     public:
        MapS();
        ~MapS();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        glm::mat3 VisualizationTransf2D(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);
        void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);
        void MapS::DrawWalls(glm::mat3 visMatrix);
        void MapS::DrawEnemy(float x, float y, float player_x, float player_y);
        bool MapS::CheckColl(float player_x, float player_y);
        bool MapS::CheckEnemColl(float enem_x, float enem_y, float player_x, float player_y);
        bool MapS::CheckCollWithEnem(glm::vec3 bullet_data, float bul_off_x);
        bool MapS::CheckCollBulWalls(glm::vec3 bullet_data, float bul_off_x);

     protected:
        ViewportSpace viewSpace;
        LogicSpace logicSpace;
        // Vector pentru matricea de desenare a fiecarui proiectil
        std::vector<glm::vec3> bullets;
        // Vector care retine cu cat trebuie translatat un proiectil
        std::vector<float> bul_x;
        std::vector<std::pair<float, float>> coord_enem;
        std::vector<std::pair<float, float>> center_enem;
        std::vector<std::pair<float, float>> coord_walls;
        std::vector<std::pair<float, float>> size_walls;
        glm::mat3 modelMatrix, visMatrix;
        glm::mat3 rot;
        float scaleX, life, angle, angle_bullet, enemies_touched;
        float map_sz_x, map_sz_y, scale_score, score_fact;
        float offset_hands_x, offset_hands_y, old_log_x, old_log_y, old_mx, old_my;
        float timer, timer_enem, player_x, player_y;
        int nr_bullets, nr_enemies;
        bool bullet_add, timer_bul, add_enem;
    };
}   // namespace m1
