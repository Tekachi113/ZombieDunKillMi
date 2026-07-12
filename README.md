# 🧟 Zombie Don't Kill Me

A single-player top-down zombie survival shooter built in **C++** with **SFML**.

Fight off increasingly dangerous waves of zombies, upgrade your weapons and stats between rounds, and survive as long as you can.

---

## 🎮 Gameplay

- Top-down twin-stick style shooting: move with keyboard, aim/shoot with mouse
- Wave-based survival — each wave gets harder, with new zombie types unlocking as you progress
- Earn XP and currency from kills to level up and upgrade your loadout between waves
- Multiple weapons: pistol, shotgun, assault rifle, sniper rifle, grenades, melee
- Pick-ups: health, ammo, armor, weapons, XP orbs

## 🧟 Zombie Types

| Zombie | Behavior |
|---|---|
| Walker | Standard slow melee attacker |
| Runner | Fast, low HP, rushes the player |
| Tank | High HP, slow, heavy melee damage |
| Spitter | Ranged acid attack, keeps distance |
| Crawler | Low profile, hard to hit |
| Exploder | Rushes in and explodes on death/contact |
| Boss | Multi-phase fight with unique attack patterns |

## 🛠️ Tech Stack

- **Language:** C++17/20
- **Graphics/Audio:** [SFML 2.6](https://www.sfml-dev.org/)
- **Math:** GLM
- **Serialization:** nlohmann/json (level data, save files)
- **Build system:** CMake
- **Dependency management:** vcpkg / Conan

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
│   │   ├── zombies/
│   │   ├── weapons/
│   │   ├── Projectile
│   │   └── pickups/
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
- [ ] Zombie AI & wave spawning
- [ ] Weapon system & pickups
- [ ] Upgrade shop between waves
- [ ] Boss encounters
- [ ] Save system & high scores
- [ ] Level editor
- [ ] Co-op mode

## 📄 License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

## 🤝 Contributing

Contributions, issues, and feature requests are welcome. Feel free to check the [issues page](../../issues) or open a pull request.
