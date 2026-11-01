# Mood Ring Face (Pebble)

An 8-bit mood ring watchface. It reads your live heart rate, estimates stress from BPM, and shifts both the background color and a PixelLab character’s expression — from relaxed blue to anxious red.

## Moods

| Mood | Default BPM | Color |
| --- | --- | --- |
| Relaxed | ≤ 65 | Deep blue |
| Calm | ≤ 85 | Cerulean |
| Alert | ≤ 105 | Chrome yellow |
| Anxious | > 105 | Candy-apple red |

Thresholds are adjustable in Settings. Watches without a heart-rate sensor show **No HR** (or enable **Demo mode** to cycle moods).

## Settings (phone)

Config uses **Clay** (`@rebble/clay`). Open **Mood Ring Face → Settings** in the Pebble/Rebble app:

- Hour format / leading zero / date
- Show BPM + mood label
- Demo mode
- BPM thresholds for each mood band

## Build

On Windows (Podman or Docker):

```powershell
./build.ps1
```

Output: `build/MoodRingFace.pbw`

```powershell
./build.ps1 -Phone 192.168.1.23
./build.ps1 -Zip
```

## CloudPebble

Import `dist/Mood-Ring-Face-cloudpebble.zip` into CloudPebble (or rebuild it with
`python build-env/make_zip.py`).

## Layout

```
src/c/              watchface, HR → mood mapping
src/pkjs/           Clay settings
resources/images/   PixelLab face sprites + menu icon
```
