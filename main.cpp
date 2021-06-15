#define OLC_PGE_APPLICATION

#include "olcPixelGameEngine.h"
#include <condition_variable>

using namespace std;

int nMaxThreads = static_cast<int>(std::thread::hardware_concurrency());
static std::atomic<int> nWorkerComplete = 0;


class Example : public olc::PixelGameEngine {
    float playerX = 1.5f;
    float playerY = 15.5f;
    float playerAngle = 0.0f;
    const int mapHeight = 64;
    const int mapWidth = 64;
    wstring map;
    const float fieldOfView = 3.14 / 4.0;
    const float halfOfFieldOfView = fieldOfView / 2.0f;
    const float depth = 24.0f;
    float deltaAngle{};
    int screenHeight;
    int screenWidth;
public:

    Example() {
        map += L"################################################################";
        map += L"#...##################################.........................#";
        map += L"#...##################################.........................#";
        map += L"#...##################################.........................#";
        map += L"#...##################################.........................#";
        map += L"#...##################################.........................#";
        map += L"#...##################################.........................#";
        map += L"#...##################################.........................#";
        map += L"#...##################################.........................#";
        map += L"#...##################################.........................#";
        map += L"#...##################################.........................#";
        map += L"#..............................................................#";
        map += L"#...........................####################...............#";
        map += L"#...........................####################...............#";
        map += L"#...........................####################...............#";
        map += L"#...........................####################...............#";
        map += L"#..............................................................#";
        map += L"#................................#####...#######################";
        map += L"#....#####################.......#####.........................#";
        map += L"#....#####################.......#####.........................#";
        map += L"#....#####################.......#########################.....#";
        map += L"#....#####################.......#########################.....#";
        map += L"#....#####################.......#####.........................#";
        map += L"#................................#####.........................#";
        map += L"#................................#####.........................#";
        map += L"#................................#####.........................#";
        map += L"#................................#####.........................#";
        map += L"#................................#####.........................#";
        map += L"#................................#####.........................#";
        map += L"#................................#####.........................#";
        map += L"#................................#####.........................#";
        map += L"#................................#####.........................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"#..............................................................#";
        map += L"################################################################";
        sAppName = "RayCasting";
    }


    struct WorkerThread {
        olc::PixelGameEngine *myEngine{};
        int32_t myHeight{};
        int myWorkSize{};
        float myPA{};
        float myHOFoV{};
        float myDA{};
        float myD{};
        float myPx{};
        float mypy{};
        int myMw{};


        int myMh{};
        const wstring *myM{};
        int myItem{};
        std::condition_variable cvStart{};
        bool alive = true;
        bool ready = false;
        std::mutex mux{};

        std::thread thread;

        void StartRenderingPart(olc::PixelGameEngine *engine, int32_t height, int workSize, float pA,
                                float hOFoV, float dA, float d, float px, float py,
                                int mw,
                                int mh, const wstring *m, int item) {
            myEngine = engine;
            myHeight = height;
            myWorkSize = workSize;
            myPA = pA;
            myHOFoV = hOFoV;
            myDA = dA;
            myD = d;
            myPx = px;
            mypy = py;
            myMw = mw;
            myMh = mh;
            myM = m;
            myItem = item;
            {
                std::unique_lock<std::mutex> lm(mux);
                ready = true;
            }
            cvStart.notify_one();
        }


        void RenderPartOfScreen() const {
            float distanceToWall;
            for (int x = myItem; x < myItem + myWorkSize; x++) {
                float rayAngle = myPA - myHOFoV + myDA * static_cast<float>(x);
                float unitVectorX = sinf(rayAngle);
                float unitVectorY = cosf(rayAngle);
                bool hitWall = false;
                bool hitBoundary = false;
                distanceToWall = 0.0f;
                int currentRayX;
                int currentRayY;

                while (!hitWall && distanceToWall < myD) {
                    distanceToWall += 0.01f;
                    float exactX = myPx + distanceToWall * unitVectorX;
                    currentRayX = (int) (exactX);
                    float exactY = mypy + distanceToWall * unitVectorY;
                    currentRayY = (int) (exactY);
                    if (currentRayX < 0 || currentRayY < 0 || currentRayX >= myMw || currentRayY >= myMh) {
                        hitWall = true;
                        distanceToWall = myD;
                    } else if ((*myM)[myMw * currentRayY + currentRayX] == '#') {
                        hitWall = true;
                        int nearestX = (int) (round(exactX));
                        int nearestY = (int) (round(exactY));
                        float deltaX = abs(static_cast<float>(nearestX) - exactX);
                        float deltaY = abs(static_cast<float>(nearestY) - exactY);
                        if (sqrt(deltaX * deltaX + deltaY * deltaY) < 0.01) hitBoundary = true;
                    }
                }

                int color;
                if (distanceToWall > myD) color = 0.0f;
                else color = static_cast<int>(192.0f * (1.0f - distanceToWall / myD) + 0.0f * (distanceToWall / myD));
                int ceiling = max(0, (int) (static_cast<float>(myHeight) / 2.0f -
                                            static_cast<float>(myHeight) /
                                            (distanceToWall * cosf(myPA - rayAngle))));
                int floor = myHeight - ceiling;
                for (int32_t y = 0; y < myHeight; y++) {
                    if (y < ceiling) {
//                        mypixels[y * width + x] = olc::BLUE;
                        myEngine->Draw(x, y, olc::BLUE);
                        continue;
                    }
                    if (y > floor) {
                        float floorShadeCoefficient = 2.0f * (1 - static_cast<float>(y) / static_cast<float>(myHeight));
                        int greenShadeFloorColor = (int) (0.0f * floorShadeCoefficient +
                                                          (1 - floorShadeCoefficient) * 150.0f);
//                        mypixels[y * width + x] = olc::Pixel(0, greenShadeFloorColor, 0);
                        myEngine->Draw(x, y, olc::Pixel(0, greenShadeFloorColor, 0));
                        continue;
                    }
//                    mypixels[y * width + x] = hitBoundary ? olc::BLACK : olc::Pixel(color, color, color, 255);
                    myEngine->Draw(x, y, hitBoundary ? olc::BLACK : olc::Pixel(color, color, color, 255));
                }
            }

            nWorkerComplete++;
        }

        void RenderPartOfRayCastingWorld() {
            while (alive) {
                std::unique_lock<std::mutex> lm(mux);
                bool *ready_ptr = &ready;
                cvStart.wait(lm, [ready_ptr] { return *ready_ptr; });
                RenderPartOfScreen();
                ready = false;
            }
        }
    };

    WorkerThread *workers;
    int workersNumber{};

    void InitialiseThreadPool() {
        workersNumber = static_cast<int>(std::thread::hardware_concurrency());
        workers = new WorkerThread[workersNumber];
        for (int i = 0; i < workersNumber; i++) {
            workers[i].thread = std::thread(&WorkerThread::RenderPartOfRayCastingWorld, &(workers[i]));
        }
    }


    bool OnUserDestroy() override {
        for (int i = 0; i < workersNumber; i++) {
            workers[i].alive = false;
            workers[i].cvStart.notify_one();
        }

        for (int i = 0; i < workersNumber; i++) {
            workers[i].thread.join();
        }

        delete workers;
        return true;
    }

    bool OnUserCreate() override {
        deltaAngle = fieldOfView / static_cast<float>(ScreenWidth());
        InitialiseThreadPool();
        return true;
    }

    bool multicoreMode = true;

    bool OnUserUpdate(float fElapsedTime) override {
        UpdateScreenResolution();
        ProcessInput(fElapsedTime);
        RenderWorld();
        return true;
    }

    void RenderWorld() {
        if (multicoreMode) RenderWorldInParallel();
        else RenderOnSingleThread();
        RenderPlayerMap();
    }

    void UpdateScreenResolution() {
        screenWidth = ScreenWidth();
        screenHeight = ScreenHeight();
    }

    void ProcessInput(float fElapsedTime) {
        if (GetKey(olc::Q).bHeld) playerAngle -= fElapsedTime * 3.14f / 2.0f;

        if (GetKey(olc::E).bHeld) playerAngle += fElapsedTime * 3.14f / 2.0f;

        if (GetKey(olc::S).bHeld) {
            float newPlayerX = playerX - fElapsedTime * sinf(playerAngle) * 5.0f;
            float newPlayerY = playerY - fElapsedTime * cosf(playerAngle) * 5.0f;
            setNewPlayerPosition(newPlayerX, newPlayerY);
        }
        if (GetKey(olc::W).bHeld) {
            float newPlayerX = playerX + fElapsedTime * sinf(playerAngle) * 5.0f;
            float newPlayerY = playerY + fElapsedTime * cosf(playerAngle) * 5.0f;
            setNewPlayerPosition(newPlayerX, newPlayerY);
        }

        if (GetKey(olc::A).bHeld) {
            float newPlayerX = playerX - fElapsedTime * cosf(playerAngle) * 5.0f;
            float newPlayerY = playerY + fElapsedTime * sinf(playerAngle) * 5.0f;
            setNewPlayerPosition(newPlayerX, newPlayerY);
        }

        if (GetKey(olc::D).bHeld) {
            float newPlayerX = playerX + fElapsedTime * cosf(playerAngle) * 5.0f;
            float newPlayerY = playerY - fElapsedTime * sinf(playerAngle) * 5.0f;
            setNewPlayerPosition(newPlayerX, newPlayerY);
        }

        if (GetKey(olc::O).bPressed) {
            multicoreMode = !multicoreMode;
        }
    }

    void RenderPlayerMap() {
        for (int nx = 0; nx < mapWidth; nx++)
            for (int ny = 0; ny < mapHeight; ny++) {
                olc::Pixel color;
                color = map[ny * mapWidth + nx] == '#' ? olc::BLACK : olc::CYAN;
                Draw(nx, ny, color);
            }
        Draw((int) playerX, (int) playerY, olc::RED);
    }

    void RenderOnSingleThread() {
        float distanceToWall;
        for (int x = 0; x < screenWidth; x++) {
            float rayAngle = playerAngle - halfOfFieldOfView + deltaAngle * static_cast<float>(x);
            float unitVectorX = sinf(rayAngle);
            float unitVectorY = cosf(rayAngle);
            bool hitWall = false;
            bool hitBoundary = false;
            distanceToWall = 0.0f;
            int currentRayX;
            int currentRayY;

            while (!hitWall && distanceToWall < depth) {
                distanceToWall += 0.01f;
                float exactX = playerX + distanceToWall * unitVectorX;
                currentRayX = static_cast<int>(exactX);
                float exactY = playerY + distanceToWall * unitVectorY;
                currentRayY = static_cast<int>(exactY);
                if (currentRayX < 0 || currentRayY < 0 || currentRayX >= mapWidth || currentRayY >= mapHeight) {
                    hitWall = true;
                    distanceToWall = depth;
                } else if (map[mapWidth * currentRayY + currentRayX] == '#') {
                    hitWall = true;
                    int nearestX = static_cast<int>(round(exactX));
                    int nearestY = static_cast<int>(round(exactY));
                    float deltaX = abs(static_cast<float>(nearestX) - exactX);
                    float deltaY = abs(static_cast<float>(nearestY) - exactY);
                    if (sqrt(deltaX * deltaX + deltaY * deltaY) < 0.01) hitBoundary = true;
                }
            }

            int color;
            if (distanceToWall > depth) color = 0.0f;
            else color = static_cast<int>(192.0f * (1.0f - distanceToWall / depth) + 0.0f * (distanceToWall / depth));
            int ceiling = max(0, static_cast<int>(static_cast<float>(screenHeight) / 2.0f -
                                                  static_cast<float>(screenHeight) /
                                                  (distanceToWall * cosf(playerAngle - rayAngle))));
            int floor = screenHeight - ceiling;
            for (int32_t y = 0; y < screenHeight; y++) {
                if (y < ceiling) {
                    Draw(x, y, olc::BLUE);
                    continue;
                }
                if (y > floor) {
                    float floorShadeCoefficient = 2.0f * (1 - static_cast<float>(y) / static_cast<float>(screenHeight));
                    int greenShadeFloorColor = static_cast<int>(0.0f * floorShadeCoefficient +
                                                                (1 - floorShadeCoefficient) * 150.0f);
                    Draw(x, y, olc::Pixel(0, greenShadeFloorColor, 0));
                    continue;
                }
                Draw(x, y, hitBoundary ? olc::BLACK : olc::Pixel(color, color, color, 255));
            }
        }
    }

    void RenderWorldInParallel() {
        int workSize = screenWidth / nMaxThreads;
        float pA = playerAngle;
        float hOFoV = halfOfFieldOfView;
        float dA = deltaAngle;
        float d = depth;
        float px = playerX;
        float py = playerY;
        int mw = mapWidth;
        int mh = mapHeight;
        wstring *m = &map;

        nWorkerComplete = 0;

        for (int i = 0; i < nMaxThreads; i++)
            workers[i].StartRenderingPart(this, screenHeight, workSize, pA, hOFoV, dA, d, px, py, mw, mh, m,
                                          i * workSize);

        while (nWorkerComplete < nMaxThreads) // Wait for all workers to complete
        {}
    }

    void setNewPlayerPosition(float newPlayerX, float newPlayerY) {
        if (!isNewPositionInBoundaries(newPlayerX, newPlayerY)) return;
        playerX = newPlayerX;
        playerY = newPlayerY;
    }

    bool isNewPositionInBoundaries(float newPlayerX, float newPlayerY) {
        return newPlayerX - 0.11 >= 0 &&
               newPlayerY - 0.11 >= 0 &&
               newPlayerX + 0.11 < static_cast<float>(mapWidth) &&
               newPlayerY + 0.11 < static_cast<float>(mapHeight) &&
               map[static_cast<int>(newPlayerY) * mapWidth + static_cast<int>(newPlayerX)] != '#';
    }
};


int main() {
    Example demo;
    if (demo.Construct(3840, 2160, 1, 1))
        demo.Start();

    return 0;
}