# Corpse-run battle-type reward/action matrix

While `corpseRun.state == CR_ACTIVE_1`, battle permissions and payouts are resolved by centralized checks (not per-script toggles).

| Capability | State 1 (`CR_ACTIVE_1`) policy | Salvage (`CR_SALVAGE`) policy | Normal (`CR_NORMAL`) |
| --- | --- | --- | --- |
| Bag availability in battle menu | Blocked | Blocked | Allowed |
| Run/flee availability | Allowed | Allowed (`Run` remains available for emergency Safari flow) | Allowed |
| EXP payout | Blocked | Blocked | Allowed |
| Souls/currency payout | Blocked | Blocked | Allowed |
| Capture permission (ball throws) | Blocked | Allowed (emergency rebuild capture flow) | Allowed |
| Post-battle trainer scripts | Blocked | Blocked | Allowed |

## Integration points

- Action menu handling and action acceptance (`battle_controller_player.c`, `battle_main.c`).
- EXP script dispatch (`battle_util.c`).
- Prize money / Pay Day grants and capture throw gate (`battle_script_commands.c`).
- Trainer post-battle script return dispatch (`battle_setup.c`).
- Safari Zone step-limit countdown bypass while active state 1 (`safari_zone.c`).

## Safari Zone behavior during corpse-run mode

When corpse-run state 1 is active, Safari step countdown is paused so Safari-linked encounters do not force `Times Up` during the run.

Vanilla step countdown resumes automatically as soon as corpse-run exits active state 1, because the pause check is keyed directly to `CorpseRun_IsActive()`.
