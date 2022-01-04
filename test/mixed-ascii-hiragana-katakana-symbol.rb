#!/usr/bin/env bismite-run
DIR="build/test/mixed-ascii-hiragana-katakana-symbol"

FONT12    = "test/fonts/gohufont/gohufont-11.bdf"
FONT12B   = "test/fonts/gohufont/gohufont-11b.bdf"
FONT12_JP = "test/fonts/shinonome-plus/shnmk12u.pcf.gz"
FONT12B_JP= "test/fonts/shinonome-plus/shnmk12ub.pcf.gz"

FONT14    = "test/fonts/gohufont/gohufont-14.bdf"
FONT14B   = "test/fonts/gohufont/gohufont-14b.bdf"
FONT14_JP = "test/fonts/shinonome-plus/shnmk14u.pcf.gz"
FONT14B_JP= "test/fonts/shinonome-plus/shnmk14ub.pcf.gz"

CP_ASCII   ="codepoints/blocks/basic_latin.codepoint"
CP_HIRAGANA="codepoints/blocks/hiragana.codepoint"
CP_KATAKABA="codepoints/blocks/katakana.codepoint"
CP_SYMBOL  ="codepoints/blocks/cjk_symbols_and_punctuation.codepoint"

`mkdir -p #{DIR}`
cmd =<<EOS
./build/bin/bismite-font-sheeter #{DIR}/font.png 512 4 \
  #{DIR}/font12.dat 4 \
    #{CP_ASCII}    #{FONT12}    11 0.0 \
    #{CP_HIRAGANA} #{FONT12_JP} 12 0.0 \
    #{CP_KATAKABA} #{FONT12_JP} 12 0.0 \
    #{CP_SYMBOL}   #{FONT12_JP} 12 0.0 \
  #{DIR}/font12b.dat 4 \
    #{CP_ASCII}    #{FONT12B}    11 0.0 \
    #{CP_HIRAGANA} #{FONT12B_JP} 12 0.0 \
    #{CP_KATAKABA} #{FONT12B_JP} 12 0.0 \
    #{CP_SYMBOL}   #{FONT12B_JP} 12 0.0 \
  #{DIR}/font14.dat 4 \
    #{CP_ASCII}    #{FONT14}    14 0.0 \
    #{CP_HIRAGANA} #{FONT14_JP} 14 0.0 \
    #{CP_KATAKABA} #{FONT14_JP} 14 0.0 \
    #{CP_SYMBOL}   #{FONT14_JP} 14 0.0 \
  #{DIR}/font14b.dat 4 \
    #{CP_ASCII}    #{FONT14B}    14 0.0 \
    #{CP_HIRAGANA} #{FONT14B_JP} 14 0.0 \
    #{CP_KATAKABA} #{FONT14B_JP} 14 0.0 \
    #{CP_SYMBOL}   #{FONT14B_JP} 14 0.0 \
> #{DIR}/log.txt
EOS
`#{cmd}`


Bi::init 480,320, title:__FILE__
texture = Bi::Texture.new "#{DIR}/font.png"

layer = Bi::Layer.new
layer.root = Bi::Node.new
Bi::layers.add_layer layer
layer.set_texture 0, texture

fonts = [
  Bi::Font.new(texture,File.read("#{DIR}/font12.dat")),
  Bi::Font.new(texture,File.read("#{DIR}/font12b.dat")),
  Bi::Font.new(texture,File.read("#{DIR}/font14.dat")),
  Bi::Font.new(texture,File.read("#{DIR}/font14b.dat"))
]

height = [ 12, 12, 14, 14 ]

texts = [
  "The quick brown fox jumps over the lazy dog",
  "いろはにほへと　ちりぬるを　わかよたれそ　つねならむ",
  "カタカナと、Alphabetと、ひらがな。",
]

y = 20
texts.each{|text|
  fonts.each_with_index{|font,i|
    label = Bi::Label.new font
    label.set_text text
    label.set_position 20, y
    label.set_color 0xff-rand(50), 0xff-rand(50), 0xff-rand(50), 0xff
    label.set_background_color rand(100),rand(100),rand(100),0xff
    layer.root.add label
    y += (height[i] * 1.4).to_i
  }
}

Bi::start_run_loop
