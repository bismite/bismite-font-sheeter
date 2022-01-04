#!/usr/bin/env bismite-run
DIR="build/test/simple-jp"
FONT="test/fonts/mplus/Mplus1-Regular.ttf"

`mkdir -p #{DIR}`
cmd =<<EOS
./build/bin/bismite-font-sheeter #{DIR}/simple-jp.png 2048 1 \
  #{DIR}/jp18.dat 1 \
    - #{FONT} 18 0.0 \
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

font = Bi::Font.new(texture,File.read("#{DIR}/jp18.dat"))

y = 20
texts.each{|text|
  label = Bi::Label.new font
  label.set_text text
  label.set_position 2, y
  label.set_color 0xff-rand(50), 0xff-rand(50), 0xff-rand(50), 0xff
  label.set_background_color rand(100),rand(100),rand(100),0xff
  layer.root.add label
  y += 32
}

Bi::start_run_loop
