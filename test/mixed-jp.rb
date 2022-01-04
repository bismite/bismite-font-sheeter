#!/usr/bin/env bismite-run
DIR="build/test/mixed-jp"
FONT6X12 ="test/fonts/shinonome-plus/shnm6x12u.pcf.gz"
FONT12X12="test/fonts/shinonome-plus/shnmk12u.pcf.gz"
FONT7X14 ="test/fonts/shinonome-plus/shnm7x14u.pcf.gz"
FONT14X14="test/fonts/shinonome-plus/shnmk14u.pcf.gz"
FONT8X16 ="test/fonts/shinonome-plus/shnm8x16u.pcf.gz"
FONT16X16="test/fonts/shinonome-plus/shnmk16u.pcf.gz"
CP_ASCII ="codepoints/blocks/basic_latin.codepoint"
CP_JP    ="codepoints/jis0208.codepoint"

`mkdir -p #{DIR}`
cmd =<<EOS
./build/bin/bismite-font-sheeter #{DIR}/font.png 4096 3 \
  #{DIR}/font12.dat 2 \
    #{CP_ASCII} #{FONT6X12} 12 0.0 \
    #{CP_JP}    #{FONT12X12} 12 0.0 \
  #{DIR}/font14.dat 2 \
    #{CP_ASCII} #{FONT7X14} 14 0.0 \
    #{CP_JP}    #{FONT14X14} 14 0.0 \
  #{DIR}/font16.dat 2 \
    #{CP_ASCII} #{FONT8X16} 16 0.0 \
    #{CP_JP}    #{FONT16X16} 16 0.0 \
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
  Bi::Font.new(texture,File.read("#{DIR}/font14.dat")),
  Bi::Font.new(texture,File.read("#{DIR}/font16.dat"))
]

height = [ 12, 14, 16 ]

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
