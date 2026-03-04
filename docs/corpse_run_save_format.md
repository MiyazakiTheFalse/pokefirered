# Corpse-run save format

`SaveBlock1.corpseRun` is a fixed-width 400-byte payload with a versioned header.

## Current schema (`version = 1`)

- `magic` (`'CRSV'`) and `version` identify the schema.
- `payloadLength` and `payloadChecksum` protect the rest of the schema bytes (`state` through `partySnapshot[]`).
- Death metadata is persisted as map group/num/coords/elevation.
- Stored party payload is persisted as a compact 6-slot snapshot for **State 1 recovery only**.
- Stored Souls are persisted in `droppedSouls`.
- Corpse marker metadata is persisted as spawn flag, map identity, coords, elevation, style, map section, and spawn counter.
- `state` is persisted as `CR_NORMAL`, `CR_ACTIVE_1`, or `CR_SALVAGE`.

## Authoritative transitions

1. `CR_NORMAL -> CR_ACTIVE_1` (first party wipe)
   - Snapshot party into stored snapshot payload.
   - Snapshot Souls into `droppedSouls`.
   - Spawn corpse marker at death location.
   - Respawn uses the whiteout pipeline warp.
   - Corpse-run mode becomes active.
2. `CR_ACTIVE_1 -> CR_NORMAL` (corpse retrieved before second death)
   - Stored Souls are recovered (`droppedSouls` cleared).
   - Corpse marker is removed.
   - Temporary corpse-run snapshot payload is cleared.
3. `CR_ACTIVE_1 -> CR_SALVAGE` (second death before corpse retrieval)
   - Stored party snapshot is permanently invalidated/cleared.
   - Stored Souls are permanently deleted.
   - Corpse marker is removed.
   - Run enters emergency salvage rebuild mode.
4. `CR_SALVAGE -> CR_NORMAL` (rebuild complete)
   - While in salvage with no usable Pokémon, only emergency Safari-style capture/escape flow is allowed.
   - On first successful capture of a usable Pokémon, salvage exits and normal gameplay resumes.

**Explicit wording:** Second death during State 1 (before corpse retrieval) triggers permanent loss of stored Souls + stored party and enters salvage rebuild mode.

## Validation and safe recovery

On load, corpse-run data is considered invalid unless all checks pass:

1. `magic` and `version` match expected constants.
2. `payloadLength` matches the expected v1 payload size.
3. `payloadChecksum` matches the checksum of the payload bytes.
4. `state` enum is in-range.
5. Party count and marker metadata are sane.

If validation fails, data is reset to a safe baseline (`CR_NORMAL`, marker despawned, dropped souls cleared).

## Save/quit behavior during active state 1

When a player saves/quits while `state == CR_ACTIVE_1`:

- Active state is preserved as-is.
- Marker location and stored Souls remain persisted.
- On continue, player can recover by touching the marker.
- If the player whites out again before retrieval, transition is `CR_ACTIVE_1 -> CR_SALVAGE` with permanent loss.
