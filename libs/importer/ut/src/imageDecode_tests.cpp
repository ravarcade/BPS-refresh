#include <fmt/core.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <ranges>
#include "importer/decode.hpp"
#include "tools/base64.hpp"
#include "common/enumFormatter.hpp"

namespace
{
// below are files with images stored as base64 string... stupid, but works
const auto tga_32bit_compression =
    "AAAKAAAAAAAAAAAAEAAQACAIg/8ASP+D/wCy/4PcAP//g24A//+D/wBI/4P/ALL/g9wA//+DbgD//4P/AEj/g/8Asv+D3AD//4NuAP//g/8ASP+D/"
    "wCy/4PcAP//g24A//+DkP8A/4P//wD/g/+UAP+D/yYA/4OQ/wD/g///AP+D/5QA/4P/JgD/g5D/AP+D//8A/4P/lAD/g/8mAP+DkP8A/4P//wD/g/"
    "+UAP+D/yYA/4MA2P//gwD/tv+DAP9M/4Mh/wD/gwDY//+DAP+2/4MA/0z/gyH/AP+DANj//4MA/7b/gwD/TP+DIf8A/4MA2P//gwD/tv+DAP9M/"
    "4Mh/wD/gwAAAP+DQEBA/4MAAP//gwBq//+DAAAA/4NAQED/gwAA//+DAGr//4MAAAD/g0BAQP+DAAD//4MAav//gwAAAP+DQEBA/4MAAP//gwBq///"
    "vAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAJAOYHDAAoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABwY"
    "WludC5uZXQgNC4yLjE0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIAAAAAABAAEAAQABAAAAAAAAAAAAAAAAAANSAQAAAAAAAFRSVUVWSVNJT04"
    "tWEZJTEUuAA==";

const auto tga_24bit_compression =
    "AAAKAAAAAAAAAAAAEAAQABgAg/8ASIP/ALKD3AD/g24A/4P/AEiD/wCyg9wA/4NuAP+D/wBIg/8AsoPcAP+DbgD/g/8ASIP/ALKD3AD/g24A/4OQ/"
    "wCD//8Ag/+UAIP/JgCDkP8Ag///AIP/lACD/yYAg5D/AIP//wCD/5QAg/8mAIOQ/wCD//8Ag/+UAIP/JgCDANj/gwD/toMA/0yDIf8AgwDY/4MA/"
    "7aDAP9MgyH/AIMA2P+DAP+2gwD/TIMh/wCDANj/gwD/toMA/0yDIf8AgwAAAINAQECDAAD/gwBq/4MAAACDQEBAgwAA/"
    "4MAav+DAAAAg0BAQIMAAP+DAGr/gwAAAINAQECDAAD/gwBq/"
    "+8BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAkA5gcMACgALwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHB"
    "haW50Lm5ldCA0LjIuMTQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgAAAAAAEAAQABAAEAAAAAAAAAAAAAAAAAABIBAAAAAAAAVFJVRVZJU0lPT"
    "i1YRklMRS4A";

const auto tga_32bit_nocompression =
    "AAACAAAAAAAAAAAAEAAQACAI/wBI//8ASP//AEj//wBI//8Asv//ALL//wCy//8Asv/cAP//3AD//9wA///cAP//bgD//24A//9uAP//bgD///"
    "8ASP//AEj//wBI//8ASP//ALL//wCy//8Asv//ALL/3AD//9wA///cAP//3AD//24A//9uAP//bgD//24A////AEj//wBI//8ASP//AEj//wCy//"
    "8Asv//ALL//wCy/9wA///cAP//3AD//9wA//9uAP//bgD//24A//9uAP///wBI//8ASP//AEj//wBI//8Asv//ALL//wCy//8Asv/cAP//3AD//"
    "9wA///cAP//bgD//24A//9uAP//bgD//5D/AP+Q/wD/kP8A/5D/AP///wD///8A////AP///wD//5QA//+UAP//lAD//5QA//8mAP//JgD//yYA//"
    "8mAP+Q/wD/kP8A/5D/AP+Q/wD///8A////AP///wD///8A//+UAP//lAD//5QA//+UAP//JgD//yYA//8mAP//JgD/kP8A/5D/AP+Q/wD/kP8A////"
    "AP///wD///8A////AP//lAD//5QA//+UAP//lAD//yYA//8mAP//JgD//yYA/5D/AP+Q/wD/kP8A/5D/AP///wD///8A////AP///wD//5QA//"
    "+UAP//lAD//5QA//8mAP//JgD//yYA//8mAP8A2P//ANj//wDY//8A2P//AP+2/wD/tv8A/7b/AP+2/wD/TP8A/0z/AP9M/wD/TP8h/wD/If8A/yH/"
    "AP8h/wD/ANj//wDY//8A2P//ANj//wD/tv8A/7b/AP+2/wD/tv8A/0z/AP9M/wD/TP8A/0z/If8A/yH/AP8h/wD/If8A/wDY//8A2P//ANj//wDY//"
    "8A/7b/AP+2/wD/tv8A/7b/AP9M/wD/TP8A/0z/AP9M/yH/AP8h/wD/If8A/yH/AP8A2P//ANj//wDY//8A2P//AP+2/wD/tv8A/7b/AP+2/wD/"
    "TP8A/0z/AP9M/wD/TP8h/wD/If8A/yH/AP8h/wD/AAAA/wAAAP8AAAD/AAAA/0BAQP9AQED/QEBA/0BAQP8AAP//AAD//wAA//8AAP//AGr//wBq//"
    "8Aav//AGr//wAAAP8AAAD/AAAA/wAAAP9AQED/QEBA/0BAQP9AQED/AAD//wAA//8AAP//AAD//wBq//8Aav//AGr//wBq//8AAAD/AAAA/"
    "wAAAP8AAAD/QEBA/0BAQP9AQED/QEBA/wAA//8AAP//AAD//wAA//8Aav//AGr//wBq//8Aav//AAAA/wAAAP8AAAD/AAAA/0BAQP9AQED/QEBA/"
    "0BAQP8AAP//AAD//wAA//8AAP//AGr//wBq//8Aav//AGr//"
    "+8BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAkA5gcMACgAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHB"
    "haW50Lm5ldCA0LjIuMTQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgAAAAAAEAAQABAAEAAAAAAAAAAAAAAAAAAxIEAAAAAAAAVFJVRVZJU0lPT"
    "i1YRklMRS4A";

const auto tga_24bit_nocompression =
    "AAACAAAAAAAAAAAAEAAQABgA/wBI/wBI/wBI/wBI/wCy/wCy/wCy/wCy3AD/3AD/3AD/3AD/bgD/bgD/bgD/bgD//wBI/wBI/wBI/wBI/wCy/wCy/"
    "wCy/wCy3AD/3AD/3AD/3AD/bgD/bgD/bgD/bgD//wBI/wBI/wBI/wBI/wCy/wCy/wCy/wCy3AD/3AD/3AD/3AD/bgD/bgD/bgD/bgD//wBI/wBI/"
    "wBI/wBI/wCy/wCy/wCy/wCy3AD/3AD/3AD/3AD/bgD/bgD/bgD/bgD/kP8AkP8AkP8AkP8A//8A//8A//8A//8A/5QA/5QA/5QA/5QA/yYA/yYA/"
    "yYA/yYAkP8AkP8AkP8AkP8A//8A//8A//8A//8A/5QA/5QA/5QA/5QA/yYA/yYA/yYA/yYAkP8AkP8AkP8AkP8A//8A//8A//8A//8A/5QA/5QA/"
    "5QA/5QA/yYA/yYA/yYA/yYAkP8AkP8AkP8AkP8A//8A//8A//8A//8A/5QA/5QA/5QA/5QA/yYA/yYA/yYA/yYAANj/ANj/ANj/ANj/"
    "AP+2AP+2AP+2AP+2AP9MAP9MAP9MAP9MIf8AIf8AIf8AIf8AANj/ANj/ANj/ANj/"
    "AP+2AP+2AP+2AP+2AP9MAP9MAP9MAP9MIf8AIf8AIf8AIf8AANj/ANj/ANj/ANj/"
    "AP+2AP+2AP+2AP+2AP9MAP9MAP9MAP9MIf8AIf8AIf8AIf8AANj/ANj/ANj/ANj/"
    "AP+2AP+2AP+2AP+2AP9MAP9MAP9MAP9MIf8AIf8AIf8AIf8AAAAAAAAAAAAAAAAAQEBAQEBAQEBAQEBAAAD/AAD/AAD/AAD/AGr/AGr/AGr/AGr/"
    "AAAAAAAAAAAAAAAAQEBAQEBAQEBAQEBAAAD/AAD/AAD/AAD/AGr/AGr/AGr/AGr/AAAAAAAAAAAAAAAAQEBAQEBAQEBAQEBAAAD/AAD/AAD/AAD/"
    "AGr/AGr/AGr/AGr/AAAAAAAAAAAAAAAAQEBAQEBAQEBAQEBAAAD/AAD/AAD/AAD/AGr/AGr/AGr/AGr/"
    "7wEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEACQDmBwwAKAAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAcG"
    "FpbnQubmV0IDQuMi4xNAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAAAAAAQABAAEAAQAAAAAAAAAAAAAAAAAAEgMAAAAAAABUUlVFVklTSU9O"
    "LVhGSUxFLgA=";

const auto bmp_32bit =
    "Qk2WBAAAAAAAAJYAAAB8AAAAEAAAABAAAAABACAAAwAAAAAAAADEDgAAxA4AAAAAAAAAAAAAAAD/AAD/AAD/AAAAAAAA/"
    "yBuaVcAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA/wAAAAD/AAAAAP8A/wBI//"
    "8ASP//AEj//wBI//8Asv//ALL//wCy//8Asv/cAP//3AD//9wA///cAP//bgD//24A//9uAP//bgD///8ASP//AEj//wBI//8ASP//ALL//wCy//"
    "8Asv//ALL/3AD//9wA///cAP//3AD//24A//9uAP//bgD//24A////AEj//wBI//8ASP//AEj//wCy//8Asv//ALL//wCy/9wA///cAP//3AD//"
    "9wA//9uAP//bgD//24A//9uAP///wBI//8ASP//AEj//wBI//8Asv//ALL//wCy//8Asv/cAP//3AD//9wA///cAP//bgD//24A//9uAP//bgD//"
    "5D/AP+Q/wD/kP8A/5D/AP///wD///8A////AP///wD//5QA//+UAP//lAD//5QA//8mAP//JgD//yYA//8mAP+Q/wD/kP8A/5D/AP+Q/wD///8A///"
    "/AP///wD///8A//+UAP//lAD//5QA//+UAP//JgD//yYA//8mAP//JgD/kP8A/5D/AP+Q/wD/kP8A////AP///wD///8A////AP//lAD//5QA//"
    "+UAP//lAD//yYA//8mAP//JgD//yYA/5D/AP+Q/wD/kP8A/5D/AP///wD///8A////AP///wD//5QA//+UAP//lAD//5QA//8mAP//JgD//yYA//"
    "8mAP8A2P//ANj//wDY//8A2P//AP+2/wD/tv8A/7b/AP+2/wD/TP8A/0z/AP9M/wD/TP8h/wD/If8A/yH/AP8h/wD/ANj//wDY//8A2P//ANj//wD/"
    "tv8A/7b/AP+2/wD/tv8A/0z/AP9M/wD/TP8A/0z/If8A/yH/AP8h/wD/If8A/wDY//8A2P//ANj//wDY//8A/7b/AP+2/wD/tv8A/7b/AP9M/wD/"
    "TP8A/0z/AP9M/yH/AP8h/wD/If8A/yH/AP8A2P//ANj//wDY//8A2P//AP+2/wD/tv8A/7b/AP+2/wD/TP8A/0z/AP9M/wD/TP8h/wD/If8A/yH/"
    "AP8h/wD/AAAA/wAAAP8AAAD/AAAA/0BAQP9AQED/QEBA/0BAQP8AAP//AAD//wAA//8AAP//AGr//wBq//8Aav//AGr//wAAAP8AAAD/AAAA/"
    "wAAAP9AQED/QEBA/0BAQP9AQED/AAD//wAA//8AAP//AAD//wBq//8Aav//AGr//wBq//8AAAD/AAAA/wAAAP8AAAD/QEBA/0BAQP9AQED/QEBA/"
    "wAA//8AAP//AAD//wAA//8Aav//AGr//wBq//8Aav//AAAA/wAAAP8AAAD/AAAA/0BAQP9AQED/QEBA/0BAQP8AAP//AAD//wAA//8AAP//AGr//"
    "wBq//8Aav//AGr//w==";

const auto bmp_24bit =
    "Qk02AwAAAAAAADYAAAAoAAAAEAAAABAAAAABABgAAAAAAAAAAADEDgAAxA4AAAAAAAAAAAAA/wBI/wBI/wBI/wBI/wCy/wCy/wCy/wCy3AD/3AD/"
    "3AD/3AD/bgD/bgD/bgD/bgD//wBI/wBI/wBI/wBI/wCy/wCy/wCy/wCy3AD/3AD/3AD/3AD/bgD/bgD/bgD/bgD//wBI/wBI/wBI/wBI/wCy/wCy/"
    "wCy/wCy3AD/3AD/3AD/3AD/bgD/bgD/bgD/bgD//wBI/wBI/wBI/wBI/wCy/wCy/wCy/wCy3AD/3AD/3AD/3AD/bgD/bgD/bgD/bgD/"
    "kP8AkP8AkP8AkP8A//8A//8A//8A//8A/5QA/5QA/5QA/5QA/yYA/yYA/yYA/yYAkP8AkP8AkP8AkP8A//8A//8A//8A//8A/5QA/5QA/5QA/5QA/"
    "yYA/yYA/yYA/yYAkP8AkP8AkP8AkP8A//8A//8A//8A//8A/5QA/5QA/5QA/5QA/yYA/yYA/yYA/yYAkP8AkP8AkP8AkP8A//8A//8A//8A//8A/"
    "5QA/5QA/5QA/5QA/yYA/yYA/yYA/yYAANj/ANj/ANj/ANj/AP+2AP+2AP+2AP+2AP9MAP9MAP9MAP9MIf8AIf8AIf8AIf8AANj/ANj/ANj/ANj/"
    "AP+2AP+2AP+2AP+2AP9MAP9MAP9MAP9MIf8AIf8AIf8AIf8AANj/ANj/ANj/ANj/"
    "AP+2AP+2AP+2AP+2AP9MAP9MAP9MAP9MIf8AIf8AIf8AIf8AANj/ANj/ANj/ANj/"
    "AP+2AP+2AP+2AP+2AP9MAP9MAP9MAP9MIf8AIf8AIf8AIf8AAAAAAAAAAAAAAAAAQEBAQEBAQEBAQEBAAAD/AAD/AAD/AAD/AGr/AGr/AGr/AGr/"
    "AAAAAAAAAAAAAAAAQEBAQEBAQEBAQEBAAAD/AAD/AAD/AAD/AGr/AGr/AGr/AGr/AAAAAAAAAAAAAAAAQEBAQEBAQEBAQEBAAAD/AAD/AAD/AAD/"
    "AGr/AGr/AGr/AGr/AAAAAAAAAAAAAAAAQEBAQEBAQEBAQEBAAAD/AAD/AAD/AAD/AGr/AGr/AGr/AGr/";

const auto bmp_8bit = "Qk12AQAAAAAAAHYAAAAoAAAAEAAAABAAAAABAAgAAAAAAAAAAADEDgAAxA4AABAAAAAQAAAAAAAA/0BAQP8AAP//AGr//"
                      "wDY//8A/7b/AP9M/yH/AP+Q/wD///8A//+UAP//JgD//wBI//8Asv/cAP//bgD//"
                      "wwMDAwNDQ0NDg4ODg8PDw8MDAwMDQ0NDQ4ODg4PDw8PDAwMDA0NDQ0ODg4ODw8PDwwMDAwNDQ0NDg4ODg8PDw8ICAgICQkJC"
                      "QoKCgoLCwsLCAgICAkJCQkKCgoKCwsLCwgICAgJCQkJCgoKCgsLCwsICAgICQkJCQoKCgoLCwsLBAQEBAUFBQUGBgYGBwcHB"
                      "wQEBAQFBQUFBgYGBgcHBwcEBAQEBQUFBQYGBgYHBwcHBAQEBAUFBQUGBgYGBwcHBwAAAAABAQEBAgICAgMDAwMAAAAAAQEBA"
                      "QICAgIDAwMDAAAAAAEBAQECAgICAwMDAwAAAAABAQEBAgICAgMDAwM=";

const auto bmp_4bit =
    "Qk32AAAAAAAAAHYAAAAoAAAAEAAAABAAAAABAAQAAAAAAAAAAADEDgAAxA4AABAAAAAQAAAAAAAA/0BAQP8AAP//AGr//wDY//8A/7b/AP9M/yH/"
    "AP+Q/wD///8A//+UAP//JgD//wBI//8Asv/cAP//bgD//8zM3d3u7v//zMzd3e7u///MzN3d7u7//8zM3d3u7v//"
    "iIiZmaqqu7uIiJmZqqq7u4iImZmqqru7iIiZmaqqu7tERFVVZmZ3d0REVVVmZnd3RERVVWZmd3dERFVVZmZ3dwAAEREiIjMzAAARESIiMzMAABERIi"
    "IzMwAAEREiIjMz";

const auto png_32bit = "iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/"
                       "9hAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAABuSURBVDhP3ZKxCYAwFAVf3"
                       "MTKNVLauIiNq4igkwiWZgwbZ/kKuQixtBE9CPeqXxxxkux8F957VmQNgQUthgI/"
                       "5v0Dzra8wVIxYMKJWSUr8ocGsj5rIHUYRpwYMPyhQS3LGty+uhrtrEQe4fMNpANRKRI0zAyhZwAAAABJRU5ErkJggg==";

const auto png_24bit =
    "iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAIAAACQkWg2AAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAA"
    "BoSURBVDhPY2RAAg4ODlAWA8P+AwegLCDIgtJAwASliQa018D4/waUBQTb1aEMIJgGpUFgC4MilDU4/cDwfwKUCQL5UBoIpkJpEJgEpYFgMPrBg+E/"
    "lImSZBh8Ge5CWSCA8MSg8wMDAwCPBA44Ga0g7QAAAABJRU5ErkJggg==";

const auto png_8bit = "iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAMAAAAoLQ9TAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAwUExURQAA"
                      "AEBAQP8AAP9qAP/YALb/AEz/AAD/IQD/kAD//wCU/wAm/0gA/7IA//8A3P8AbsPMQ/"
                      "kAAAAJcEhZcwAADsQAAA7EAZUrDhsAAAAwSURBVChTnchHAQAgEAQxeofz75ZBAptn3OMREKFEQkZBhRINHQMTSixsHBi+w+"
                      "wCpC4HgQcIHAAAAAAASUVORK5CYII=";

const auto png_4bit = "iVBORw0KGgoAAAANSUhEUgAAABAAAAAQBAMAAADt3eJSAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAwUExURQA"
                      "AAEBAQP8AAP9qAP/YALb/AEz/AAD/IQD/kAD//wCU/wAm/0gA/7IA//8A3P8AbsPMQ/"
                      "kAAAAJcEhZcwAADsQAAA7EAZUrDhsAAAA1SURBVBjTY2BgEBRUUjI2xsdwcQkNTUsrL8fH6OiYOXPVqt278THOnLl79927/"
                      "/9xMv7/BwDf9T/B+ETdLwAAAABJRU5ErkJggg==";

const auto jpg =
    "/9j/4AAQSkZJRgABAQEAYABgAAD/"
    "4QBoRXhpZgAATU0AKgAAAAgABAEaAAUAAAABAAAAPgEbAAUAAAABAAAARgEoAAMAAAABAAIAAAExAAIAAAARAAAATgAAAAAAAXcMAAAD6AABdwwAAA"
    "PocGFpbnQubmV0IDQuMi4xNAAA/"
    "9sAQwABAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEB/"
    "9sAQwEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEB/"
    "8AAEQgAEAAQAwERAAIRAQMRAf/EAB8AAAEFAQEBAQEBAAAAAAAAAAABAgMEBQYHCAkKC//"
    "EALUQAAIBAwMCBAMFBQQEAAABfQECAwAEEQUSITFBBhNRYQcicRQygZGhCCNCscEVUtHwJDNicoIJChYXGBkaJSYnKCkqNDU2Nzg5OkNERUZHSElKU"
    "1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6g4SFhoeIiYqSk5SVlpeYmZqio6Slpqeoqaqys7S1tre4ubrCw8TFxsfIycrS09TV1tfY2drh4uPk5ebn6On"
    "q8fLz9PX29/j5+v/EAB8BAAMBAQEBAQEBAQEAAAAAAAABAgMEBQYHCAkKC//"
    "EALURAAIBAgQEAwQHBQQEAAECdwABAgMRBAUhMQYSQVEHYXETIjKBCBRCkaGxwQkjM1LwFWJy0QoWJDThJfEXGBkaJicoKSo1Njc4OTpDREVGR0hJS"
    "lNUVVZXWFlaY2RlZmdoaWpzdHV2d3h5eoKDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uLj5OXm5+"
    "jp6vLz9PX29/j5+v/aAAwDAQACEQMRAD8A8t+BP/Bsj/wpf/hKf+M2v+Ek/wCEk/sP/m23+x/sX9j/ANr/APVfNV+0/af7V/6YeT5H/"
    "LXzf3f+Gf8AxWh/6xt/87F/+Sw+q8YOGf8AiFH+rv8Atv8Ab/8Ab/8Aa/8AzDf2X9U/sv8Asz/p/mPt/b/2j/059l7H/l57T3Poj4Y/sbf8M9f23/"
    "xcf/hL/wDhL/7N/wCZQ/sD+z/7A/tD/qaNa+1/a/7a/wCnbyPs3/Lbzv3X4fi/FD/iNPs/+EP/AFa/1a5/+Zn/AGz9d/tnl/6l+VfVvq39lf8AT/"
    "23t/8Al17L95630mP9K/8Abf6lf8cEcvL/AKyf85S81+b+wf8ArHNWtY8F+F//AAWA/t7+3P8AjHj7L9l/sz/mrXn+Z5/9of8AVModu3yf9rdu7Y5/"
    "9CLw8/Zb/wDEB/7Y/wCN5/61f61f2f8A82y/sP6h/Yf13/q4OcfWvrX9sf8AUP7D6v8A8vvbfuv8e/C//RrP+Izf25/x2j/q3/q3/Zn/ADjp/"
    "bH13+2P7Q/6vtlf1f6v/Zf/AE/9t7f/AJdey/efBv7RnhX/AIWv/wAId/p/9g/2B/wkP/Lr/av2v+1P7D/6eNO8jyP7O/6beb53/LPy/n+64h+mf/"
    "xLR9U/41v/AK6/66/WP+aw/wBXP7M/1c9j/wBUtn31z65/b3/UJ9X+qf8AL/2/7n/oe8G/9GH/ANQv9Y/+O3v7V/"
    "tX+yP+cavqPsPqP9qf9X+xntfa/XP+nfJ7P7fP7v8A/9k=";

const auto webp = "UklGRigBAABXRUJQVlA4WAoAAAAIAAAADwAADwAAVlA4TFMAAAAvD8ADAAbXkCQJ1tAcx/Ecz4N7NvM/"
                  "BJWK2kZyAm2f7dBWekHIRpKqciZHdCwP+C7T/xhUb2w1qj7h/i/sh12wDO6XhyX8qHqoup8Ms8P4q/"
                  "GqewBFWElGrgAAAElJKgAIAAAABgASAQMAAQAAAAEAAAAaAQUAAQAAAFYAAAAbAQUAAQAAAF4AAAAoAQMAAQAAAAIAAAAxAQIAEQ"
                  "AAAGYAAABphwQAAQAAAHgAAAAAAAAADHcBAOgDAAAMdwEA6AMAAHBhaW50Lm5ldCA0LjIuMTQAAAQAAJAHAAQAAAAwMjMwAaADAA"
                  "EAAAABAAAAAqAEAAEAAAAQAAAAA6AEAAEAAAAQAAAAAAAAAA==";

// pixels from test.bmp as rgba base64 binary data
const auto pixels =
    "SAD//0gA//9IAP//SAD//7IA//+yAP//sgD//7IA////ANz//wDc//8A3P//ANz//wBu//8Abv//AG7//wBu/0gA//9IAP//SAD//0gA//+yAP//"
    "sgD//7IA//+yAP///wDc//8A3P//ANz//wDc//8Abv//AG7//wBu//8Abv9IAP//SAD//0gA//9IAP//sgD//7IA//+yAP//sgD///8A3P//ANz//"
    "wDc//8A3P//AG7//wBu//8Abv//AG7/SAD//0gA//9IAP//SAD//7IA//+yAP//sgD//7IA////ANz//wDc//8A3P//ANz//wBu//8Abv//AG7//"
    "wBu/wD/kP8A/5D/AP+Q/wD/kP8A////AP///wD///8A////AJT//wCU//8AlP//AJT//wAm//8AJv//ACb//wAm//8A/5D/AP+Q/wD/kP8A/5D/AP/"
    "//wD///8A////AP///wCU//8AlP//AJT//wCU//8AJv//ACb//wAm//8AJv//AP+Q/wD/kP8A/5D/AP+Q/wD///8A////AP///wD///8AlP//AJT//"
    "wCU//8AlP//ACb//wAm//8AJv//ACb//wD/kP8A/5D/AP+Q/wD/kP8A////AP///wD///8A////AJT//wCU//8AlP//AJT//wAm//8AJv//ACb//"
    "wAm////2AD//9gA///YAP//2AD/tv8A/7b/AP+2/wD/tv8A/0z/AP9M/wD/TP8A/0z/AP8A/yH/AP8h/wD/If8A/yH//9gA///YAP//2AD//9gA/"
    "7b/AP+2/wD/tv8A/7b/AP9M/wD/TP8A/0z/AP9M/wD/AP8h/wD/If8A/yH/AP8h///YAP//2AD//9gA///YAP+2/wD/tv8A/7b/AP+2/wD/TP8A/"
    "0z/AP9M/wD/TP8A/wD/If8A/yH/AP8h/wD/If//2AD//9gA///YAP//2AD/tv8A/7b/AP+2/wD/tv8A/0z/AP9M/wD/TP8A/0z/AP8A/yH/AP8h/"
    "wD/If8A/yH/AAAA/wAAAP8AAAD/AAAA/0BAQP9AQED/QEBA/0BAQP//AAD//wAA//8AAP//AAD//2oA//9qAP//agD//2oA/wAAAP8AAAD/AAAA/"
    "wAAAP9AQED/QEBA/0BAQP9AQED//wAA//8AAP//AAD//wAA//9qAP//agD//2oA//9qAP8AAAD/AAAA/wAAAP8AAAD/QEBA/0BAQP9AQED/QEBA//"
    "8AAP//AAD//wAA//8AAP//agD//2oA//9qAP//agD/AAAA/wAAAP8AAAD/AAAA/0BAQP9AQED/QEBA/0BAQP//AAD//wAA//8AAP//AAD//2oA//"
    "9qAP//agD//2oA/w==";

const auto pixels2 =
    "SAH//0gB//9IAf//SAH//7EA/v+xAP7/sQD+/7IB////ANz//wDc//8A3P//ANz//wBu//8Abv//AG7//wBu/0gB//9IAf//SAH//0gB//+xAP7/"
    "sQD+/7EA/v+xAP7//wDc//8A3P//ANz//wDc//8Abv//AG7//wBu//8Abv9IAf//SAH//0gB//9IAf//sQD+/7EA/v+xAP7/sQD+//8A3P//ANz//"
    "wDc//8A3P//AG7//wBu//8Abv//AG7/SAH//0gB//9IAf//SAH//7EA/v+xAP7/sQD+/7EA/v//ANz//wDc//8A3P//ANz//wBu//8Abv//AG7//"
    "wBu/wH+kf8B/pH/Af6R/wH+kf8B////Af///wH///8B////AJT+/wCU/v8AlP7/AJT+/wAm//8AJv//ACb//wAm/f8B/pH/Af6R/wH+kf8B/pH/Af/"
    "//wH///8B////Af///wCU/v8AlP7/AJT+/wCU/v8AJv//ACb//wAm//8AJv//Af6R/wH+kf8B/pH/Af6R/wH///8B////Af///wH///8AlP7/AJT+/"
    "wCU/v8AlP7/ACb//wAm//8AJv//ACb//wH+kf8B/pH/Af6R/wH+kf8B////Af///wH///8A/v7/AJT+/wCU/v8AlP7/AJT+/wAm//8AJv//ACb//"
    "wAm////2AD//9kA///YAP//2AD/tv8A/7b/AP+2/wD/tv8A/0v/AP9L/wD/S/8A/0v/AP8A/iH/AP4h/wD+If8A/iH//9gA///YAP//2AD//9gA/"
    "7b/AP+2/wD/tv8A/7b/AP9L/wD/S/8A/0v/AP9L/wD/AP4h/wD+If8A/iH/AP4h///YAP//2AD//9gA///YAP+2/wD/tv8A/7b/AP+2/wD/S/8A/"
    "0v/AP9L/wD/S/8A/wD+If8A/iH/AP4h/wD+If//2AD//9gA///YAP//2AD/tv8A/7b/AP+2/wD/tv8A/0v/AP9L/wD/S/8A/0v/AP8A/iH/AP4h/"
    "wD+If8A/iH/AAAA/wAAAP8AAAD/AAAA/0BAQP9AQED/QEBA/0BAQP/+AAD//gAA//4AAP/+AAD//moA//5qAP/+agD//moA/wAAAP8AAAD/AAAA/"
    "wAAAP9AQED/QEBA/0BAQP9AQED//gAA//4AAP/+AAD//gAA//5qAP/+agD//moA//5qAP8AAAD/AAAA/wAAAP8AAAD/QEBA/0BAQP9AQED/QEBA//"
    "4AAP/9AQD//gAA//4AAP/+agD//moA//9pAP/+agD/AAAA/wAAAP8AAAD/AAAA/0BAQP9AQED/QEBA/0BAQP/+AAD//gAA//4AAP/+AAD//moA//"
    "5qAP/+agD//moA/w==";

const auto expectedImage = common::Image(tools::base64_decode(pixels), 16);
const auto expectedJpgImage = common::Image(tools::base64_decode(pixels2), 16);

std::string diff(const common::Image& a, const common::Image& b)
{
    std::stringstream o;

#define check(x) \
    if (a.x != b.x) o << fmt::format("[{}: {} != {}]", #x, a.x, b.x);
    check(width);
    check(height);
    check(format);
    check(pitch);
    check(num_mipmaps);
    check(num_layers);
    check(num_faces);
    check(isCube);
    check(isArray);
    if (o.str().size() == 0)
    {
        for (auto mipmap : std::ranges::iota_view(0u, a.num_mipmaps))
            for (auto layer : std::ranges::iota_view(0u, a.num_layers))
                for (auto face : std::ranges::iota_view(0u, a.num_layers))
                {
                    auto sa = a.getMipmap(mipmap, layer, face);
                    auto sb = b.getMipmap(mipmap, layer, face);
                    if (sa != sb)
                    {
                        o << fmt::format("  [error at mipmap {}, layer {}, face {}]", mipmap, layer, face);
                        for (auto row : std::ranges::iota_view(0u, a.height))
                        {
                            if (memcmp(sa.data() + row * a.pitch, sb.data() + row * b.pitch, a.pitch) != 0)
                            {
                                bool first = true;
                                auto ba = reinterpret_cast<const uint32_t*>(sa.data() + row * a.pitch);
                                auto bb = reinterpret_cast<const uint32_t*>(sb.data() + row * a.pitch);
                                for (auto col : std::ranges::iota_view(0u, a.width))
                                {
                                    if (ba[col] != bb[col])
                                    {
                                        if (first)
                                            o << fmt::format("\n  [x={}, y={}: ", col, row);
                                        else
                                            o << ", ";
                                        o << fmt::format("{:x}!={:x}", ba[col], bb[col]);
                                        first = false;
                                    }
                                }
                                if (not first) o << "]";
                            }
                        }
                        o << "\n";
                        return "  because: " + o.str();
                    }
                }
    }
    return "  because: " + o.str();
}
} // namespace

struct params
{
    const char* base64Img;
    const char* imgType{nullptr};
    const common::Image& expectedImg{expectedImage};
};

class sdlImageDecode : public ::testing::TestWithParam<params>
{
};

TEST_P(sdlImageDecode, decodeImage)
{
    auto [base64Image, imgType, expectedImage] = GetParam();
    common::Image img;

    EXPECT_TRUE(Importer::DecodeSDL(img, tools::base64_decode(base64Image), imgType));
    EXPECT_EQ(expectedImage, img) << diff(expectedImage, img);
}

INSTANTIATE_TEST_CASE_P(
    sdlImageDecodeFromBase64String,
    sdlImageDecode,
    ::testing::Values(
        params{bmp_24bit},
        params{bmp_8bit},
        params{bmp_4bit},
        params{bmp_32bit},
        params{png_24bit},
        params{png_32bit},
        params{png_4bit},
        params{png_8bit},
        params{webp},
        params{tga_24bit_nocompression, "TGA"},
        params{tga_32bit_nocompression, "TGA"},
        params{tga_24bit_compression, "TGA"},
        params{tga_32bit_compression, "TGA"},
        params{jpg, nullptr, expectedJpgImage}));
