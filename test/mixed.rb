#!/usr/bin/env bismite-run
DIR="build/test/mixed"
FONT11="test/fonts/gohufont/gohufont-11.bdf"
FONT11B="test/fonts/gohufont/gohufont-11b.bdf"
FONT14="test/fonts/gohufont/gohufont-14.bdf"
FONT14B="test/fonts/gohufont/gohufont-14b.bdf"
CP_ASCII="codepoints/blocks/basic_latin.codepoint"

`mkdir -p #{DIR}`
cmd =<<EOS
./build/bin/bismite-font-sheeter #{DIR}/font.png 512 4 \
  #{DIR}/font11.dat 1 \
    #{CP_ASCII} #{FONT11} 11 0.0 \
  #{DIR}/font11b.dat 1 \
    #{CP_ASCII} #{FONT11B} 11 0.0 \
  #{DIR}/font14.dat 1 \
    #{CP_ASCII} #{FONT14} 14 0.0 \
  #{DIR}/font14b.dat 1 \
    #{CP_ASCII} #{FONT14B} 14 0.0 \
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
  Bi::Font.new(texture,File.read("#{DIR}/font11.dat")),
  Bi::Font.new(texture,File.read("#{DIR}/font11b.dat")),
  Bi::Font.new(texture,File.read("#{DIR}/font14.dat")),
  Bi::Font.new(texture,File.read("#{DIR}/font14b.dat")),
]

texts = [
  "The quick brown fox jumps over the lazy dog",
  "0123456789 !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~",
]

y = 1
fonts.each{|font|
  texts.each{|text|
    label = Bi::Label.new font
    label.set_text text
    label.set_position 20, 30+y
    label.set_color 0xff-rand(50), 0xff-rand(50), 0xff-rand(50), 0xff
    label.set_background_color rand(100),rand(100),rand(100),0xff
    layer.root.add label
    y += 16
  }
}

Bi::start_run_loop
