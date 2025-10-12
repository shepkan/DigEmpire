# Repository Guidelines

## Project Structure & Module Organization
- Root: `DigEmpire.uproject`, `DigEmpire.sln`.
- Code: `Source/DigEmpire` (modules: `Character/`, `Map/`, `Map/Generation/`, `Map/Zones/`). Targets in `Source/DigEmpire.Target.cs` and `Source/DigEmpireEditor.Target.cs`.
- Config: `Config/Default*.ini`.
- Assets: `Content/` (e.g., `Map/`, `Character/`, `Sprites/`).
- Plugins: `Plugins/GameplayMessageRouter`.
- Suggested tests: add under `Source/DigEmpire/Tests/` if not present.

## Build, Test, and Development Commands
- Open in Editor: `UnrealEditor.exe DigEmpire.uproject`.
- C++ build (Win64): `Engine/Build/BatchFiles/Build.bat DigEmpireEditor Win64 Development -Project="<path>\\DigEmpire.uproject" -WaitMutex`.
- Package quick run: `Engine/Build/BatchFiles/RunUAT.bat BuildCookRun -project=DigEmpire.uproject -platform=Win64 -build -cook -stage -pak -archive -archivedirectory=Build`.
- Automation tests (headless): `UnrealEditor-Cmd.exe DigEmpire.uproject -unattended -nop4 -NullRHI -run=Automation -Test=Project`.

## Coding Style & Naming Conventions
- Indentation: 4 spaces; follow Unreal C++ style.
- Classes: `U/A/F/I` prefixes; booleans with `b` (e.g., `bIsVisible`). File names match class names.
- Functions/Types: PascalCase; local variables camelCase; constants `G`/`C` per UE norms.
- Use `UPROPERTY`/`UFUNCTION` where reflection/GC or Blueprints are needed.
- Asset naming: `BP_` Blueprints, `M_` Materials, `MI_` Material Instances, `T_` Textures, `L_` Levels, `DA_` Data Assets, `WBP_` UI Widgets. Example: `Content/Map/DA_CaveTextureSet.uasset`.

## Testing Guidelines
- Prefer Unreal Automation Tests (C++) or Functional Testing for gameplay flows.
- Place C++ specs in `Source/DigEmpire/Tests` using `IMPLEMENT_SIMPLE_AUTOMATION_TEST` and register under the `Project` category.
- Run via the command above; keep tests deterministic (e.g., seed procedural gen).

## Commit & Pull Request Guidelines
- Commits: imperative mood, concise subject; optional types: `feat:`, `fix:`, `refactor:`, `test:`, `build:`.
- PRs: clear description, linked issues, repro steps, before/after screenshots or short video for visual changes, and passing build/tests.
- Do not commit generated or local files: `Binaries/`, `Intermediate/`, `DerivedDataCache/`, `Saved/`, `*.pdb`.

## Agent Notes
- Don’t edit generated files; prefer targeted changes in `Source/DigEmpire/`.
- When adding modules, update `Source/DigEmpire/DigEmpire.Build.cs` and relevant `*.Target.cs`.
- Keep plugin edits isolated and justified; prefer project-side extensions.
