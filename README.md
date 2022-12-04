# Btrieve Dump
Reads a [Btrieve](https://en.wikipedia.org/wiki/Btrieve) data file and prints every record, in the order found in the file. Each line of output corresponds to one record in space-delimited, hexadecimal format.

## Usage

Let's say we have a Btrieve file called `valar.dat` with 15 records measuring 22 bytes each matching the following structure:
```c
struct Valar {
    uint8_t id;
    uint8_t name[20];
    uint8_t spouse_id;
};
```

```shell
$ btrieve-dump valar.dat
01 4D 61 6E 77 C3 AB 20 53 C3 BA 6C 69 6D 6F 00 00 00 00 00 00 09
02 4D 65 6C 6B 6F 72 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
03 55 6C 6D 6F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
04 41 75 6C C3 AB 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0A
05 4F 72 6F 6D C3 AB 20 41 6C 64 61 72 6F 6E 00 00 00 00 00 00 0E
06 4E C3 A1 6D 6F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0D
07 49 72 6D 6F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0C
08 54 75 6C 6B 61 73 20 41 73 74 61 6C 64 6F 00 00 00 00 00 00 0F
09 56 61 72 64 61 20 45 6C 65 6E 74 C3 A1 72 69 00 00 00 00 00 01
0A 59 61 76 61 6E 6E 61 20 4B 65 6D 65 6E 74 C3 A1 72 69 00 00 04
0B 4E 69 65 6E 6E 61 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0C 45 73 74 C3 AB 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 07
0D 56 61 69 72 C3 AB 00 00 00 00 00 00 00 00 00 00 00 00 00 00 06
0E 56 C3 A1 6E 61 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 05
0F 4E 65 73 73 61 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 08
```

If you need it in binary format, you could use something like [xxd](https://manned.org/xxd.1):
```shell
$ btrieve-dump valar.dat | xxd -revert -plain > valar.bin
$ xxd -cols 22 -groupsize 1 valar.bin
00000000: 01 4d 61 6e 77 c3 ab 20 53 c3 ba 6c 69 6d 6f 00 00 00 00 00 00 09  .Manw.. S..limo.......
00000016: 02 4d 65 6c 6b 6f 72 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  .Melkor...............
0000002c: 03 55 6c 6d 6f 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  .Ulmo.................
00000042: 04 41 75 6c c3 ab 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0a  .Aul..................
00000058: 05 4f 72 6f 6d c3 ab 20 41 6c 64 61 72 6f 6e 00 00 00 00 00 00 0e  .Orom.. Aldaron.......
0000006e: 06 4e c3 a1 6d 6f 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0d  .N..mo................
00000084: 07 49 72 6d 6f 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0c  .Irmo.................
0000009a: 08 54 75 6c 6b 61 73 20 41 73 74 61 6c 64 6f 00 00 00 00 00 00 0f  .Tulkas Astaldo.......
000000b0: 09 56 61 72 64 61 20 45 6c 65 6e 74 c3 a1 72 69 00 00 00 00 00 01  .Varda Elent..ri......
000000c6: 0a 59 61 76 61 6e 6e 61 20 4b 65 6d 65 6e 74 c3 a1 72 69 00 00 04  .Yavanna Kement..ri...
000000dc: 0b 4e 69 65 6e 6e 61 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  .Nienna...............
000000f2: 0c 45 73 74 c3 ab 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 07  .Est..................
00000108: 0d 56 61 69 72 c3 ab 00 00 00 00 00 00 00 00 00 00 00 00 00 00 06  .Vair.................
0000011e: 0e 56 c3 a1 6e 61 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 05  .V..na................
00000134: 0f 4e 65 73 73 61 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 08  .Nessa................
```
