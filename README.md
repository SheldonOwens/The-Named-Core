# The-Named-Core
Game development
# The Named

A competitive 5v5 MOBA built in Unreal Engine 5, featuring mythologized historical figures from underrepresented global cultures.

## About

*The Named* is a 5v5 team-based MOBA where players take on mythologized versions of real historical figures from across the globe, emphasizing cultural variety alongside iconic names. The game blends tactical combat, distinct role archetypes, and a stylized visual identity (deep violet and bronze) with a broadcast-style spectator aesthetic.

Beneath the surface, the game carries a slow-burn alien civilization lore layer, invisible at launch and seeded entirely through environmental storytelling, announcer language, and flavor text.

Developed by **The Game Corner**.

## Status

Early foundational development. This repository is a from-scratch C++/UE5 rebuild of the game's core systems, separate from an earlier third-party template-based prototype. The rebuild prioritizes full ownership of core feel, particularly movement, over rapid template-driven iteration.

## Tech Stack

- **Engine:** Unreal Engine 5.7
- **Architecture:** C++ foundation for performance-sensitive and rarely-changing systems (movement, damage pipeline, core attributes), Blueprint for champion kits, VFX, UI, and content iteration
- **Version Control:** Git + Git LFS

## Core Pillars

- **Global Roster** — Historical figures from around the world, emphasizing cultural variety and lesser-known legends alongside iconic names
- **Tactical Combat** — Four active abilities (Q/W/E/R) and one passive per champion
- **Distinct Visual Identity** — Deep violet and bronze aesthetic
- **Hidden Lore Layer** — An alien meta-narrative seeded invisibly through environment and flavor text, revealed slowly post-launch

## Architecture Notes

### Movement System

Movement is built around an interface-driven design so input methods can be swapped or added without touching the hero base class or any champion Blueprint:

- `IMovementIntentProvider` — interface abstracting movement input source from the character
- `UClickToMoveComponent` — current implementation. Builds and follows its own nav-mesh path rather than relying on `AAIController` pathing, so feel (corner smoothing, rotation speed, click responsiveness) is fully exposed as tunable properties
- `ANamedHeroBase` — root character class all champions derive from. Routes movement requests through the active `EMovementInputMode` without needing to know which component is handling them

Direct (WASD) movement is planned as a future addition using the same interface, not a separate system.

### Design Principles Carried Into This Rebuild

- **Template-first discipline**, now self-directed: understand and document what a system does before extending it
- **Risk-ordered implementation**: validate mechanics most likely to be architecturally risky before building around them
- **Tunability without recompilation**: gameplay feel lives in Blueprint-exposed properties on C++ classes, not hardcoded values

## Roadmap

- [ ] Validate click-to-move feel in standalone play
- [ ] Damage pipeline (magic/physical/pure, modifiers)
- [ ] Buff/debuff system
- [ ] Ability system (four ability types: Passive, TargetedLocation, TargetedEnemy, TargetedAlly, Useable)
- [ ] Port first champion (Tomoe Gozen) as proof of concept
- [ ] Direct (WASD) movement mode
- [ ] Lane/creep/jungle systems
- [ ] Shop and item system

## Contact

sheldon.owens@gamecorner.gg
