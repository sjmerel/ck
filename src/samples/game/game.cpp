#include "game.h"
#include "gl.h"
#include "cube.h"
#include "groundplane.h"
#include "ship.h"
#include "timer.h"
#include "ck/ck.h"
#include "ck/config.h"
#include "ck/sound.h"
#include "ck/bank.h"
#include "ck/mixer.h"
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>

#if CK_PLATFORM_ANDROID
#  include <android/log.h>
#endif

////////////////////////////////////////

// window dimensions
int g_width = 0;
int g_height = 0;

// game objects
const int kNumCubes = 5;
Cube* g_cubes[kNumCubes] = { 0 };
GroundPlane* g_ground = NULL;
Ship* g_ship = NULL;

Vector3 g_playerPos(0.0f, 0.01f, 0.0f);
float g_playerPosChange = 0.0f;
float g_playerPosChangeTarget = 0.0f;
float g_playerAngle = M_PI;
float g_playerAngleChange = 0.0f;
float g_playerAngleChangeTarget = 0.0f;

// touch start location
float g_touchStartX = -1.0f;
float g_touchStartY = -1.0f;

Timer g_timer;

bool g_inited = false;

// music
CkSound* g_music = NULL;
CkSound* g_pointSound = NULL;
CkSound* g_engineSound = NULL;
CkBank* g_bank = NULL;

////////////////////////////////////////

float randomFloat(float min, float max)
{
    return (float) rand() / RAND_MAX * (max - min) + min;
}

void placeCube(Cube* cube)
{
    const float kMax = 20.0f;

    Vector3 pos;
    pos.x = randomFloat(-kMax, kMax);
    pos.y = 0.501f;
    pos.z = randomFloat(-kMax, kMax);

    cube->setPosition(pos);
}

float calcChange(float x, float xMax, float xStart, float prevChange, float changeScale, float changeMax)
{
    const float kDeadZone = 0.03f;

    float dx = (x - xStart) / xMax;
    float change = prevChange;
    if (fabs(dx) >= kDeadZone)
    {
        if (dx > 0.0f)
        {
            dx -= kDeadZone;
        }
        else
        {
            dx += kDeadZone;
        }

        change = -dx * changeScale;

        if (change > changeMax)
        {
            change = changeMax;
        }
        if (change < -changeMax)
        {
            change = -changeMax;
        }
    }

    return change;
}

const float kPosChangeScale = 25.0f;
const float kPosChangeMax = 7.0f;

const float kAngleChangeScale = 3.0f;
const float kAngleChangeMax = 0.7f;

void handleMove(float x, float y)
{
    g_playerPosChangeTarget = calcChange(x, g_width, g_touchStartX, g_playerPosChangeTarget, kPosChangeScale, kPosChangeMax);
    g_playerAngleChangeTarget = calcChange(-y, g_height, -g_touchStartY, g_playerAngleChangeTarget, kAngleChangeScale, kAngleChangeMax);
}

////////////////////////////////////////

extern "C"
{

#ifdef CK_PLATFORM_ANDROID
void gameInit(JNIEnv* env, jobject activity)
#else
void gameInit()
#endif
{
    if (!g_inited)
    {
        Timer::init();
        g_timer.start();

#if CK_PLATFORM_ANDROID
        CkConfig config(env, activity);
#else
        CkConfig config;
#endif
        CkInit(&config);

        // mute the volume until the first update; 
        // the volumes & pans of 3D sounds won't be correct until then.
        CkMixer::getMaster()->setVolume(0.0f);

        // sound
        g_music = CkSound::newStreamSound("music.cks");
        g_bank = CkBank::newBank("game.ckb");

        g_pointSound = CkSound::newBankSound(g_bank, "point");

        g_engineSound = CkSound::newBankSound(g_bank, "engine");
        g_engineSound->setLoopCount(-1);
        g_engineSound->setPaused(true);
        g_engineSound->play();

        g_music->setLoopCount(-1);
        g_music->setVolume(0.2f);
        g_music->play();

        g_pointSound->setVolume(0.6f);

        CkSound::set3dAttenuation(kCkAttenuationMode_InvDistanceSquared, 0.5f, 10.0f, 0.05f);


        // game objects
        for (int i = 0; i < kNumCubes; ++i)
        {
            // cube sits just above ground plane
            Cube* cube = new Cube();
            placeCube(cube);

            g_cubes[i] = cube;
        }

        g_ground = new GroundPlane();

        g_ship = new Ship();

        // gameGlInit() is called when the drawing surface is created.

        g_inited = true;
    }
}

void gameShutdown()
{
    if (g_inited)
    {
        gameGlShutdown(); // TODO call this when context still active

        // game objects
        delete g_ground;
        g_ground = NULL;
        delete g_ship;
        g_ship = NULL;
        for (int i = 0; i < kNumCubes; ++i)
        {
            delete g_cubes[i];
            g_cubes[i] = NULL;
        }

        // sound
        g_music->destroy();
        g_pointSound->destroy();
        g_engineSound->destroy();
        g_bank->destroy();

        CkShutdown();

        g_inited = false;
    }
}

void gameSuspend()
{
    if (g_inited)
    {
        CkSuspend();
        g_timer.stop();
    }
}

void gameResume()
{
    if (g_inited)
    {
        CkResume();
        g_timer.start();
    }
}

// This is called when the GL drawing surface is created.
// (On iOS, this happens when the app is launched; on Android, it also
// happens when bringing an app out of the background.)
void gameGlInit()
{
    gameGlShutdown();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    g_ground->initGraphics();
}

// This is called when the GL drawing surface is destroyed.
// (On iOS, this happens when the app exits; on Android, it also
// happens when sending an app to the background.)
void gameGlShutdown()
{
    g_ground->destroyGraphics();
}

void gameResize(int w, int h)
{
    g_width = w;
    g_height = h;
}

void gameTouch(GameTouchType type, float x, float y)
{
    switch (type)
    {
        case GameTouchDown:
            g_touchStartX = x;
            g_touchStartY = y;
            break;

        case GameTouchMove:
            handleMove(x, y);
            break;

        case GameTouchUp:
            g_playerPosChangeTarget = 0.0f;
            g_playerAngleChangeTarget = 0.0f;
            break;
    }
}

void gameRender()
{
    // update simulation
    float dt = g_timer.getElapsedMs() * 0.001f;
    g_timer.reset();

    // filter motion so you slow down gradually when you lift your finger off the screen
    const float kChangeFactor = 0.1f;
    g_playerPosChange = g_playerPosChangeTarget * kChangeFactor + g_playerPosChange * (1.0f - kChangeFactor);
    g_playerAngleChange = g_playerAngleChangeTarget * kChangeFactor + g_playerAngleChange * (1.0f - kChangeFactor);

    // update engine sound
    g_engineSound->setPaused(g_playerPosChange == 0.0f);
    float normSpeed = fabs(g_playerPosChange / kPosChangeMax);
    g_engineSound->setPitchShift(normSpeed*8);
    g_engineSound->setVolume(normSpeed * 0.4f);

    // update player position
    Vector3 forwardDir(sinf(g_playerAngle), 0.0f, cosf(g_playerAngle));
    Vector3 dr;
    Vector3::multiply(forwardDir, g_playerPosChange*dt, dr);
    Vector3::add(g_playerPos, dr, g_playerPos);
    g_playerAngle += g_playerAngleChange * dt;

    // keep player angle in [0..2pi]
    if (g_playerAngle > 2.0f*M_PI)
    {
        g_playerAngle -= 2.0f*M_PI;
    }
    else if (g_playerAngle < 0.0f)
    {
        g_playerAngle += 2.0f*M_PI;
    }

    // test for hits
    for (int i = 0; i < kNumCubes; ++i)
    {
        if (g_cubes[i]->hitTest(g_playerPos))
        {
            g_pointSound->play();
            placeCube(g_cubes[i]);
        }
    }

    // render
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float fov = 0.9f;
    float near = 0.1f;
    float far = 100.0f;
    float aspect = (float) g_width / g_height;

    float top = tanf(fov*0.5) * near;
    float bottom = -top;
    float left = aspect * bottom;
    float right = aspect * top;

    glFrustumf(left, right, bottom, top, near, far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // for landscape orientation
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);

    Vector3 eye;
    Vector3 v;
    Vector3::multiply(forwardDir, -3.0f, v);
    Vector3::add(g_playerPos, v, eye);
    eye.y = 1.0f;

    Vector3::multiply(forwardDir, 1.0f, v);
    Vector3 center;
    Vector3::add(g_playerPos, v, center);

    gluLookAt(eye.x, eye.y, eye.z,
              center.x, center.y, center.z,
              0.0f, 1.0f, 0.0f);

    CkSound::set3dListenerPosition(
            eye.x, eye.y, eye.z,
            center.x, center.y, center.z,
            0.0f, 1.0f, 0.0f);

    g_ship->setTransform(g_playerPos, g_playerAngle * 180.0f / M_PI + 180.0f);
    g_ship->draw();

    for (int i = 0; i < kNumCubes; ++i)
    {
        g_cubes[i]->draw(dt);
    }

    g_ground->draw();

    CkMixer::getMaster()->setVolume(1.0f);

    CkUpdate();
}

void gamePrintf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
#if CK_PLATFORM_ANDROID
    __android_log_vprint(ANDROID_LOG_INFO, "game", format, args);
#else
    vprintf(format, args);
#endif
    va_end(args);
}

} // extern "C"
