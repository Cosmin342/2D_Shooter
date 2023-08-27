#include "tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/tema1/transform2D.h"
#include "func2D.h"

#define WALLS_NR            18
#define BULLET_SPEED        0.07f
#define PLAYER_SPEED        0.05f
#define BAR_DIM_X           1.5f
#define BAR_DIM_Y           0.5f
#define BULLET_X            0.24f
#define BULLET_Y            0.16f
#define TIMER_SPAWN_ENEM    6
#define HEALTH_BAR_Y        8
#define SCORE_BAR_Y         7.2f
#define MAX_RANGE_BULLET    3.8f
#define ENEMY_SIZE          0.7f
#define RADIUS_BULLET       0.12f
#define MAP_X               40
#define MAP_Y               23
#define RADIUS_PLAYER       0.42f
#define RADIUS_HAND         0.14f

using namespace std;
using namespace m1;

MapS::MapS()
{
}


MapS::~MapS()
{
}


void MapS::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    logicSpace.x = 0;       // logic x
    logicSpace.y = 0;       // logic y
    logicSpace.width = 16;   // logic width
    logicSpace.height = 9;  // logic height

    glm::vec3 corner = glm::vec3(0, 0, 0);

    offset_hands_x = corner.x + 0.39f;
    offset_hands_y = corner.y + 0.26f;

    old_log_x = 0;
    old_log_y = 0;
    map_sz_x = MAP_X;
    map_sz_y = MAP_Y;

    scaleX = 1.5;
    enemies_touched = 0;
    scale_score = 0;
    score_fact = 0;
    life = 1;

    bullet_add = false;
    timer_bul = false;
    add_enem = false;

    angle = 0;
    angle_bullet = 0;

    nr_bullets = 0;
    nr_enemies = 0;
    timer_enem = 0;

    player_x = 8;
    player_y = 4.5f;
    timer = 1;

    rot = transform2D::Rotate(angle);

    Mesh* map = func2D::CreateRectangle("map", corner, map_sz_x, map_sz_y, glm::vec3(0, 0.8f, 0.8f), true);
    AddMeshToList(map);

    Mesh* wall = func2D::CreateRectangle("wall", corner, 1, 2, glm::vec3(0, 0.8f, 0), true);
    AddMeshToList(wall);

    Mesh* enem_part = func2D::CreateRectangle("enem_part", corner, ENEMY_SIZE, ENEMY_SIZE, glm::vec3(1, 0.5f, 0), true);
    AddMeshToList(enem_part);

    Mesh* enem_part_bord = func2D::CreateRectangle("enem_part_bord", corner, ENEMY_SIZE + 0.04f,
        ENEMY_SIZE + 0.04f, glm::vec3(0.5, 0.5, 0.5), true);
    AddMeshToList(enem_part_bord);

    Mesh* health = func2D::CreateRectangle("health", corner, BAR_DIM_X, BAR_DIM_Y, glm::vec3(1, 1, 0));
    AddMeshToList(health);

    Mesh* healthb = func2D::CreateRectangle("healthb", corner, BAR_DIM_X, BAR_DIM_Y, glm::vec3(1, 1, 0), true);
    AddMeshToList(healthb);

    Mesh* score = func2D::CreateRectangle("score", corner, BAR_DIM_X, BAR_DIM_Y, glm::vec3(0, 0.6f, 0.3f));
    AddMeshToList(score);

    Mesh* scoreb = func2D::CreateRectangle("scoreb", corner, BAR_DIM_X, BAR_DIM_Y, glm::vec3(0, 0.6f, 0.3f), true);
    AddMeshToList(scoreb);

    Mesh* player_body = func2D::CreateCircle("player_body", corner, RADIUS_PLAYER, glm::vec3(1, 1, 0.5), true);
    AddMeshToList(player_body);

    Mesh* player_hand = func2D::CreateCircle("player_hand", corner, RADIUS_HAND, glm::vec3(1, 1, 0.5), true);
    AddMeshToList(player_hand);

    Mesh* player_border = func2D::CreateCircle("player_border", corner, RADIUS_PLAYER + 0.04f, glm::vec3(0.5, 0.5, 0.5), true);
    AddMeshToList(player_border);

    Mesh* player_hand_bord = func2D::CreateCircle("player_hand_bord", corner, RADIUS_HAND + 0.04f, glm::vec3(0.5, 0.5, 0.5), true);
    AddMeshToList(player_hand_bord);

    Mesh* bullet = func2D::CreateRectangle("bullet", corner, BULLET_X, BULLET_Y, glm::vec3(0, 0, 0), true);
    AddMeshToList(bullet);
}


// 2D visualization matrix
glm::mat3 MapS::VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
    float sx, sy, tx, ty;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    tx = viewSpace.x - sx * logicSpace.x;
    ty = viewSpace.y - sy * logicSpace.y;

    return glm::transpose(glm::mat3(
        sx, 0.0f, tx,
        0.0f, sy, ty,
        0.0f, 0.0f, 1.0f));
}


glm::mat3 MapS::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}


void MapS::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}


void MapS::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void MapS::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->GetResolution();
    /*
    Daca timer_bul e true, vor scadea secunde din timpul in care jucatorul nu are voie
    sa traga cu proiectile
    */
    if (timer_bul) {
        timer -= deltaTimeSeconds;
    }

    //Daca au trecut secundele, se reseteaza timer-ul, iar jucatorul poate trage proiectile
    if (timer <= 0) {
        timer = 1;
        timer_bul = false;
    }
    
    //Se incrementeaza si timer-ul pentru inamici
    timer_enem += deltaTimeSeconds;

    //Daca ajunge la o anumita valoare, acesta se reseteaza, si vor aparea noi inamici
    if (timer_enem >= TIMER_SPAWN_ENEM) {
        add_enem = true;
        timer_enem = 0;
    }
    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0), true);

    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

    //Se deseneaza mainile player-ului si contururile acestora
    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(logicSpace.x + logicSpace.width / 2, logicSpace.y + logicSpace.height / 2) *
        rot * transform2D::Translate(-offset_hands_x, offset_hands_y);
    RenderMesh2D(meshes["player_hand"], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["player_hand_bord"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(logicSpace.x + logicSpace.width / 2, logicSpace.y + logicSpace.height / 2) *
        rot * transform2D::Translate(-offset_hands_x, -offset_hands_y);
    RenderMesh2D(meshes["player_hand"], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["player_hand_bord"], shaders["VertexColor"], modelMatrix);

    //Apoi se va desena corpul player-ului
    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(logicSpace.x + logicSpace.width / 2, logicSpace.y + logicSpace.height / 2);
    RenderMesh2D(meshes["player_body"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(logicSpace.x + logicSpace.width / 2, logicSpace.y + logicSpace.height / 2);
    RenderMesh2D(meshes["player_border"], shaders["VertexColor"], modelMatrix);

    if (add_enem == true) {
        float x_enem, y_enem;
        //Cand se poate, se vor genera 3 noi inamici la pozitii random de pe harta
        for (int i = 0; i < 3; i++) {
            x_enem = (float) (rand() % 36) + 1;
            y_enem = (float) (rand() % 21) + 1;
            /*
            Se salveaza coordonatele centrelor inamicilor si cele ale colturilor de
            unde porneste desenarea
            */
            coord_enem.push_back(make_pair(x_enem, y_enem));
            center_enem.push_back(make_pair(x_enem + ENEMY_SIZE / 2, y_enem + ENEMY_SIZE / 2));
            nr_enemies++;
        }
        add_enem = false;
    }
    float dx, dy;
    //Se calculeaza noile pozitii ale inamicilor si apoi acestia sunt desenati
    for (int i = 0; i < nr_enemies; i++) {
        dx = player_x - coord_enem[i].first;
        dy = player_y - coord_enem[i].second;

        coord_enem[i].first = coord_enem[i].first + dx / (float)sqrt((double)dx * dx + dy * dy) * 1 * deltaTimeSeconds;
        coord_enem[i].second = coord_enem[i].second + dy / (float)sqrt((double)dx * dx + dy * dy) * 1 * deltaTimeSeconds;

        DrawEnemy(coord_enem[i].first, coord_enem[i].second, player_x, player_y);

        dx = player_x - center_enem[i].first;
        dy = player_y - center_enem[i].second;

        center_enem[i].first = center_enem[i].first + dx / (float)sqrt((double)dx * dx + dy * dy) * 1 * deltaTimeSeconds;
        center_enem[i].second = center_enem[i].second + dy / (float)sqrt((double)dx * dx + dy * dy) * 1 * deltaTimeSeconds;
    }

    //Se deseneaza bara de viata si cea de scor
    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(logicSpace.x + 1, logicSpace.y + HEALTH_BAR_Y);
    RenderMesh2D(meshes["health"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(logicSpace.x + 1, logicSpace.y + HEALTH_BAR_Y) *
        transform2D::Scale(life, 1);
    RenderMesh2D(meshes["healthb"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(logicSpace.x + 1, logicSpace.y + SCORE_BAR_Y);
    RenderMesh2D(meshes["score"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(logicSpace.x + 1, logicSpace.y + SCORE_BAR_Y);
    modelMatrix *= transform2D::Scale(score_fact, 1);
    RenderMesh2D(meshes["scoreb"], shaders["VertexColor"], modelMatrix);

    //Se adauga un nou proiectil, daca timer-ul nu este pornit
    if (bullet_add == true && timer == 1) {
        glm::vec3 tup = { old_log_x, old_log_y, angle_bullet };
        bullets.push_back(tup);
        bul_x.push_back(0);

        bullet_add = false;
        timer_bul = true;
    }

    /*
    Se deseneaza gloantele, verificandu - se in prealabil eventualele coliziuni sau daca un glont
    a atins distanta maxima
    */
    for (int i = 0; i < nr_bullets; i++) {
        if (bul_x[i] >= MAX_RANGE_BULLET || CheckCollWithEnem(bullets[i], bul_x[i])
            || CheckCollBulWalls(bullets[i], bul_x[i])) {
            //In caz afirmativ, glontul respectiv este eliminat
            bul_x.erase(bul_x.begin() + i);
            bullets.erase(bullets.begin() + i);
            nr_bullets--;
            continue;
        }

        modelMatrix = visMatrix;
        /*
        Intai glontul este translatat in mijlocul ferestrei, apoi este rotit cat sa fie orientat catre mouse, iar
        apoi translatat pe directia pozitiei mouse-ului
        */
        modelMatrix *= transform2D::Translate(bullets[i][0] + logicSpace.width / 2, bullets[i][1] + logicSpace.height / 2) *
            transform2D::Rotate(-bullets[i][2]) * transform2D::Translate(-offset_hands_x - 0.45f, -offset_hands_y + 0.16f) *
            transform2D::Translate(-bul_x[i], 0);
        RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
        bul_x[i] += BULLET_SPEED;
    }

    //Se deseneaza harta si obstacolele
    DrawWalls(visMatrix);
    RenderMesh2D(meshes["map"], shaders["VertexColor"], visMatrix * transform2D::Translate(0, 0));
}

/*
Se deseneaza zidurile si se retin date despre acestea, dar si despre marginile hartii
(si acelea sunt considerate ziduri)
*/
void MapS::DrawWalls(glm::mat3 visMatrix) {
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], visMatrix * transform2D::Translate(1, 8) *
        transform2D::Scale(1.5, 2));
    coord_walls.push_back(make_pair(1.0f, 8.0f));
    size_walls.push_back(make_pair(1.5f, 4.0f));
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], visMatrix * transform2D::Translate(10, 2) *
        transform2D::Scale(1, 3));
    coord_walls.push_back(make_pair(10.0f, 2.0f));
    size_walls.push_back(make_pair(1.0f, 6.0f));
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], visMatrix * transform2D::Translate(10, 8) *
        transform2D::Scale(5, 1));
    coord_walls.push_back(make_pair(10.0f, 8.0f));
    size_walls.push_back(make_pair(5.0f, 2.0f));
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], visMatrix * transform2D::Translate(17, 1.3f) *
        transform2D::Scale(2, 1));
    coord_walls.push_back(make_pair(17.0f, 1.3f));
    size_walls.push_back(make_pair(2.0f, 2.0f));
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], visMatrix * transform2D::Translate(17, 1.3f) *
        transform2D::Scale(-5, 1));
    coord_walls.push_back(make_pair(12.0f, 1.3f));
    size_walls.push_back(make_pair(5.0f, 2.0f));
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], visMatrix * transform2D::Translate(22, 3) *
        transform2D::Scale(1, 4));
    coord_walls.push_back(make_pair(22.0f, 3.0f));
    size_walls.push_back(make_pair(1.0f, 8.0f));
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], visMatrix * transform2D::Translate(28, 7) *
        transform2D::Scale(1, 4));
    coord_walls.push_back(make_pair(28.0f, 7.0f));
    size_walls.push_back(make_pair(1.0f, 8.0f));
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], visMatrix * transform2D::Translate(34, 4) *
        transform2D::Scale(3, 5));
    coord_walls.push_back(make_pair(34.0f, 4.0f));
    size_walls.push_back(make_pair(3.0f, 10.0f));
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], visMatrix * transform2D::Translate(5, 12) *
        transform2D::Scale(12, 1));
    coord_walls.push_back(make_pair(5.0f, 12.0f));
    size_walls.push_back(make_pair(12.0f, 2.0f));
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], visMatrix * transform2D::Translate(2.5f, 15) *
        transform2D::Scale(1, 4));
    coord_walls.push_back(make_pair(2.5f, 15.0f));
    size_walls.push_back(make_pair(1.0f, 8.0f));
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], visMatrix * transform2D::Translate(8, 20) *
        transform2D::Scale(22, 0.5f));
    coord_walls.push_back(make_pair(8.0f, 20.0f));
    size_walls.push_back(make_pair(22.0f, 1.0f));
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], visMatrix * transform2D::Translate(25, 15) *
        transform2D::Scale(7, 0.5f));
    coord_walls.push_back(make_pair(25.0f, 15.0f));
    size_walls.push_back(make_pair(7.0f, 1.0f));
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], visMatrix * transform2D::Translate(37, 16) *
        transform2D::Scale(1, 3));
    coord_walls.push_back(make_pair(37.0f, 16.0f));
    size_walls.push_back(make_pair(1.0f, 6.0f));
    RenderMesh2D(meshes["wall"], shaders["VertexColor"], visMatrix * transform2D::Translate(32, 14) *
        transform2D::Scale(1, 4));
    coord_walls.push_back(make_pair(32.0f, 14.0f));
    size_walls.push_back(make_pair(1.0f, 8.0f));

    //Marginea de jos a hartii
    coord_walls.push_back(make_pair(0.0f, -2.0f));
    size_walls.push_back(make_pair(40.0f, 2.0f));

    //Marginea de sus a hartii
    coord_walls.push_back(make_pair(0.0f, map_sz_y));
    size_walls.push_back(make_pair(map_sz_x, 2.0f));

    //Marginea din dreapta hartii
    coord_walls.push_back(make_pair(map_sz_x, 0.0f));
    size_walls.push_back(make_pair(2.0f, map_sz_y));

    //Marginea din stanga hartii
    coord_walls.push_back(make_pair(-2.0f, 0.0f));
    size_walls.push_back(make_pair(2.0f, map_sz_y));
}

void MapS::DrawEnemy(float x, float y, float player_x, float player_y) {
    //Se calculeaza unghiul pentru rotirea catre jucator a inamicului
    float angle_enem = (float)atan2(x - (double)player_x, y - (double)player_y);
    glm::mat3 rot_enem = transform2D::Rotate(-angle_enem);    

    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(x + ENEMY_SIZE / 2, y + ENEMY_SIZE / 2) * rot_enem *
        transform2D::Translate(0.175f, -0.45f) * transform2D::Scale(0.25f, 0.25f);
    RenderMesh2D(meshes["enem_part"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(x + ENEMY_SIZE / 2, y + ENEMY_SIZE / 2) * rot_enem *
        transform2D::Translate(0.165f, -0.47f) * transform2D::Scale(0.28f, 0.28f);
    RenderMesh2D(meshes["enem_part_bord"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(x + ENEMY_SIZE / 2, y + ENEMY_SIZE / 2) * rot_enem *
        transform2D::Translate(-ENEMY_SIZE / 2, -0.45f) * transform2D::Scale(0.25f, 0.25f);
    RenderMesh2D(meshes["enem_part"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(x + ENEMY_SIZE / 2, y + ENEMY_SIZE / 2) * rot_enem *
        transform2D::Translate(-0.37f, -0.47f) * transform2D::Scale(0.28f, 0.28f);
    RenderMesh2D(meshes["enem_part_bord"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(x + ENEMY_SIZE / 2, y + ENEMY_SIZE / 2) * rot_enem *
        transform2D::Translate(-ENEMY_SIZE / 2, -ENEMY_SIZE / 2);
    RenderMesh2D(meshes["enem_part"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(x + ENEMY_SIZE / 2, y + ENEMY_SIZE / 2) * rot_enem
        * transform2D::Translate(-0.37f, -0.37f);
    RenderMesh2D(meshes["enem_part_bord"], shaders["VertexColor"], modelMatrix);
}

void MapS::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

bool MapS::CheckCollBulWalls(glm::vec3 bullet_data, float bul_off_x) {
    /*
    Se determina centrul proiectilului(valorile din formula care sunt adunate / scazute la x si y
    sunt luate din matricea de desenare a unui glont)
    */
    float x_center_bul = bullet_data[0] + logicSpace.width / 2 + (-offset_hands_x - 0.45f - bul_off_x + 0.12f) *
        cos(-bullet_data[2]) - (-offset_hands_y + 0.16f + 0.08f) * sin(-bullet_data[2]);
    float y_center_bul = bullet_data[1] + logicSpace.height / 2 + (-offset_hands_x - 0.45f - bul_off_x + 0.12f) *
        sin(-bullet_data[2]) + (-offset_hands_y + 0.16f + 0.08f) * cos(-bullet_data[2]);
    float radius_bul = RADIUS_BULLET;
    float x_wall_cen, y_wall_cen, x_dif, y_dif;
    glm::vec2 bul_coord, clamped, closest, difference;
    for (int i = 0; i < WALLS_NR; i++) {
        x_wall_cen = coord_walls[i].first + size_walls[i].first / 2;
        y_wall_cen = coord_walls[i].second + size_walls[i].second / 2;

        bul_coord.x = x_center_bul;
        bul_coord.y = y_center_bul;

        //Se calculeaza diferenta dintre centrul proiectilului si al zidului
        x_dif = x_center_bul - x_wall_cen;
        y_dif = y_center_bul - y_wall_cen;

        //Se determina cel mai apropiat punct de proiectil
        clamped = glm::clamp(glm::vec2(x_dif, y_dif),
            -glm::vec2(size_walls[i].first / 2, size_walls[i].second / 2),
            glm::vec2(size_walls[i].first / 2, size_walls[i].second / 2));

        closest = glm::vec2(x_wall_cen, y_wall_cen) + clamped;

        difference = closest - bul_coord;

        /*
        Se verifica daca distanta pana la cel mai apropiat punct de pe zid este mai mica decat
        raza proiectilului
        */
        if (glm::length(difference) < radius_bul)
            return true;
    }
    return false;
}

bool MapS::CheckCollWithEnem(glm::vec3 bullet_data, float bul_off_x) {
    //Se determina centrul proiectilului
    float x_center_bul = bullet_data[0] + logicSpace.width / 2 + (-offset_hands_x - 0.45f - bul_off_x + 0.12f) *
        cos(-bullet_data[2]) - (-offset_hands_y + 0.16f + 0.08f) * sin(-bullet_data[2]);
    float y_center_bul = bullet_data[1] + logicSpace.height / 2 + (-offset_hands_x - 0.45f - bul_off_x + 0.12f) *
        sin(-bullet_data[2]) + (-offset_hands_y + 0.16f + 0.08f) * cos(-bullet_data[2]);
    float dx, dy;
    float radius_bul = RADIUS_BULLET;
    float radius_enem = ENEMY_SIZE / 2;
    for (int i = 0; i < nr_enemies; i++) {
        //Se calculeaza distanta dintre centrul unui inamic si cel al glontului
        dx = (x_center_bul + radius_bul) - (center_enem[i].first + radius_enem);
        dy = (y_center_bul + radius_bul) - (center_enem[i].second + radius_enem);
        //Daca distanta este mai mica decat raza celor doi, apare o coliziune
        if (sqrt(dx * dx + dy * dy) < radius_bul + radius_enem) {
            //Inamicul atins este eliminat
            coord_enem.erase(coord_enem.begin() + i);
            center_enem.erase(center_enem.begin() + i);
            nr_enemies--;
            //Creste scorul jucatorului
            scale_score += 0.05f;
            score_fact = scale_score / scaleX;
            //Daca acesta scale_score ajunge egal cu scaleX (scorul maxim), jocul se incheie
            if (fabs(score_fact - 1.00f) < 0.01f) {
                cout << "Ai castigat!\n";
                exit(0);
            }
            return true;
        }
    }
    return false;
}

void MapS::OnInputUpdate(float deltaTime, int mods)
{
    //Player-ul se misca utilizand tastele W, A, S si D
    /*
    Inainte si dupa orice miscare, se verifica o eventuala coliziune cu un zid sau cu
    marginea hartii
    */
    if (window->KeyHold(GLFW_KEY_W)) {
        if (CheckColl(player_x, player_y) == false) {
            logicSpace.y += PLAYER_SPEED;
            player_y += PLAYER_SPEED;
        }
        if (CheckColl(player_x, player_y) == true) {
            logicSpace.y -= PLAYER_SPEED;
            player_y -= PLAYER_SPEED;
        }
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        if (CheckColl(player_x, player_y) == false) {
            logicSpace.y -= PLAYER_SPEED;
            player_y -= PLAYER_SPEED;
        }
        if (CheckColl(player_x, player_y) == true) {
            logicSpace.y += PLAYER_SPEED;
            player_y += PLAYER_SPEED;
        }
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        if (CheckColl(player_x, player_y) == false) {
            logicSpace.x += PLAYER_SPEED;
            player_x += PLAYER_SPEED;
        }
        if (CheckColl(player_x, player_y) == true) {
            logicSpace.x -= PLAYER_SPEED;
            player_x -= PLAYER_SPEED;
        }
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        if (CheckColl(player_x, player_y) == false) {
            logicSpace.x -= PLAYER_SPEED;
            player_x -= PLAYER_SPEED;
        }
        if (CheckColl(player_x, player_y) == true) {
            logicSpace.x += PLAYER_SPEED;
            player_x += PLAYER_SPEED;
        }
    }
    //De asemenea, se verifica si o eventuala coliziune a player-ului cu inamicii
    for (int i = 0; i < nr_enemies; i++) {
        if (CheckEnemColl(center_enem[i].first, center_enem[i].second, player_x, player_y)) {
            //Se elimina inamicul in caz de coliziune
            coord_enem.erase(coord_enem.begin() + i);
            center_enem.erase(center_enem.begin() + i);
            nr_enemies--;
            //Scade viata player-ului
            enemies_touched += 0.1f;
            life = (scaleX - enemies_touched) / scaleX;
            //Daca viata player-ului ajunge la 0, jocul se incheie
            if (fabs(life - 0.00f) < 0.01f) {
                cout << "Ai pierdut!\n";
                exit(0);
            }
        }
    }
}

bool MapS::CheckEnemColl(float enem_x, float enem_y, float player_x, float player_y) {
    //Se verifica intai o coliziune dintre inamic si jucator (ambii sunt considerati cercuri)
    float dx = (enem_x + ENEMY_SIZE / 2) - (player_x + RADIUS_PLAYER + 0.04f);
    float dy = (enem_y + ENEMY_SIZE / 2) - (player_y + RADIUS_PLAYER + 0.04f);
    //Daca distanta este mai mica decat cele doua raze adunate, exista coliziune
    if (sqrt(dx * dx + dy * dy) < ENEMY_SIZE / 2 + RADIUS_PLAYER + 0.04f)
        return true;
    float x_wall_cen, y_wall_cen, x_dif, y_dif;
    glm::vec2 enem_coord, difference, clamped, closest;
    //Apoi se verifica doar limitele hartii (in cazul in care inamicul iese din harta)
    for (int i = 14; i < WALLS_NR; i++) {
        x_wall_cen = coord_walls[i].first + size_walls[i].first / 2;
        y_wall_cen = coord_walls[i].second + size_walls[i].second / 2;

        enem_coord.x = enem_x;
        enem_coord.y = enem_y;

        x_dif = enem_x - x_wall_cen;
        y_dif = enem_y - y_wall_cen;

        clamped = glm::clamp(glm::vec2(x_dif, y_dif),
            -glm::vec2(size_walls[i].first / 2, size_walls[i].second / 2),
            glm::vec2(size_walls[i].first / 2, size_walls[i].second / 2));

        closest = glm::vec2(x_wall_cen, y_wall_cen) + clamped;

        difference = closest - enem_coord;

        if (glm::length(difference) < ENEMY_SIZE / 2)
            return true;
    }
    return false;
}

bool MapS::CheckColl(float player_x, float player_y) {
    float x_wall_cen, y_wall_cen, x_dif, y_dif;
    glm::vec2 player_coord, difference, clamped, closest;
    for (int i = 0; i < WALLS_NR; i++) {

        x_wall_cen = coord_walls[i].first + size_walls[i].first / 2;
        y_wall_cen = coord_walls[i].second + size_walls[i].second / 2;

        player_coord.x = player_x;
        player_coord.y = player_y;
        
        x_dif = player_x - x_wall_cen;
        y_dif = player_y - y_wall_cen;

        clamped = glm::clamp(glm::vec2(x_dif, y_dif),
            -glm::vec2(size_walls[i].first / 2, size_walls[i].second / 2),
            glm::vec2(size_walls[i].first / 2, size_walls[i].second / 2));

        closest = glm::vec2(x_wall_cen, y_wall_cen) + clamped;

        difference = closest - player_coord;

        if (glm::length(difference) < RADIUS_PLAYER + 0.04f)
            return true;
    }
    return false;
}


void MapS::OnKeyPress(int key, int mods)
{

}


void MapS::OnKeyRelease(int key, int mods)
{

}


void MapS::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    //Se calculeaza unghiul pentru rotirea player-ului catre mouse si matricea de rotatie
    angle = (float)atan2(window->GetResolution().y / 2 - (double)mouseY,
        window->GetResolution().x / 2 - (double)mouseX);
    rot = transform2D::Rotate(-angle);
}


void MapS::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    //Daca se da click stanga, se va adauga un nou proiectil
    if (button == 1 && timer == 1) {
        bullet_add = true;
        /*
        Se retin unghiul pentru rotatia in directia mouse - ului si actualele coordonate
        ale spatiului logic
        */
        angle_bullet = angle;
        nr_bullets++;
        old_log_x = logicSpace.x;
        old_log_y = logicSpace.y;
    }
}


void MapS::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{

}


void MapS::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void MapS::OnWindowResize(int width, int height)
{
}
