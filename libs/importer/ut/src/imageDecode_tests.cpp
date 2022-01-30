#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include "importer/decode.hpp"
#include "tools/base64.hpp"
#include <fmt/core.h>
#include <ranges>

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
const auto expectedImage = common::Image(tools::base64_decode(pixels), 16);
std::string diff(const common::Image &a, const common::Image &b)
{
    // if (a == b) return "";
    std::stringstream o;
    o << "  because: ";

#define check(x) if (a.x != b.x) o << fmt::format("[{}: {} != {}]", #x, a.x, b.x);
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
                    if (sa == sb)
                    {
                        o << fmt::format("[mipmap {}, layer {}, face {} differs]\n", mipmap, layer, face);
                        o << "[compare pixels]";
                        break;
                    }
                }
    }
    return o.str();
}
} // namespace

TEST(imageDecode, decodeBmp)
{
    common::Image img;

    EXPECT_TRUE(Importer::DecodeBMP(img, tools::base64_decode(bmp_24bit)));
    EXPECT_EQ(expectedImage, img) << diff(expectedImage, img);

    EXPECT_TRUE(Importer::DecodeBMP(img, tools::base64_decode(bmp_8bit)));
    EXPECT_EQ(expectedImage, img) << diff(expectedImage, img);

    EXPECT_TRUE(Importer::DecodeBMP(img, tools::base64_decode(bmp_4bit)));
    EXPECT_EQ(expectedImage, img) << diff(expectedImage, img);

    EXPECT_FALSE(Importer::DecodeBMP(img, tools::base64_decode(bmp_32bit)));
    // EXPECT_EQ(expectedImage, img) << diff(expectedImage, img); // we have only 32 bit with compression... not supported
}

TEST(imageDecode, decodeTGA)
{
    common::Image img;
    // std::cout << "rav: " << MemoryBuffer{tools::base64_decode(tga_24bit_nocompression)};
    EXPECT_TRUE(Importer::DecodeTGA(img, tools::base64_decode(tga_24bit_nocompression)));
    EXPECT_EQ(expectedImage, img) << diff(expectedImage, img);

    EXPECT_TRUE(Importer::DecodeTGA(img, tools::base64_decode(tga_32bit_nocompression)));
    EXPECT_EQ(expectedImage, img) << diff(expectedImage, img);

    EXPECT_TRUE(Importer::DecodeTGA(img, tools::base64_decode(tga_24bit_compression)));
    EXPECT_EQ(expectedImage, img) << diff(expectedImage, img);

    EXPECT_TRUE(Importer::DecodeTGA(img, tools::base64_decode(tga_32bit_compression)));
    EXPECT_EQ(expectedImage, img) << diff(expectedImage, img);
}

class sdlImageDecode : public ::testing::TestWithParam<const char*>
{
protected:
    const char* base64Img;
};

TEST_P(sdlImageDecode, decodeImage)
{
    auto base64Image = GetParam();
    common::Image img;

    EXPECT_TRUE(Importer::DecodeSDL(img, tools::base64_decode(base64Image)));
    EXPECT_EQ(expectedImage, img) << diff(expectedImage, img);
}

INSTANTIATE_TEST_CASE_P(
        sdlImageDecodeFromBase64String,
        sdlImageDecode,
        ::testing::Values(
            bmp_24bit,
            bmp_8bit,
            bmp_4bit,
            bmp_32bit,
            png_24bit,
            png_32bit,
            png_4bit,
            png_8bit
        ));


TEST(imageDecode, decodeTgaWithSdl)
{
    common::Image img;

    EXPECT_TRUE(Importer::DecodeSDL(img, tools::base64_decode(tga_24bit_nocompression), "TGA"));
    EXPECT_EQ(expectedImage, img) << diff(expectedImage, img);

    EXPECT_TRUE(Importer::DecodeSDL(img, tools::base64_decode(tga_32bit_nocompression), "TGA"));
    EXPECT_EQ(expectedImage, img) << diff(expectedImage, img);

    EXPECT_TRUE(Importer::DecodeSDL(img, tools::base64_decode(tga_24bit_compression), "TGA"));
    EXPECT_EQ(expectedImage, img) << diff(expectedImage, img);

    EXPECT_TRUE(Importer::DecodeSDL(img, tools::base64_decode(tga_32bit_compression), "TGA"));
    EXPECT_EQ(expectedImage, img) << diff(expectedImage, img);
}