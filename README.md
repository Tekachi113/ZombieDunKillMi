# 🧟 Zombie Don't Kill Me

A single-player top-down pixel-art zombie survival shooter built in **C++** with **SFML 3.0**.

Fight off increasingly dangerous waves of zombies, scavenge supplies, and survive as long as you can in a post-apocalyptic farm town.

---

## 🎮 Core Gameplay

- Top-down shooting: move with keyboard, aim/shoot with mouse
- Wave-based survival — each wave gets harder as new zombie types show up
- Collect money and supplies from kills and breakable crates/barrels
- Use money to buy upgrades on weapons or player's base stats between waves

## 🧟 Zombie Types

| Zombie | Behavior |
|---|---|
| Small | Fast, low HP, rushes the player |
| Medium | Balanced HP/speed/damage — the standard threat |
| Big | High HP, slow, heavy melee damage |
| Turret (legless) | Stationary/slow, spits vomit at range |

## 🔫 Weapons

| Weapon | Type |
|---|---|
| Knife | Melee |
| Pistol | Ranged, single shot |
| Shotgun | Ranged, spread pellets |
| Grenade | Thrown, area damage |

## 🎒 Items

Money · Ammo · Health kits

## 🌍 World Objects

- Breakable boxes (drop loot)
- Exploding barrels (AoE damage)

## 🛠️ Tech Stack

- **Language:** C++17/20
- **Graphics:** SFML 3.0
- **Math:** GLM
- **Serialization:** nlohmann/json (level data, save files)
- **Build system:** CMake
- **Dependency management:** vcpkg

## 🎨 Assets & Credits

All pixel art comes from the **[Zombie Apocalypse Tileset](https://ittaimanero.itch.io/zombie-apocalypse-tileset)** by **Ittai Manero**.

## 📁 Project Structure

```
ZombieDontKillMe/
├── CMakeLists.txt
├── assets/
│   ├── textures/
│   ├── sounds/
│   ├── fonts/
│   └── data/              # zombie stats, wave config, weapon stats (JSON)
├── src/
│   ├── main.cpp
│   ├── core/              # Game, StateManager, ResourceManager, InputManager
│   ├── states/            # MenuState, PlayState, PauseState, GameOverState, UpgradeShopState
│   ├── entities/
│   │   ├── Entity
│   │   ├── Player
│   │   ├── Projectile
│   │   ├── zombies/       # Zombie, SmallZombie, MediumZombie, BigZombie, TurretZombie
│   │   ├── weapons/       # Weapon, Knife, Pistol, Shotgun, Grenade
│   │   ├── world_objects/ # BreakableBox, ExplodingBarrel
│   │   └── pickups/       # Pickup, MoneyPickup, AmmoPickup, HealthKitPickup
│   ├── world/             # TileMap, SpawnManager, CollisionSystem, ParticleSystem, EntityManager
│   ├── ui/                # HUD, Menu
│   └── save/              # SaveSystem
└── tests/
```

## 🚀 Getting Started

### Prerequisites

- CMake >= 3.16
- A C++20-compatible compiler (GCC, Clang, or MSVC)
- SFML 3.0 (installed via vcpkg)

### Build

```bash
git clone https://github.com/<your-username>/ZombieDontKillMe.git
cd ZombieDontKillMe

mkdir build && cd build
cmake ..
cmake --build .
```

### Run

```bash
./ZombieDontKillMe
```

## 🎯 Controls

| Action | Key |
|---|---|
| Move | W A S D |
| Aim / Shoot | Mouse / Left Click |
| Reload | R |
| Switch Weapon | 1-4 |
| Pause | ESC |

## 🗺️ Roadmap

- [ ] Core movement & shooting
- [ ] Zombie AI & wave spawning (small/medium/big/turret)
- [ ] Weapon system & pickups
- [ ] Upgrade shop between waves
- [ ] Breakables & exploding barrels
- [ ] Save system & high scores

## 📄 License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.
