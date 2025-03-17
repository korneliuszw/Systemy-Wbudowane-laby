## Extracing frames for sending

Generate scaled video with
```bash
ffmpeg -i input.webm -vf scale=20:16 scaled.mp4
```

Then run binarize.py

