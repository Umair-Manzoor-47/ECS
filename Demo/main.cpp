#include <raylib.h>
#include <World.h>
#include <Components.h>
#include <cstdlib>
#include <cmath>
#include <chrono>

// ─────────────────────────────────────────────
// Config
// ─────────────────────────────────────────────

static const int SCREEN_W       = 1280;
static const int SCREEN_H       = 720;
static const int STATS_H        = 110;
static const int RENDER_H       = SCREEN_H - STATS_H;

static const int WANDERER_COUNT = 150;
static const int PULSER_COUNT   = 80;
static const int ORBITER_COUNT  = 120;
static const int FADER_COUNT    = 100;

// ─────────────────────────────────────────────
// Timing
// ─────────────────────────────────────────────

using Clock     = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

float ElapsedUs(TimePoint& start) {
    auto now  = Clock::now();
    float us  = std::chrono::duration<float, std::micro>(now - start).count();
    start     = now;
    return us;
}

// ─────────────────────────────────────────────
// Stats
// ─────────────────────────────────────────────

struct Stats {
    float fps              = 0;
    float frameTimeMs      = 0;
    int   entityCount      = 0;
    int   archetypeCount   = 0;

    // per system (microseconds)
    float wandererUs       = 0;
    float pulserUs         = 0;
    float orbiterUs        = 0;
    float faderUs          = 0;
    float renderUs         = 0;
    float totalQueryUs     = 0;
};

float Smooth(float current, float target, float factor = 0.1f) {
    return current + (target - current) * factor;
}

// ─────────────────────────────────────────────
// Stats panel
// ─────────────────────────────────────────────

void DrawStatsPanel(const Stats& s) {
    DrawRectangle(0, RENDER_H, SCREEN_W, STATS_H, { 12, 12, 22, 255 });
    DrawLine(0, RENDER_H, SCREEN_W, RENDER_H, { 70, 70, 110, 255 });

    const int row1Y = RENDER_H + 10;
    const int row2Y = RENDER_H + 42;
    const int row3Y = RENDER_H + 74;
    const int f1    = 18;
    const int f2    = 15;

    // ── Row 1: overview ──────────────────────
    Color fpsColor = s.fps >= 55 ? GREEN : (s.fps >= 30 ? YELLOW : RED);
    DrawText(TextFormat("FPS: %.0f", s.fps),
             20, row1Y, f1, fpsColor);

    DrawText(TextFormat("Frame: %.2f ms", s.frameTimeMs),
             160, row1Y, f1, RAYWHITE);

    DrawText(TextFormat("Entities: %d", s.entityCount),
             360, row1Y, f1, { 180, 220, 255, 255 });

    DrawText(TextFormat("Archetypes: %d", s.archetypeCount),
             560, row1Y, f1, { 180, 220, 255, 255 });

    DrawText(TextFormat("Total Query: %.1f us", s.totalQueryUs),
             760, row1Y, f1, { 255, 200, 100, 255 });

    // ── Row 2: system timings ─────────────────
    DrawText("Systems:", 20, row2Y, f2, { 130, 130, 155, 255 });

    DrawText(TextFormat("Wanderers %.1f us", s.wandererUs),
             115, row2Y, f2, { 100, 220, 160, 255 });

    DrawText(TextFormat("Pulsers %.1f us", s.pulserUs),
             320, row2Y, f2, { 180, 120, 255, 255 });

    DrawText(TextFormat("Orbiters %.1f us", s.orbiterUs),
             510, row2Y, f2, { 100, 190, 255, 255 });

    DrawText(TextFormat("Faders %.1f us", s.faderUs),
             710, row2Y, f2, { 255, 160, 100, 255 });

    DrawText(TextFormat("Render Query %.1f us", s.renderUs),
             890, row2Y, f2, { 220, 220, 100, 255 });

    // ── Row 3: architecture label ─────────────
    DrawText(TextFormat("Wanderers: %d  |  Pulsers: %d  |  Orbiters: %d  |  Faders: %d",
             WANDERER_COUNT, PULSER_COUNT, ORBITER_COUNT, FADER_COUNT),
             20, row3Y, f2, { 90, 90, 120, 255 });

    // DrawText("Archetype ECS  |  Packed Arrays  |  Zero Virtual Calls  |  Cache Friendly",
    //          600, row3Y, f2, { 90, 90, 120, 255 });
}

// ─────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────

float RandFloat(float min, float max) {
    return min + (float)(rand() % 10000) / 10000.0f * (max - min);
}

int RandInt(int min, int max) {
    return min + rand() % (max - min);
}

// ─────────────────────────────────────────────
// Main
// ─────────────────────────────────────────────

int main() {
    InitWindow(SCREEN_W, SCREEN_H, "ECS Demo — 4 Archetypes");
    SetTargetFPS(60);

    ecs::World world;

    // ── Archetype 1: Wanderers ────────────────
    // [Transform, Velocity, Sprite]
    // bounce off walls, warm otter tones
    for (int i = 0; i < WANDERER_COUNT; ++i) {
        auto e    = world.CreateEntity();
        float spd = RandFloat(1.5f, 4.5f);
        float ang = RandFloat(0.0f, 6.28f);
        world.AddComponents(e,
            ecs::Transform{ RandFloat(0, SCREEN_W), RandFloat(0, RENDER_H) },
            ecs::Velocity{ cosf(ang) * spd, sinf(ang) * spd },
            ecs::Sprite{ RandFloat(4, 9),
                         RandInt(140, 220), RandInt(80, 140), RandInt(30, 80) }
        );
    }

    // ── Archetype 2: Pulsers ──────────────────
    // [Transform, Sprite, Pulse]
    // stationary, breathe in and out, cool purple tones
    for (int i = 0; i < PULSER_COUNT; ++i) {
        auto e = world.CreateEntity();
        world.AddComponents(e,
            ecs::Transform{ RandFloat(40, SCREEN_W - 40), RandFloat(40, RENDER_H - 40) },
            ecs::Sprite{ 6.0f,
                         RandInt(140, 200), RandInt(60, 120), RandInt(200, 255) },
            ecs::Pulse{ 0.0f, RandFloat(0.8f, 2.5f), RandFloat(3, 6), RandFloat(10, 18) }
        );
    }

    // ── Archetype 3: Orbiters ─────────────────
    // [Transform, Velocity, Sprite, Orbit]
    // circle around a fixed center point, blue tones
    for (int i = 0; i < ORBITER_COUNT; ++i) {
        auto e       = world.CreateEntity();
        float cx     = RandFloat(100, SCREEN_W - 100);
        float cy     = RandFloat(100, RENDER_H - 100);
        float radius = RandFloat(30, 120);
        float angle  = RandFloat(0.0f, 6.28f);
        float spd    = RandFloat(0.3f, 1.5f) * (rand() % 2 == 0 ? 1 : -1);
        world.AddComponents(e,
            ecs::Transform{ cx + cosf(angle) * radius, cy + sinf(angle) * radius },
            ecs::Velocity{ 0, 0 },
            ecs::Sprite{ RandFloat(3, 7),
                         RandInt(60, 120), RandInt(160, 220), RandInt(200, 255) },
            ecs::Orbit{ cx, cy, radius, angle, spd }
        );
    }

    // ── Archetype 4: Faders ───────────────────
    // [Transform, Velocity, Sprite, Fade]
    // drift slowly, pulse alpha, warm orange tones
    for (int i = 0; i < FADER_COUNT; ++i) {
        auto e    = world.CreateEntity();
        float spd = RandFloat(0.3f, 1.5f);
        float ang = RandFloat(0.0f, 6.28f);
        world.AddComponents(e,
            ecs::Transform{ RandFloat(0, SCREEN_W), RandFloat(0, RENDER_H) },
            ecs::Velocity{ cosf(ang) * spd, sinf(ang) * spd },
            ecs::Sprite{ RandFloat(5, 12),
                         RandInt(220, 255), RandInt(120, 180), RandInt(30, 80) },
            ecs::Fade{ RandFloat(0, 6.28f), RandFloat(0.5f, 2.0f), 255 }
        );
    }

    Stats stats;
    stats.entityCount = WANDERER_COUNT + PULSER_COUNT + ORBITER_COUNT + FADER_COUNT;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        stats.fps         = Smooth(stats.fps,         (float)GetFPS(), 0.15f);
        stats.frameTimeMs = Smooth(stats.frameTimeMs, dt * 1000.0f,    0.15f);
        stats.archetypeCount = (int)world.GetArchetypeCount();

        // ── Wanderer system ───────────────────
        TimePoint t0 = Clock::now();
        world.Query<ecs::Transform, ecs::Velocity>(
            [dt](ecs::Transform& t, ecs::Velocity& v) {
                t.x += v.vx * dt * 60.0f;
                t.y += v.vy * dt * 60.0f;
            });
        float rawWanderer = ElapsedUs(t0);

        // bounce (only wanderers and faders have Velocity but not Orbit)
        // we use a separate bounce query — it only matches archetypes with all three
        world.Query<ecs::Transform, ecs::Velocity, ecs::Sprite>(
            [](ecs::Transform& t, ecs::Velocity& v, ecs::Sprite& s) {
                if (t.x - s.radius < 0)        { t.x = s.radius;             v.vx =  fabsf(v.vx); }
                if (t.x + s.radius > SCREEN_W) { t.x = SCREEN_W - s.radius; v.vx = -fabsf(v.vx); }
                if (t.y - s.radius < 0)        { t.y = s.radius;             v.vy =  fabsf(v.vy); }
                if (t.y + s.radius > RENDER_H) { t.y = RENDER_H - s.radius; v.vy = -fabsf(v.vy); }
            });
        rawWanderer += ElapsedUs(t0);
        stats.wandererUs = Smooth(stats.wandererUs, rawWanderer, 0.1f);

        // ── Pulser system ─────────────────────
        t0 = Clock::now();
        world.Query<ecs::Sprite, ecs::Pulse>(
            [dt](ecs::Sprite& s, ecs::Pulse& p) {
                p.timer += dt * p.speed;
                float t  = (sinf(p.timer) + 1.0f) * 0.5f; // 0..1
                s.radius = p.minRadius + (p.maxRadius - p.minRadius) * t;
            });
        stats.pulserUs = Smooth(stats.pulserUs, ElapsedUs(t0), 0.1f);

        // ── Orbiter system ────────────────────
        t0 = Clock::now();
        world.Query<ecs::Transform, ecs::Orbit>(
            [dt](ecs::Transform& t, ecs::Orbit& o) {
                o.angle += o.speed * dt;
                t.x      = o.centerX + cosf(o.angle) * o.radius;
                t.y      = o.centerY + sinf(o.angle) * o.radius;
            });
        stats.orbiterUs = Smooth(stats.orbiterUs, ElapsedUs(t0), 0.1f);

        // ── Fader system ──────────────────────
        t0 = Clock::now();
        world.Query<ecs::Sprite, ecs::Fade>(
            [dt](ecs::Sprite& s, ecs::Fade& f) {
                f.timer  += dt * f.speed;
                float a   = (sinf(f.timer) + 1.0f) * 0.5f; // 0..1
                f.alpha   = (int)(40 + a * 215);
            });
        stats.faderUs = Smooth(stats.faderUs, ElapsedUs(t0), 0.1f);

        // ── Render ────────────────────────────
        BeginDrawing();
        ClearBackground({ 18, 18, 28, 255 });

        TimePoint t1 = Clock::now();

        // render faders (use alpha from Fade component)
        world.Query<ecs::Transform, ecs::Sprite, ecs::Fade>(
            [](ecs::Transform& t, ecs::Sprite& s, ecs::Fade& f) {
                DrawCircle((int)t.x, (int)t.y, s.radius + 3,
                    { (unsigned char)(s.r / 4),
                      (unsigned char)(s.g / 4),
                      (unsigned char)(s.b / 4),
                      (unsigned char)(f.alpha / 3) });
                DrawCircle((int)t.x, (int)t.y, s.radius,
                    { (unsigned char)s.r,
                      (unsigned char)s.g,
                      (unsigned char)s.b,
                      (unsigned char)f.alpha });
            });

        // render everything else (wanderers, pulsers, orbiters)
        // Faders also have Sprite so we render all Sprite entities here,
        // faders will draw twice but with correct alpha from their own query above
        world.Query<ecs::Transform, ecs::Sprite>(
            [](ecs::Transform& t, ecs::Sprite& s) {
                DrawCircle((int)t.x, (int)t.y, s.radius + 2,
                    { (unsigned char)(s.r / 3),
                      (unsigned char)(s.g / 3),
                      (unsigned char)(s.b / 3), 80 });
                DrawCircle((int)t.x, (int)t.y, s.radius,
                    { (unsigned char)s.r,
                      (unsigned char)s.g,
                      (unsigned char)s.b, 255 });
                DrawCircle((int)(t.x - s.radius * 0.25f),
                           (int)(t.y - s.radius * 0.25f),
                           s.radius * 0.3f,
                    { 255, 255, 255, 50 });
            });

        stats.renderUs     = Smooth(stats.renderUs, ElapsedUs(t1), 0.1f);
        stats.totalQueryUs = Smooth(stats.totalQueryUs,
            stats.wandererUs + stats.pulserUs + stats.orbiterUs +
            stats.faderUs + stats.renderUs, 0.1f);

        DrawStatsPanel(stats);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}