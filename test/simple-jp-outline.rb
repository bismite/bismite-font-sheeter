#!/usr/bin/env bismite-run
DIR="build/test/simple-jp-outline"
FONT="test/fonts/mplus/Mplus1-Black.ttf"

`mkdir -p #{DIR}`
cmd =<<EOS
./build/bin/bismite-font-sheeter #{DIR}/simple-jp.png 4096 1 \
  #{DIR}/jp32.dat 1 \
    - #{FONT} 32 1.5 \
  > #{DIR}/log.txt
EOS
`#{cmd}`


Bi::init 480,320, title:__FILE__
texture = Bi::Texture.new "#{DIR}/simple-jp.png"

layer = Bi::Layer.new
layer.root = Bi::Node.new
Bi::layers.add_layer layer
layer.set_texture 0, texture

texts = [
  "The quick brown fox jumps over the lazy dog",
  "いろはにほへと　ちりぬるを　わかよたれそ　つねならむ",
  "カタカナと、Alphabetと、ひらがな。",
  "0123456789 !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~",
]

font = Bi::Font.new(texture,File.read("#{DIR}/jp32.dat"))

y = 20
texts.each{|text|
  label = Bi::Label.new font
  label.set_text text
  label.set_position 2, y
  label.set_color 0,0,0,0
  label.set_background_color rand(100),rand(100),rand(100),0xff
  layer.root.add label
  y += 60
}

Bi::start_run_loop
