# Pankcake

A top-down capture-the-flag shooter written in C++. You grab flags that spawn in different parts of the map and fight off waves of AI enemies while you do it. A built-in level editor lets you draw your own maps and play them.

Led a team of total four at DigiPen Institute of Technology Singapore as an academic project.

<!-- TODO: add a GIF or gameplay video here. A stranger decides in about 20 seconds, and a moving picture does most of that work. -->

## Features

- **Three ways to play:** a tutorial range, the main level, and custom levels you build yourself.
- **Flag capture:** flags spawn in different sections of the map, and an on-screen pointer shows you where the current one is.
- **Enemy AI:** enemies spawn in timed waves, use line of sight to find you and shoot back with the same weapon system the player uses.
- **Weapons:** several fire types, including a shotgun with a calculated spread.
- **Movement:** WASD movement plus a dash that can kill enemies on contact.
- **Effects:** particle trails, dash silhouettes, death explosions and damage numbers.
- **Level editor:** a tile palette with hotkeys, a free camera you pan with the middle mouse button, a grey preview box showing where the next tile goes, and save, load and clear.
- **Leak check:** when you save a map, a flood-fill check makes sure it has no holes and warns you if it finds one.
- **Data-driven tuning:** gameplay and UI constants load from text files, so balance changes don't need a recompile.
- **Menus:** main menu, pause menu, settings with volume control, a controls page, a confirmation screen before you quit or restart, and credits.


The full controls are also listed in the in-game Controls screen and the pause menu.

## Download and play


1. Download the repository as a ZIP (green **Code** button → **Download ZIP**) and extract it.
2. Open the release folder and run the game's `.exe`.
3. If Windows Defender warns that the app is unrecognised, click **More info**, then **Run anyway**.

## Building from source

1. Open the solution file in Visual Studio.
2. Select the `Release` / `x64` configuration.
3. Build and run.

## Tech stack

- **Language:** C++
- **Framework:** 
- **Platform:** Windows

## How some of it works

**Bullets.** Bullets started out as manually allocated objects created by a dedicated create function, which made ownership explicit and avoided leaks. They were later moved into a `std::vector`, which handles growth and cleanup automatically and keeps bullet data contiguous in memory, so updating and drawing them each frame is a single linear pass. Each frame, every bullet is tested for collision against the player and against enemies.

**State machine.** The game moves between states (menus, tutorial, main level, custom level, editor, end screen). The state machine stores both the previous and the current state. That lets "back" and "resume" return to the right screen, and it guarantees each level is fully unloaded before the next one is loaded.

**Event system.** Gameplay events such as flag pickup and the win condition are raised as events, and the score system listens to them. The rules for scoring and winning live in one place instead of being scattered through the gameplay code.

**Map format.** Custom maps are saved as text files. The first lines store the map height, width and tile size, followed by the tile grid, so maps of different sizes load without code changes.

**Leak check.** Before a custom map is saved, a flood fill starts from one cell and spreads up, down, left and right through every connected cell of the same type. If the fill reaches the edge of the map, the map has a hole and the player is warned.

## Team

| Member | Main areas |
|---|---|
| Yeo Kah Boon Joshua | Player systems: movement and dash, bullet and weapon fire types, particles, HUD, state machine, event and score system, sound system, level loading |
| Ye Min Thwin | Weapon system, enemy types and AI, enemy wave spawning, settings menu, damage numbers |
| Zhang Yida | Map drawing, camera, block collision, flag spawning, editor free camera, flood-fill leak check, tutorial map |
| Pang Jun Yuan | Pause and pre-game menus, editor save/load and tile palette, controls and credits screens, UI design |

## Credits

Full credits are on the Credits screen in the game's main menu.

## License

No license has been applied. This project was made for academic purposes as part of DigiPen's curriculum. All content © DigiPen Institute of Technology Singapore.
