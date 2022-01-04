# bismite-font-sheeter

## Commands

- `bismite-font-sheeter`
- `bismite-codepoint-extractor`

### bismite-font-sheeter

```
font-sheet-maker <path/to/output.png> <OutputTextureWidth> <NumbersOfDatFiles> \
  <DatFileName1> <NumbersOfFonts> \
    <path/to/code_points_file> <path/to/font> <FontSize> <OutlineSize> \
    ... \
  <DatFileName2> <NumbersOfFonts> \
    <path/to/code_points_file> <path/to/font> <FontSize> <OutlineSize> \
    ... \
  ... \
```

example-1: 4096px output image and 2 dat files (16pt and 24pt) with codepoints.
```
font-sheet-maker out.png 4096 2 \
  16pt.dat 2 \
    basic_latin.codepoint Foo.ttf 16 0.0 \
    hiragana.codepoint Bar.ttf 16 0.0 \
  24pt.dat 2 \
    basic_latin.codepoint Foo.ttf 24 0.0 \
    hiragana.codepoint Bar.ttf 24 0.0
```

example-2: 512px output image and 1 dat file (16pt) with all glyphs from font file.
```
font-sheet-maker out.png 512 1 \
  16pt.dat 1 \
    - A.ttf 16 0.0
```

### bismite-codepoint-extractor

`bismite-codepoint-extractor {--text|--font} <infile> <outfile>`

Outputs all codepoints to `<outfile>` from `<infile>`.
Accepts UTF-8 text file or font file.

## codepoints file

UTF-32 little endian codepoints. not UTF-8 or any others.
example, 👺(Tengu) is [F0,9F,91,BA] in UTF-8, but 1F47A in codepoints file.

# Build

[cairo](https://cairographics.org) required. (e.g. `apt install libcairo2-dev`)

# License
Copyright 2022 kbys <work4kbys@gmail.com>

Apache License Version 2.0

(This license does not include the font files under the test/fonts directory. Each of these fonts has its own license.)

## fonts

Gohufont(WTFPL) : https://font.gohu.org
shinonome-plus(public domain) : http://vega.pgw.jp/~kabe/vsd/shinonome-plus/unicode.html
noto-emoji(OFL) : https://github.com/googlefonts/noto-emoji
M+ Fonts(OFL) : https://github.com/coz-m/MPLUS_FONTS
