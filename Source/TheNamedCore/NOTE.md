# Setup Note

These files assume a module named `TheNamedCore` (matching the repo name) and
use the placeholder API macro `THENAMED_API`.

Once the actual .uproject/Build.cs exists:
1. Confirm the real module name in Build.cs
2. Find-and-replace `THENAMED_API` with `<YOURMODULE>_API` if it differs from `THENAMEDCORE_API`
3. Confirm `NavigationSystem` and `AIModule` are listed in PublicDependencyModuleNames in Build.cs
   (required for ClickToMoveComponent's nav-mesh path queries)
4. Move/merge this Source/TheNamedCore folder into the project's actual Source/<ModuleName> folder
