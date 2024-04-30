# WebVTT subtitles support

## Consuming a WebVTT socket and dumping a file

1. Put the [Server.xml](Server.xml) config file in `bin/DEBUG/conf`
2. Create a empty file in `bin/DEBUG/dump/webvtt/subtitles.vtt`
3. Run the OvenMediaEngine [using CLion](../development/development-using-clion.md)
4. Open a terminal and navegate to docs/webvtt
5. Run `python3 subtitles_client.py` to generate subtitles and push them to the connection 9091 in OvenMedia
6. The file in `bin/DEBUG/dump/webvtt/subtitles.vtt` will be updated with the subtitles text.