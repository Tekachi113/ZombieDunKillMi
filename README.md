# 🧟 Zombie Don't Kill Me

A single-player top-down pixel-art zombie survival shooter built in **C++** with **SFML**.

Fight off increasingly dangerous waves of zombies, scavenge supplies, trade with a wandering merchant, and survive as long as you can in a post-apocalyptic farm town.

---

## 🎮 Gameplay

- Top-down shooting: move with keyboard, aim/shoot with mouse
- Wave-based survival — each wave gets harder as new zombie types show up
- Collect money and supplies from kills and breakable crates/barrels
- Trade with a **Merchant** NPC for ammo, health kits, weapon upgrades, and fuel
- Drive the map's **car** to escape or reach objectives faster (needs fuel)
- Place a **radio decoy** to lure zombies away from you
- Full inventory interface for managing items and weapons

## 🧟 Zombie Types

| Zombie | Behavior |
|---|---|
| Small | Fast, low HP, rushes the player |
| Medium | Balanced HP/speed/damage — the standard threat |
| Big | High HP, slow, heavy melee damage |
| Turret (legless) | Stationary/slow, spits vomit at range |

## 🛠️ Tech Stack

- **Language:** C++17/20
- **Graphics/Audio:** [SFML 2.6](https://www.sfml-dev.org/)
- **Math:** GLM
- **Serialization:** nlohmann/json (level data, save files)
- **Build system:** CMake
- **Dependency management:** vcpkg / Conan

## 🎨 Assets & Credits

All pixel art — player, zombies, weapons, items, scenery, and inventory UI — comes from the **[Zombie Apocalypse Tileset](https://ittaimanero.itch.io/zombie-apocalypse-tileset)** by **Ittai Manero**.

- Free for personal and commercial use, editable — no redistribution/resale of the raw asset files
- Use the "assets split into separate files" version of the download for easier importing (the original combined sheet has inconsistent spacing)
- Credit to Ittai Manero is appreciated but not required — this project credits them here and in-game

## 🔫 Weapons

| Weapon | Type |
|---|---|
| Knife | Melee |
| Pistol | Ranged, single shot |
| Shotgun | Ranged, spread pellets |
| Grenade | Thrown, area damage |

## 🎒 Items

Money · Ammo (small/medium/big) · Health kits (small/big) · Gas can (refuels the car) · Radio (zombie decoy)

## 📁 Project Structure

```
ZombieDontKillMe/
├── CMakeLists.txt
├── assets/
│   ├── textures/
│   ├── sounds/
│   ├── fonts/
│   └── data/          # zombie stats, wave config, weapon stats (JSON)
├── src/
│   ├── main.cpp
│   ├── core/           # Game, StateManager, ResourceManager, InputManager, AudioManager
│   ├── states/          # MenuState, PlayState, PauseState, GameOverState, UpgradeShopState
│   ├── entities/
│   │   ├── Player
│   │   ├── zombies/        # SmallZombie, MediumZombie, BigZombie, TurretZombie
│   │   ├── weapons/        # Knife, Pistol, Shotgun, Grenade
│   │   ├── world_objects/  # BreakableBox, ExplodingBarrel, Bird, Merchant, Car
│   │   ├── Projectile
│   │   └── pickups/        # Money, Ammo, HealthKit, GasCan, RadioDecoy
│   ├── world/           # TileMap, SpawnManager, CollisionSystem, ParticleSystem, EntityManager
│   ├── ui/              # HUD, Menu
│   └── save/            # SaveSystem
└── tests/
```

## 🚀 Getting Started

### Prerequisites

- CMake >= 3.16
- A C++20-compatible compiler (GCC, Clang, or MSVC)
- SFML 2.6 (installed via package manager or vcpkg)

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
- [ ] Merchant trading & inventory UI
- [ ] Drivable car & fuel system
- [ ] Breakables & exploding barrels
- [ ] Save system & high scores
- [ ] Level editor
- [ ] Co-op mode

## 📄 License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

## 🤝 Contributing

Contributions, issues, and feature requests are welcome. Feel free to check the [issues page](../../issues) or open a pull request.
