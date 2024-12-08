# 3.0.0
## Chatbox
Chatboxes now have their own folder, independent from themes. Their starting location is `base/chatbox`. The standard chatbox is `default`.
A set of standard text colors are expected to be available. The following colors are standard: `Default` (white), `Blue`, `Green`, `Orange`, `Yellow`, `Red`.

* Removed stickers from chatboxes.
## Effects
The folder in which effects are expected to be found have been changed to `base/effects` from `base/themes/default/effects`.

The standard effects have been moved to the `default` folder within `base/effects`.

* Added `caption` parameter for pretty naming.
* Effects no longer fallback to default effects if a character's data points to a specific effect pack that does not exist.
## Blips
* Deprecated legacy variants. (`sfx-<identifier>`)
## Themes/Interface
* Chatboxes are no longer tied to themes.
  * Text colors can still be adjusted per-theme to ensure proper readability.
* Effects are no longer tied to themes.
* Removed various UI elements that either had no use or are exclusive to a server/client fork, thus not needed here.
  * `char_passworded`
  * `char_password`
* Updated player list formatting options.
  * Added `name` represents the player's name.
  * Added `character_name` represents the character's custom name.
  * Removed `username`.
  * Removed `displayname`.
* Removed `muted` interface element. (Was functionally deprecated.)
## Demos
Demos have been removed for the time being. A new replay system will take its place.
## Internal
* Reworked character select logic.
