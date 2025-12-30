#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <sstream>

#include "Globals.h"
#include "Shader.h"
#include "Camera_test.h"
#include "Some_functions.h"
#include "TextRenderer.h"
#include "game_object_basic.h"


const double Target_fps = 144;
const double Target_frame_time = 1.0 / Target_fps;
const bool enable_vSync = false;

const unsigned int width = 800, height = 600;
const float aspect_ratio = (float)width / (float)height;

float mouse_lastX = width / 2, mouse_lastY = height / 2;

const float mouse_sensitivity = 0.3f;

int new_lighting = 0;

const float lighting_mulp_x = 15;
const float lighting_mulp_y = 50;


camera_test camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

TextRenderer* printer;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //// make sure the viewport matches the new window dimensions; note that width and 
    //// height will be significantly larger than specified on retina displays.
    const float new_aspect_ratio = (float)width / (float)height;
    //if (new_aspect_ratio < aspect_ratio)
    //{
    //	//if the new aspect ratio is smaller than the original one, we need to adjust the viewport
    //	int new_height = (int)(width / aspect_ratio);
    //	glViewport(0, (height - new_height) / 2, width, new_height);
    //}
    //else
    //{
    //	//if the new aspect ratio is larger than the original one, we need to adjust the viewport
    //	int new_width = (int)(height * aspect_ratio);
    //	glViewport((width - new_width) / 2, 0, new_width, height);
    //}
    glViewport(0, 0, width, height);

    camera.update_projection(45.0f, new_aspect_ratio, 0.1f, 1000.0f);
    printer->change_screen_size(width, height);
}

bool pressable = true;
void processInput(GLFWwindow* window, float camera_speed, camera_test& camera)
{

    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && pressable)
    {
        static bool is_fullscreen = false;
        static int windowed_xpos = 100, windowed_ypos = 100, windowed_width = 800, windowed_height = 600;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (!is_fullscreen)
        {
            glfwGetWindowPos(window, &windowed_xpos, &windowed_ypos);
            glfwGetWindowSize(window, &windowed_width, &windowed_height);
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            is_fullscreen = true;
        }
        else
        {
            glfwSetWindowMonitor(window, nullptr, windowed_xpos, windowed_ypos, windowed_width, windowed_height, 0);
            is_fullscreen = false;
        }
        pressable = false;
    }
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_RELEASE)
    {
        pressable = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera_speed *= 2;

    bool changes[6] = { false, false, false, false, false, false };

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        changes[0] = true; // front change
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        changes[1] = true; // back change
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        changes[2] = true; // left change
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        changes[3] = true; // right change
    }


    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        changes[4] = true; // up change
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        changes[5] = true; // down change
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.camera_tilt(-camera_speed * 5);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.camera_tilt(camera_speed * 5);
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    camera.camera_move(
        1, camera_speed,
        changes[0], changes[1],
        changes[2], changes[3],
        changes[4], changes[5]);
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos)
{
    const float xoffset = (x_pos - mouse_lastX) * mouse_sensitivity;
    const float yoffset = (y_pos - mouse_lastY) * mouse_sensitivity;

    mouse_lastX = x_pos;
    mouse_lastY = y_pos;

    camera.process_mouse_movement(xoffset, yoffset, mouse_sensitivity);

}

using namespace std;

// ===================================
// GRID CONFIG
// ===================================
const int GRID_SIZE = 50;

// ===================================
// CELL STATES
// ===================================
enum class CellState {
    Empty,
    Growing,
    Mature,
    Burning,
    Burnt
};

// ===================================
// CELL STRUCT (SADE)
// ===================================
struct Cell {
    CellState state;
    float growthTime; // Growing için sayaç
    float burnTime;   // Burning & Burnt için sayaç
};

// ===================================
// FOREST GRID
// ===================================
Cell forest[GRID_SIZE][GRID_SIZE];

// ===================================
// SIMULATION PARAMETERS
// ===================================
const int   INITIAL_TREE_COUNT = 3;

const float SPACING = 5.0f;

const float TREE_GROW_TIME = 5.0f;   // Growing -> Mature
const float TREE_BURN_TIME = 6.0f;   // Burning -> Burnt (x)
const float EMPTY_COOLDOWN_TIME = 6.0f;   // Burnt -> Empty (.)

const float TREE_SPREAD_CHANCE = 0.15f;  // Ağaç yayılması
const float FIRE_SPREAD_BASE = 0.35f;  // Yangın temel ihtimali
const float LIGHTNING_CHANCE = 0.02f;  // Rastgele yangın başlatma

// RÜZGAR (sabit – şimdilik)
const int   WIND_X = 1;   // sağa
const int   WIND_Y = 0;
const float WIND_STRENGTH = 0.6f;

// ===================================
// UTILITY
// ===================================
static inline float clamp01(float v) {
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

bool isInsideGrid(int x, int y) {
    return x >= 0 && x < GRID_SIZE &&
        y >= 0 && y < GRID_SIZE;
}

bool chance(float p) {
    return (rand() / (float)RAND_MAX) < p;
}

// Rüzgâra göre yangın olasılığı
float fireSpreadWithWind(int dirX, int dirY) {
    int dot = dirX * WIND_X + dirY * WIND_Y; // -1,0,1
    float factor = 1.0f + WIND_STRENGTH * dot;
    return clamp01(FIRE_SPREAD_BASE * factor);
}

// ===================================
// INITIALIZATION
// ===================================
void initializeForest() {
    srand(static_cast<unsigned>(time(nullptr)));

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            forest[y][x].state = CellState::Empty;
            forest[y][x].growthTime = 0.0f;
            forest[y][x].burnTime = 0.0f;
        }
    }

    // Zar at → birkaç ağaç
    int planted = 0;
    while (planted < INITIAL_TREE_COUNT) {
        int x = rand() % GRID_SIZE;
        int y = rand() % GRID_SIZE;

        if (forest[y][x].state == CellState::Empty) {
            forest[y][x].state = CellState::Growing;
            forest[y][x].growthTime = 0.0f;
            planted++;
        }
    }
}

// ===================================
// SIMULATION UPDATE
// ===================================
void updateSimulation(float dt, game_object_basic_model& liggtning, shared_ptr<class_region> region) {

    bool willIgnite[GRID_SIZE][GRID_SIZE] = { false };
    bool willGrow[GRID_SIZE][GRID_SIZE] = { false };

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {

            Cell& cell = forest[y][x];

            switch (cell.state) {

            case CellState::Growing:
                cell.growthTime += dt;
                if (cell.growthTime >= TREE_GROW_TIME) {
                    cell.state = CellState::Mature;
                }
                break;

            case CellState::Mature: {
                // Ağaç yayılması
                const int dx[4] = { 1, -1, 0, 0 };
                const int dy[4] = { 0, 0, 1, -1 };

                for (int i = 0; i < 4; i++) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];

                    if (isInsideGrid(nx, ny)) {
                        if (forest[ny][nx].state == CellState::Empty) {
                            if (chance(TREE_SPREAD_CHANCE)) {
                                willGrow[ny][nx] = true;
                            }
                        }
                    }
                }
                break;
            }

            case CellState::Burning: {
                cell.burnTime += dt;

                const int dx[4] = { 1, -1, 0, 0 };
                const int dy[4] = { 0, 0, 1, -1 };

                for (int i = 0; i < 4; i++) {
                    int dirX = dx[i];
                    int dirY = dy[i];

                    int nx = x + dirX;
                    int ny = y + dirY;

                    if (isInsideGrid(nx, ny)) {
                        if (forest[ny][nx].state == CellState::Mature) {
                            float p = fireSpreadWithWind(dirX, dirY);
                            if (chance(p)) {
                                willIgnite[ny][nx] = true;
                            }
                        }
                    }
                }

                if (cell.burnTime >= TREE_BURN_TIME) {
                    cell.state = CellState::Burnt;
                    cell.burnTime = 0.0f;
                }
                break;
            }

            case CellState::Burnt:
                cell.burnTime += dt;
                if (cell.burnTime >= EMPTY_COOLDOWN_TIME) {
                    cell.state = CellState::Empty;
                    cell.burnTime = 0.0f;
                }
                break;

            default:
                break;
            }
        }
    }

    // === İKİNCİ AŞAMA ===

    // Yeni ağaçlar
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (willGrow[y][x]) {
                forest[y][x].state = CellState::Growing;
                forest[y][x].growthTime = 0.0f;
            }
        }
    }

    // Yeni yangınlar
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (willIgnite[y][x]) {
                forest[y][x].state = CellState::Burning;
                forest[y][x].burnTime = 0.0f;
            }
        }
    }

    // Rastgele yıldırım
    if (chance(LIGHTNING_CHANCE)) {
        int x = rand() % GRID_SIZE;
        int y = rand() % GRID_SIZE;

        if (forest[y][x].state == CellState::Mature)
        {
            forest[y][x].state = CellState::Burning;
            forest[y][x].burnTime = 0.0f;

            //load ligthning
            new_lighting = 6;
            vector<glm::mat4> model_matrices;
            glm::mat4 model = glm::mat4(1.0f);

            int random1 = (rand() % 200) - 100;
            float slide = (SPACING / 10) * ((float)random1 / 100.0f);

            random1 = (rand() % 200) - 100;
            float slide2 = (SPACING / 10) * ((float)random1 / 100.0f);
            model = glm::translate(model, glm::vec3((y * SPACING) + slide, 25.0f, (x * SPACING) + slide2));

            model = glm::scale(model, glm::vec3(lighting_mulp_x, lighting_mulp_y, 1));
            model_matrices.push_back(model);

            liggtning.load_instance_buffer((float*)model_matrices.data(), model_matrices.size(), 3, region);
        }
    }
}

int create_load_models(game_object_basic_model& tree,shared_ptr<class_region> tree_region, float tree_size_mulp)
{
    vector<glm::mat4> model_matrices;
	vector<glm::mat3> transpose_inverse_model_matrices;
    model_matrices.reserve(GRID_SIZE * GRID_SIZE);
	transpose_inverse_model_matrices.reserve(GRID_SIZE * GRID_SIZE);

	int instance_count = 0;
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {

            if (forest[y][x].state == CellState::Empty || forest[y][x].state == CellState::Burnt)
                continue;

			glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(y*SPACING, 0.0f,x*SPACING));

            if (forest[y][x].state == CellState::Mature || forest[y][x].state == CellState::Burning)
            {
                model = glm::scale(model, glm::vec3(tree_size_mulp, tree_size_mulp, tree_size_mulp));
            }
                

            if (forest[y][x].state == CellState::Growing)
            {
				float growth_ratio = forest[y][x].growthTime / TREE_GROW_TIME;
                growth_ratio *= tree_size_mulp;
                model = glm::scale(model, glm::vec3(growth_ratio, growth_ratio, growth_ratio));
            }
            
            

			model_matrices.push_back(model);
			transpose_inverse_model_matrices.push_back(glm::mat3(glm::transpose(glm::inverse(model))));
            instance_count++;
        }
    }

    tree.load_instance_buffer((float*)model_matrices.data(),model_matrices.size(),3,tree_region);
	tree.load_instance_buffer((float*)transpose_inverse_model_matrices.data(), transpose_inverse_model_matrices.size(), 7, tree_region);
	return instance_count;
}

int create_load_fire_models(game_object_basic_model& fire, shared_ptr<class_region> fire_region, float fire_size_mulp)
{
    vector<glm::mat4> model_matrices;
    model_matrices.reserve(GRID_SIZE * GRID_SIZE);

    int instance_count = 0;

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {

            if (forest[y][x].state != CellState::Burning)
                continue;

            glm::mat4 model = glm::mat4(1.0f);

            int random1 = (rand() % 200) - 100;
            float slide = (SPACING/10) * ((float)random1 / 100.0f);

            random1 = (rand() % 200) - 100;
            float slide2 = (SPACING/10) * ((float)random1 / 100.0f);
            model = glm::translate(model, glm::vec3((y * SPACING) + slide, 5.0f, (x * SPACING) + slide2));

            //fire_size_mulp *= 0.8 + (forest[y][x].burnTime / (TREE_BURN_TIME*5));
            model = glm::scale(model, glm::vec3(fire_size_mulp, fire_size_mulp, fire_size_mulp));
            model_matrices.push_back(model);
            instance_count++;
        }
    }

    fire.load_instance_buffer((float*)model_matrices.data(), model_matrices.size(), 3, fire_region);
    return instance_count;
}

// ===================================
// DEBUG VISUALIZATION
// ===================================
char cellChar(CellState s) {
    switch (s) {
    case CellState::Empty:   return '.';
    case CellState::Growing: return 'g';
    case CellState::Mature:  return 'T';
    case CellState::Burning: return 'F';
    case CellState::Burnt:   return 'x';
    default: return '?';
    }
}

void printForest() {
    system("cls");

    ostringstream oss;
    oss << "Wind: (" << WIND_X << "," << WIND_Y
        << ") strength=" << WIND_STRENGTH << "\n\n";

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            oss << cellChar(forest[y][x].state) << ' ';
        }
        oss << '\n';
    }

    cout << oss.str();
}

// ===================================
// MAIN
// ===================================
int main() {
    initializeForest();

    const float SIM_STEP = 0.5f;

    GLFWwindow* window = init_window(width, height, "Shader Tester");
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    if (!window) {
        return -1;
    }

    glfwSwapInterval(enable_vSync);

    camera.update_projection(45.0f, aspect_ratio, 0.1f, 1000.0f);

    printer = new TextRenderer("Textures\\Font_texture_Atlas\\DejaVu Sans Mono_512X256_16x32.png",
        "Textures\\Font_texture_Atlas\\DejaVu Sans Mono_512X256_16x32.txt", width, height, 16, 32,
        "Shaders\\Vertex_shaders\\Text_render_vertex.vert", "Shaders\\Fragment_shaders\\Text_render_fragment.frag", 0.005f);

    printer->change_deleted_colors(0, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.5f, glm::vec4(1.0f, 1.0f, 1.0f, 0.1f));
    printer->change_deleted_colors(1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.5f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    printer->push_deleted_colors();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    Shader shader("Shaders\\Vertex_shaders\\Loaded_model_vertex.vert", "Shaders\\Fragment_shaders\\Loaded_model_fragment.frag");
    Shader fire_shader("Shaders\\Vertex_shaders\\Basic_texture_vertex.vert", "Shaders\\Fragment_shaders\\Basic_texture_fragment.frag");

    Texture fire_texture;
    fire_texture.path = "C:\\Users\\altay\\Desktop\\forest_fire_assets\\fire.png";
    fire_texture.type = TextureType::DIFFUSE;
    int trash_data1, trash_data2, trash_data3;
    fire_texture.id = load_image(fire_texture.path.c_str(), trash_data1, trash_data2, trash_data3);

    Texture lightning_texture;
    lightning_texture.path = "C:\\Users\\altay\\Desktop\\forest_fire_assets\\lightning.png";
    lightning_texture.type = TextureType::DIFFUSE;
    lightning_texture.id = load_image(lightning_texture.path.c_str(), trash_data1, trash_data2, trash_data3);

    Texture ground_texture;
    ground_texture.path = "C:\\Users\\altay\\Desktop\\forest_fire_assets\\ground-diff.jpg";
    ground_texture.type = TextureType::DIFFUSE;
    ground_texture.id = load_image(ground_texture.path.c_str(), trash_data1, trash_data2, trash_data3);

    std::vector<Texture> textures_fire;
    textures_fire.push_back(fire_texture);

    std::vector<Texture> textures_lightning;
    textures_lightning.push_back(lightning_texture);

    std::vector<Texture> textures_ground;
    textures_ground.push_back(ground_texture);

    vertex_data left_up = { {-0.5,0.5,0},{0,1},{0,0,-1} };
    vertex_data left_down = { {-0.5,-0.5,0},{0,0},{0,0,-1} };
    vertex_data right_up = { {0.5,0.5,0},{1,1},{0,0,-1} };
    vertex_data right_down = { {0.5,-0.5,0},{1,0},{0,0,-1} };

    std::vector<vertex_data> vertices;
    vertices.push_back(left_down);
    vertices.push_back(left_up);
    vertices.push_back(right_up);
    vertices.push_back(right_down);

    std::vector<unsigned int> indices = {0, 1, 2, 2,3,0};

    game_object_basic_model fire;
    fire.add_mesh(vertices, indices, textures_fire);

    game_object_basic_model lightning;
    lightning.add_mesh(vertices, indices, textures_lightning);

    vertices[0] = { {-1,0,-1},{0,0},{0,1,0} };
    vertices[1] = { {-1,0, 1},{0,1},{0,1,0} };
    vertices[2] = { { 1,0, 1},{1,1},{0,1,0} };
    vertices[3] = { { 1,0,-1},{1,0},{0,1,0} };

    game_object_basic_model ground;
    ground.add_mesh(vertices, indices, textures_ground);

    game_object_basic_model tree;
    

    tree.import_model_from_file("C:\\Users\\altay\\Desktop\\forest_fire_assets\\tree_small_growing.obj");


    std::shared_ptr<class_region> grid_region = tree.reserve_class_region(GRID_SIZE * GRID_SIZE);
    std::shared_ptr<class_region> fire_region = fire.reserve_class_region(GRID_SIZE * GRID_SIZE);
    std::shared_ptr<class_region> ground_region = ground.reserve_class_region(GRID_SIZE * GRID_SIZE);
    std::shared_ptr<class_region> lightning_region = lightning.reserve_class_region(1);

    tree.add_instance_buffer(16, 3); //attrib size-mat4-16floats, attrib index, for model
    fire.add_instance_buffer(16, 3);
    ground.add_instance_buffer(16, 3);
    lightning.add_instance_buffer(16, 3);

    tree.add_instance_buffer(9, 7); //attrib size-mat3-12floats, attrib index, for transpose_inverse_viewXmodel

    vector<glm::mat4> ground_model_matrices;
    ground_model_matrices.reserve(GRID_SIZE * GRID_SIZE);

    float ground_mulp = 2.5;

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3((y * SPACING), 0.0f, (x * SPACING)));
            model = glm::scale(model, glm::vec3(ground_mulp, ground_mulp, ground_mulp));
            ground_model_matrices.push_back(model);
        }
    }

    ground.load_instance_buffer((float*)ground_model_matrices.data(), ground_model_matrices.size(), 3, ground_region);

    Light sun = { false, glm::vec3(0.0), glm::vec3(0.0,-1.0,0.0), glm::vec3(0.1,0.1,0.1), glm::vec3(5.0,5.0,5.0), glm::vec3(0.5f,0.5f,0.5f),0,0,0,0,0 };

    shader.use();
    shader.setInt("num_of_lights", 1);
    checkGLError("After changing light amount");

    shader.setBool("lights[0].has_a_source", sun.has_a_source);
    shader.setVec3("lights[0].light_pos", sun.light_pos);
    shader.setVec3("lights[0].light_target", sun.light_target);

    shader.setVec3("lights[0].ambient", sun.ambient);
    shader.setVec3("lights[0].diffuse", sun.diffuse);
    shader.setVec3("lights[0].specular", sun.specular);

    shader.setFloat("lights[0].cos_soft_cut_off_angle", sun.cos_soft_cut_off_angle);
    shader.setFloat("lights[0].cos_hard_cut_off_angle", sun.cos_hard_cut_off_angle);

    shader.setFloat("lights[0].constant", sun.constant);
    shader.setFloat("lights[0].linear", sun.linear);
    shader.setFloat("lights[0].quadratic", sun.quadratic);

    checkGLError("After loading light");


    glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D rendering
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    int x = 0, y = 0, z = 0;
    double time_of_last_frame = 1;
    
    double last_update_frame = 0.0;
    float update_time = 0.10f;
    std::string fps_text = "";
    glfwSetTime(0.0);

    //tree,fire,Burnt
	int draw_counts[3] = { 0,0,0};

    draw_counts[0] = create_load_models(tree, grid_region,100);

	

    while (!glfwWindowShouldClose(window))
    {

        processInput(window, 0.1 * ((glfwGetTime() - time_of_last_frame) / Target_frame_time), camera);
        time_of_last_frame = glfwGetTime();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setVec3("viewPos", camera.camera_position);
        shader.setMatrix4fv("view", glm::value_ptr(camera.get_view_matrix()));
        shader.setMatrix4fv("projection", glm::value_ptr(camera.projection));

		tree.draw(shader, grid_region,draw_counts[0]);

        glDisable(GL_DEPTH_TEST);

        fire_shader.use();
        fire_shader.setMatrix4fv("view", glm::value_ptr(camera.get_view_matrix()));
        fire_shader.setMatrix4fv("projection", glm::value_ptr(camera.projection));

        fire.draw(fire_shader, fire_region, draw_counts[1]);

        if (new_lighting > 0)
            lightning.draw(fire_shader, lightning_region, 1);

        glEnable(GL_DEPTH_TEST);

        ground.draw(fire_shader, grid_region, GRID_SIZE * GRID_SIZE);

        x++;
        if (glfwGetTime() - z >= 1.0f)
        {
            y = x;
            x = 0;
            z = glfwGetTime();
            fps_text = "FPS: " + std::to_string(y);
            printf("Draw calls per second : %d\n", draw_call_count);
            draw_call_count = 0;

        }

        if(glfwGetTime() - last_update_frame >= update_time)
        {
            last_update_frame = glfwGetTime();
            // --- SIMULATION UPDATE ---
			updateSimulation(SIM_STEP,lightning,lightning_region);
			draw_counts[0] = create_load_models(tree, grid_region,100);
            draw_counts[1] = create_load_fire_models(fire, fire_region, 3);
            if (new_lighting > 0)
                new_lighting--;
		}
        printer->render_text(fps_text, -1, 0.9, 2.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    delete printer;

    return 0;
}
