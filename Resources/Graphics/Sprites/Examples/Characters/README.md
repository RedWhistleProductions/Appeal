# Character sprite sheets

Each character keeps its sprite sheets and accompanying metadata, licenses, and source archives in a folder matching the character name:

`Resources/Graphics/Sprites/Examples/Characters/<Character_Name>/`

For example, Adam uses `Adam/Adam_Movement.png` and Eve uses `Eve/Eve_Movement.png`. Graphics animation paths are relative to the sprite folder, such as `Examples/Characters/Adam/Adam_Movement.png`.

Keep additional animation sheets for a character in the same folder. Read each sheet's metadata for its frame layout; layouts can differ between sheets and orientations.

Use the Red Whistle Productions naming convention for sprite sheets and matching metadata: capitalize the first letter of each word and separate words with underscores, for example `Eve_Actions_Top_Down.png` and `Eve_Actions_Top_Down.json`. Keep file extensions lowercase.
