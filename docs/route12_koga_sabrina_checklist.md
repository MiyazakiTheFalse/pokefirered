# Route 12 Koga/Sabrina Asset Wiring Checklist

Use this quick checklist during PR review to catch missing asset wiring before merge.

- [ ] **Map created and registered** (layout/header/map groups and connections are defined).
- [ ] **Warps connected in both directions** (forward + return warp targets are valid).
- [ ] **Event flags declared** (all new progression/clear flags added to constants).
- [ ] **Trainers and parties wired** (trainer entries reference valid party data and scripts).
- [ ] **Dialogue strings added** (all referenced text labels exist and are linked).
- [ ] **Pre-/post-battle scripts linked** (intro, battle, and aftermath callbacks are connected).
- [ ] **Route blockers removed after clear** (blocking objects/scripts update once completion flag is set).
