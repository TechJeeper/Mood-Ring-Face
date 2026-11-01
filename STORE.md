# Rebble Appstore listing — Mood Ring Face

Copy these fields into [dev-portal.rebble.io](https://dev-portal.rebble.io/submit).

| Field | Value |
| --- | --- |
| App name | Mood Ring Face |
| Type | Watchface |
| Category | Faces *(automatic)* |
| Website | *(optional)* |
| Source code | https://github.com/TechJeeper/Mood-Ring-Face |
| `.pbw` | `build/MoodRingFace.pbw` |

## Description

Paste into each platform asset collection (max 1600 characters):

```
Mood Ring Face turns your wrist into a living mood ring.

It reads your live heart rate and estimates stress from BPM, then shifts both the background color and an 8-bit character’s expression — from relaxed deep blue to anxious candy-apple red.

Pick your character in settings: Mia, Kai, Nova, Rex, or animal friends Cat, Dog, Fox, and Owl. Each has four PixelLab expressions that track your mood.

• 8 selectable faces (people + animals)
• Relaxed · Calm · Alert · Anxious expressions
• Background color tracks your current mood band
• Digital time with optional date
• Live BPM readout (or “No HR” when unavailable)
• Adjustable BPM thresholds in phone settings
• Demo mode cycles moods on watches without a heart-rate sensor
• Clay configuration for character, time, labels, and thresholds

Works best on HR-capable Pebbles (Pebble 2 HR, Pebble Time 2). Color watches show the full blue→red mood spectrum; B&W watches keep the expressions and labels on a black face.
```

*(~920 characters)*

## Release notes (v1.1.0)

```
Selectable characters: Mia, Kai, Nova, Rex, Cat, Dog, Fox, Owl — each with four mood expressions.
```

## Screenshots

Exact platform sizes (no watch bezel). Regenerate with:

```powershell
python tools/generate_store_screenshots.py
```

| Platform | Size | Folder |
| --- | --- | --- |
| basalt (Pebble Time) | 144×168 | `store/screenshots/basalt/` |
| diorite (Pebble 2) | 144×168 | `store/screenshots/diorite/` |
| flint (Pebble 2 Duo) | 144×168 | `store/screenshots/flint/` |
| emery (Pebble Time 2) | 200×228 | `store/screenshots/emery/` |

Five shots per platform showcasing face variants + moods:

1. Mia · Relaxed  
2. Kai · Calm (+date)  
3. Nova · Alert  
4. Cat · Anxious  
5. Fox · Calm

## Listing checklist

- [ ] Screenshots uploaded per platform (up to 5)
- [ ] Description above on each asset collection
- [ ] Source URL set to the GitHub repo
- [ ] Publish `build/MoodRingFace.pbw` as release 1.0.0
