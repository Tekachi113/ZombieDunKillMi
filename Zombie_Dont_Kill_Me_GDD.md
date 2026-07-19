# Zombie Don't Kill Me — Game Design Document

**Genre:** Single-player top-down pixel-art zombie survival shooter
**Language:** C++17/20
**Platform target:** PC (Windows/Linux)
**Art direction:** [Zombie Apocalypse Tileset by Ittai Manero](https://ittaimanero.itch.io/zombie-apocalypse-tileset)

---

## 0. Art Asset Pack

This design is built directly around the **Zombie Apocalypse Tileset** (pixel art, top-down, 16×16 tile base). It's free for personal and commercial use, editable, credit appreciated — no redistribution/resale of the raw files. Use the version where the author split every sprite into its own file (linked in the pack's download page); the original single sheet has inconsistent spacing that breaks most auto-slicing tools.

What it provides, and how it maps onto this design:

| Asset pack content | Maps to |
|---|---|
| Post-apocalyptic scenery: buildings, farm, windmill, scarecrow, crops, tractor, gas station, fences, roads, traffic signs, broken cars, bridges, animated water | `TileMap` tiles + static `Prop` entities |
| Drivable car (8 top-down directions) | `Car` entity |
| Breakable boxes, exploding barrel | `BreakableBox`, `ExplodingBarrel` |
| Player character (walk + damage animations) | `Player` |
| 3 regular zombie sizes: small, medium, big (walk + damage animations) | `SmallZombie`, `MediumZombie`, `BigZombie` |
| Legless "turret" zombie (vomit-shooting animation) | `TurretZombie` |
| Animated blood | `ParticleSystem` blood effect |
| 2 birds (fly, land/peck, die-when-shot) | `Bird` ambient/shootable entity |
| Merchant with van | `Merchant` NPC |
| Weapons: knife, pistol, shotgun, grenade (melee/shoot/explosion animations) | `Knife`, `Pistol`, `Shotgun`, `Grenade` weapon classes |
| Items: money, ammo (small/medium/big), health kits (small/big), radio (zombie decoy), gas can, soda can (shootable prop) | `Pickup` subclasses + `RadioDecoy`, `ShootableProp` |
| Inventory interface art | `InventoryUI` |

---

## 1. Technology Stack

| Purpose | Recommended Library | Notes |
|---|---|---|
| Graphics / Windowing | **SFML 3.0** | Simplest 2D API, great for a top-down shooter. Alternative: SDL2 + SDL_image if you want lower-level control |
| Physics / Collision | **Box2D** (optional) or custom AABB/circle collision | Box2D is overkill for pure top-down but great for knockback, ragdolls, destructible props |
| Audio | SFML Audio module (or **FMOD**/**OpenAL** if you want 3D positional sound) | |
| UI / Debug overlay | **Dear ImGui** | For debug menus, tuning values live, level editor tools |
| Math | **GLM** | Vectors, matrices, lerp/easing helpers |
| Serialization | **nlohmann/json** | Save files, level data, config |
| Build system | **CMake** + **vcpkg** or **Conan** for dependency management | |
| Scripting (optional, for level/wave data) | **Lua** via sol2 | Lets you tweak enemy waves without recompiling |

If you want 3D instead of 2D top-down, swap SFML for **raylib** (still simple) or full **OpenGL + GLFW + GLAD** for a custom renderer.

---

## 2. Architecture Overview

Two viable approaches — pick one:

- **Classic OOP** (simpler to start, described below in detail)
- **ECS (Entity Component System)**, e.g. using **EnTT** — better if you plan many enemy types, buffs, and complex interactions later

Below is the **OOP class design**, since it's easier to reason about for a first full game and maps cleanly to a shooter.

```
Game
 ├── StateManager
 │    ├── MenuState
 │    ├── PlayState
 │    ├── PauseState
 │    ├── GameOverState
 │    └── UpgradeShopState
 ├── ResourceManager (textures, sounds, fonts)
 ├── InputManager
 ├── AudioManager
 ├── World / Level
 │    ├── TileMap
 │    ├── SpawnManager
 │    ├── CollisionSystem
 │    ├── ParticleSystem
 │    └── EntityManager
 │         ├── Player
 │         ├── Zombie (base) → subclasses
 │         ├── Projectile (base) → subclasses
 │         ├── Pickup (base) → subclasses
 │         └── Obstacle / Prop
 ├── UI / HUD
 └── SaveSystem
```

---

## 3. Core Class Reference

### 3.1 Engine / Application Layer

```cpp
class Game {
public:
    Game();
    void run();
private:
    void processInput();
    void update(float dt);
    void render();

    sf::RenderWindow window;
    StateManager stateManager;
    ResourceManager resources;
    AudioManager audio;
    float targetFPS = 60.f;
};
```

```cpp
class GameState {
public:
    virtual ~GameState() = default;
    virtual void handleInput(sf::Event&) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow&) = 0;
    virtual void onEnter() {}
    virtual void onExit() {}
};

class StateManager {
public:
    void pushState(std::unique_ptr<GameState>);
    void popState();
    void changeState(std::unique_ptr<GameState>);
    GameState* current();
private:
    std::vector<std::unique_ptr<GameState>> states;
};
```

Concrete states: `MenuState`, `PlayState`, `PauseState`, `GameOverState`, `UpgradeShopState`, `SettingsState`.

```cpp
class ResourceManager {
public:
    sf::Texture& getTexture(const std::string& id);
    sf::SoundBuffer& getSound(const std::string& id);
    sf::Font& getFont(const std::string& id);
    void loadFromManifest(const std::string& jsonPath);
private:
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::SoundBuffer> sounds;
    std::unordered_map<std::string, sf::Font> fonts;
};
```

```cpp
class InputManager {
public:
    void update();
    bool isActionPressed(const std::string& action) const;   // "move_up", "shoot", "reload"
    bool isActionJustPressed(const std::string& action) const;
    sf::Vector2i getMouseWorldPos() const;
private:
    std::unordered_map<std::string, sf::Keyboard::Key> bindings;
};
```

```cpp
class AudioManager {
public:
    void playSound(const std::string& id, float volume = 100.f);
    void playMusic(const std::string& id, bool loop = true);
    void stopMusic();
    void setMasterVolume(float v);
private:
    sf::Music music;
    std::vector<sf::Sound> activeSounds;
};
```

---

### 3.2 Entity Base Class

```cpp
class Entity {
public:
    virtual ~Entity() = default;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow&) = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual bool isAlive() const { return alive; }
    virtual void takeDamage(float amount);
    virtual void onDeath() {}

protected:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float health = 100.f;
    float maxHealth = 100.f;
    bool alive = true;
};
```

`EntityManager` owns all live entities, handles add/remove queues, broad-phase collision partitioning (spatial grid or quadtree recommended once entity counts grow).

```cpp
class EntityManager {
public:
    void addEntity(std::unique_ptr<Entity>);
    void update(float dt);
    void render(sf::RenderWindow&);
    void removeDead();
    std::vector<Entity*> queryRadius(sf::Vector2f center, float radius);
private:
    std::vector<std::unique_ptr<Entity>> entities;
};
```

---

### 3.3 Player

```cpp
class Player : public Entity {
public:
    void update(float dt) override;
    void render(sf::RenderWindow&) override;
    void handleInput(const InputManager&);

    void shoot();
    void reload();
    void switchWeapon(int slot);
    void pickUp(Pickup&);
    void addXP(int amount);
    void levelUp();

    int getAmmo() const;
    int getScore() const;

private:
    sf::Sprite sprite;
    std::vector<std::unique_ptr<Weapon>> inventory;
    Weapon* currentWeapon = nullptr;

    float moveSpeed = 200.f;
    float stamina = 100.f;
    int level = 1;
    int xp = 0;
    int score = 0;

    // Upgradeable stats
    float damageMultiplier = 1.f;
    float fireRateMultiplier = 1.f;
    float armor = 0.f;
};
```

---

### 3.4 Zombies (Enemy Hierarchy)

```cpp
class Zombie : public Entity {
public:
    Zombie(sf::Vector2f spawnPos, float hp, float speed, int dmg, int xpReward);
    void update(float dt) override;
    void render(sf::RenderWindow&) override;

    virtual void chase(const sf::Vector2f& targetPos, float dt);
    virtual void attack(Entity& target);
    void onDeath() override;

protected:
    sf::Sprite sprite;
    float moveSpeed;
    int damage;
    int xpReward;
    float attackCooldown = 1.f;
    float attackTimer = 0.f;
};
```

Subclasses matching the four zombie types in the asset pack:

| Class | Behavior |
|---|---|
| `SmallZombie` | Fast, low HP, rushes player relentlessly |
| `MediumZombie` | Balanced HP/speed/damage — the "standard" zombie |
| `BigZombie` | High HP, slow, heavy melee damage, resists knockback |
| `TurretZombie` | Legless, stationary or very slow, keeps distance and spits vomit at range |

```cpp
class TurretZombie : public Zombie {
public:
    void update(float dt) override;
private:
    void spitAttack(sf::Vector2f targetPos, EntityManager&);
    float preferredDistance = 220.f;
    float spitCooldown = 2.f;
    float spitTimer = 0.f;
};
```

Each animated state (idle/walk/damage/attack/death) corresponds directly to a frame row/column in the pack's per-zombie sprite sheet — see the `Animation` helper in §3.10.

`ZombieFactory` for spawning by type/name (data-driven, reads stats from JSON):

```cpp
class ZombieFactory {
public:
    static std::unique_ptr<Zombie> create(const std::string& type, sf::Vector2f pos);
};
```

---

### 3.5 Weapons & Projectiles

```cpp
class Weapon {
public:
    virtual ~Weapon() = default;
    virtual void fire(sf::Vector2f origin, sf::Vector2f direction, EntityManager&) = 0;
    virtual void reload();
    virtual bool canFire() const;

    std::string name;
    int magazineSize;
    int currentAmmo;
    int reserveAmmo;
    float fireRate;     // shots/sec
    float reloadTime;
    int damage;
    float spread;       // accuracy cone in degrees
protected:
    float fireTimer = 0.f;
};
```

The pack provides exactly four weapons, each with its own melee/shoot/explosion animation — build the class list around them rather than inventing extras:

| Class | Behavior |
|---|---|
| `Knife` | Melee only, no ammo, fast attack, short range arc check instead of a projectile |
| `Pistol` | Single projectile, moderate fire rate, starting weapon |
| `Shotgun` | Multiple pellets/projectiles per shot in a spread cone, short effective range |
| `Grenade` | Thrown, arc trajectory, AoE damage on impact/timer, uses the pack's explosion animation (shared with `ExplodingBarrel`) |

```cpp
class Knife : public Weapon {
public:
    void fire(sf::Vector2f origin, sf::Vector2f direction, EntityManager&) override; // melee swing, no projectile
};

class Grenade : public Weapon {
public:
    void fire(sf::Vector2f origin, sf::Vector2f direction, EntityManager&) override; // spawns a ThrownGrenade entity
};

class ThrownGrenade : public Entity {
public:
    void update(float dt) override;   // arc motion, fuse timer
    void explode(EntityManager&);     // AoE damage + shared explosion animation
private:
    float fuseTime = 2.f;
};
```

```cpp
class Projectile : public Entity {
public:
    Projectile(sf::Vector2f pos, sf::Vector2f dir, float speed, int damage, Entity* owner);
    void update(float dt) override;
    void render(sf::RenderWindow&) override;
    void onHit(Entity& target);
private:
    sf::Vector2f direction;
    float speed;
    int damage;
    Entity* owner;
    float lifetime = 3.f;
    bool piercing = false;
};
```

---

### 3.6 Pickups & Items

```cpp
class Pickup : public Entity {
public:
    virtual void applyEffect(Player&) = 0;
    void update(float dt) override;
    void render(sf::RenderWindow&) override;
};

// Exact item set available in the asset pack
class MoneyPickup   : public Pickup { void applyEffect(Player&) override; };            // currency for the Merchant
class SmallAmmoPickup  : public Pickup { void applyEffect(Player&) override; };
class MediumAmmoPickup : public Pickup { void applyEffect(Player&) override; };
class BigAmmoPickup    : public Pickup { void applyEffect(Player&) override; };
class SmallHealthKit : public Pickup { void applyEffect(Player&) override; };
class BigHealthKit   : public Pickup { void applyEffect(Player&) override; };
class GasCanPickup   : public Pickup { void applyEffect(Player&) override; };            // refills Car fuel
class RadioDecoy     : public Pickup {
public:
    void applyEffect(Player&) override;   // player places it in the world instead of consuming it
    void placeInWorld(sf::Vector2f pos, EntityManager&);
private:
    float attractRadius = 300.f;   // draws nearby zombies toward its position
    float duration = 15.f;
};
```

### World Interaction Objects (also from the pack)

```cpp
class BreakableBox : public Entity {
public:
    void takeDamage(float amount) override;   // breaks after N hits, spawns a random Pickup
    void onDeath() override;
};

class ExplodingBarrel : public Entity {
public:
    void takeDamage(float amount) override;   // detonates when destroyed or shot
    void onDeath() override;                  // shares the Grenade's explosion animation, AoE damage
};

class ShootableProp : public Entity {          // e.g. the soda can, has a "shot" reaction animation
public:
    void takeDamage(float amount) override;
};

class Bird : public Entity {                   // ambient wildlife: fly / land+peck / die-when-shot
public:
    enum class State { Flying, Landed, Pecking, Dying };
    void update(float dt) override;
    void takeDamage(float amount) override;    // optional: shootable for flavor, no gameplay effect
private:
    State state = State::Flying;
};

class Merchant : public Entity {               // NPC with a van, sells items/upgrades for Money
public:
    void interact(Player&);
    std::vector<ShopItem> getInventory() const;
private:
    std::vector<ShopItem> stock;
};

struct ShopItem {
    std::string id;
    std::string displayName;
    int price;
    std::function<void(Player&)> applyPurchase;
};

class Car : public Entity {                    // drivable, 8 top-down directions
public:
    void update(float dt) override;
    void enter(Player&);
    void exit();
    bool hasFuel() const;
    void refuel(float amount);
private:
    bool occupied = false;
    float fuel = 100.f;
    float fuelConsumptionRate = 1.f;
    float topSpeed = 350.f;
};
```

---

### 3.7 World / Level Systems

```cpp
class TileMap {
public:
    void loadFromFile(const std::string& path);
    void render(sf::RenderWindow&, const sf::View& camera);
    bool isWalkable(sf::Vector2f worldPos) const;
private:
    std::vector<std::vector<int>> tileGrid;
    sf::Texture tileset;
};
```

```cpp
class SpawnManager {
public:
    void update(float dt, EntityManager& entities, const Player& player);
    void startWave(int waveNumber);
    bool isWaveComplete() const;
    int getCurrentWave() const;
private:
    int currentWave = 0;
    float spawnTimer = 0.f;
    std::vector<std::string> waveComposition; // zombie types this wave
    int zombiesRemainingToSpawn = 0;
};
```

```cpp
class CollisionSystem {
public:
    void resolve(EntityManager& entities, const TileMap& map);
private:
    bool checkCircleCollision(sf::Vector2f a, float ra, sf::Vector2f b, float rb);
};
```

```cpp
class ParticleSystem {
public:
    void emit(sf::Vector2f pos, const std::string& effectType, int count);
    void update(float dt);
    void render(sf::RenderWindow&);
private:
    struct Particle { sf::Vector2f pos, vel; float life; sf::Color color; };
    std::vector<Particle> particles;
};
```

---

### 3.8 UI / HUD

```cpp
class HUD {
public:
    void update(const Player& player, int waveNumber);
    void render(sf::RenderWindow&);
private:
    sf::Text healthText, ammoText, scoreText, waveText;
    sf::RectangleShape healthBar;
};
```

```cpp
class Menu {
public:
    void addButton(const std::string& label, std::function<void()> onClick);
    void handleInput(sf::Event&);
    void render(sf::RenderWindow&);
private:
    struct Button { sf::Text text; sf::RectangleShape box; std::function<void()> callback; };
    std::vector<Button> buttons;
};
```

```cpp
class InventoryUI {
public:
    void toggle();
    void update(const Player& player);
    void render(sf::RenderWindow&);
    void handleInput(sf::Event&);   // drag/select items, use consumables, switch weapons
private:
    bool visible = false;
    std::vector<sf::Sprite> itemIcons;   // uses the pack's inventory interface art directly
};
```

---

### 3.10 Animation Helper

The pack's animated sprites (player, zombies, birds, weapons, explosions) come as per-entity sprite sheets with fixed frame sizes once split. A single reusable helper drives all of them:

```cpp
class Animation {
public:
    void load(const sf::Texture& texture, sf::Vector2i frameSize, int frameCount, float frameDuration, bool loop = true);
    void update(float dt);
    void reset();
    bool isFinished() const;
    sf::IntRect getCurrentFrameRect() const;
private:
    sf::Vector2i frameSize;
    int frameCount = 1;
    int currentFrame = 0;
    float frameDuration = 0.1f;
    float timer = 0.f;
    bool looping = true;
    bool finished = false;
};

class AnimatedSprite : public sf::Sprite {
public:
    void addAnimation(const std::string& name, Animation anim);
    void play(const std::string& name, bool restart = false);
    void update(float dt);   // advances current animation, updates texture rect
private:
    std::unordered_map<std::string, Animation> animations;
    std::string currentAnimation;
};
```

`Player`, `Zombie` and its subclasses, `Bird`, and `ThrownGrenade`/`ExplodingBarrel` all compose an `AnimatedSprite` instead of a raw `sf::Sprite`, and call `play("walk")`, `play("damage")`, `play("death")`, etc. as their state changes.

---

### 3.9 Persistence

```cpp
class SaveSystem {
public:
    void saveGame(const std::string& slot, const PlayerSaveData& data);
    PlayerSaveData loadGame(const std::string& slot);
    std::vector<int> getHighScores();
private:
    std::string savePath = "saves/";
};

struct PlayerSaveData {
    int highScore;
    int totalZombiesKilled;
    int unlockedWeapons;
    float playtimeSeconds;
};
```

---

## 4. Core Gameplay Loop

1. `MenuState` → player starts run
2. `PlayState::onEnter()` loads level, spawns player at start position
3. Each frame: `InputManager` → `Player::handleInput` → `EntityManager::update` (player, zombies, projectiles) → `CollisionSystem::resolve` → `SpawnManager::update` (waves) → `HUD::update` → render
4. Wave clears → brief pause → `UpgradeShopState` (spend XP/currency on weapon/stat upgrades) → next wave, increasing difficulty (more zombies, tougher types, faster spawn rate)
5. Player death → `GameOverState` → show score, save high score, return to menu

---

## 5. Suggested Folder Structure

```
ZombieDontKillMe/
├── CMakeLists.txt
├── assets/
│   ├── textures/
│   ├── sounds/
│   ├── fonts/
│   └── data/          (JSON: zombie stats, wave config, weapon stats)
├── src/
│   ├── main.cpp
│   ├── core/           (Game, StateManager, ResourceManager, InputManager, AudioManager, Animation, AnimatedSprite)
│   ├── states/          (MenuState, PlayState, PauseState, GameOverState, UpgradeShopState)
│   ├── entities/
│   │   ├── Entity.h/.cpp
│   │   ├── Player.h/.cpp
│   │   ├── zombies/     (Zombie.h/.cpp, SmallZombie, MediumZombie, BigZombie, TurretZombie)
│   │   ├── weapons/     (Weapon.h/.cpp, Knife, Pistol, Shotgun, Grenade, ThrownGrenade)
│   │   ├── world_objects/ (BreakableBox, ExplodingBarrel, ShootableProp, Bird, Merchant, Car)
│   │   ├── Projectile.h/.cpp
│   │   └── pickups/     (MoneyPickup, SmallAmmoPickup, MediumAmmoPickup, BigAmmoPickup, SmallHealthKit, BigHealthKit, GasCanPickup, RadioDecoy)
│   ├── world/           (TileMap, SpawnManager, CollisionSystem, ParticleSystem, EntityManager)
│   ├── ui/              (HUD, Menu, InventoryUI)
│   └── save/            (SaveSystem)
└── tests/
```

---

## 6. Progression & Balancing Systems

- **XP & Leveling:** kills grant XP → level up grants stat point or perk choice
- **Currency:** separate in-run currency for the `UpgradeShopState` between waves (weapon upgrades, armor, speed)
- **Difficulty scaling:** each wave increases zombie count, HP multiplier, and unlocks tougher types at wave thresholds (e.g., `TurretZombie` at wave 3, `BigZombie` swarms at wave 5+)
- **Merchant shop:** instead of an abstract shop screen, the player walks up to the `Merchant`'s van on the map and spends `Money` pickups on ammo, health kits, weapon upgrades, and gas
- **Perk system (optional depth):** e.g., "Life Steal," "Faster Reload," "Extra Pellets" (shotgun), implemented as modifiers applied to `Player` stats or `Weapon` behavior via strategy pattern

---

## 7. Stretch Goals / Extensions

- Swap OOP inheritance for **EnTT ECS** if enemy variety grows large — better cache performance with hundreds of zombies on screen
- Add a simple **A\* pathfinding** module for zombies navigating obstacles (`PathfindingGrid` class)
- Use the `Car` for a "reach the extraction point before fuel runs out" mode, using `GasCanPickup` for risk/reward runs into zombie-heavy areas
- Day/night cycle affecting zombie spawn rate and visibility
- Local co-op (second `Player` instance, split logic already supports it since Player is just an Entity)
- Level editor using Dear ImGui + your `TileMap` save/load

---

## 8. Minimal CMakeLists.txt Starting Point

```cmake
cmake_minimum_required(VERSION 3.16)
project(ZombieDontKillMe)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3.0 COMPONENTS graphics window system audio REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system sfml-audio)
```

---

This gives you a full skeleton — engine layer, state machine, entity hierarchy (player/zombies/weapons/projectiles/pickups/world objects), world systems, UI, and persistence — built directly around the exact content of the Zombie Apocalypse Tileset, so every class maps to art you already have and nothing needs to be reinvented or re-skinned later.
