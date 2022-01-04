#!/usr/bin/env bismite-run
DIR="build/test/simple"
FONT="test/fonts/gohufont/gohufont-11.bdf"

`mkdir -p #{DIR}`
cmd =<<EOS
./build/bin/bismite-font-sheeter #{DIR}/simple.png 256 1 \
  #{DIR}/font11.dat 1 \
    - #{FONT} 11 0.0 \
  > #{DIR}/log.txt
EOS
`#{cmd}`

Bi::init 480,320, title:__FILE__
texture = Bi::Texture.new "#{DIR}/simple.png"

layer = Bi::Layer.new
layer.root = Bi::Node.new
Bi::layers.add_layer layer
layer.set_texture 0, texture

font = Bi::Font.new(texture,File.read("#{DIR}/font11.dat"))

texts = [
  "The quick brown fox jumps over the lazy dog",
  "0123456789",
  "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~",
]

y = 20
texts.each{|text|
  label = Bi::Label.new font
  label.set_text text
  label.set_position 20, 100+y
  label.set_color 0xff-rand(50), 0xff-rand(50), 0xff-rand(50), 0xff
  label.set_background_color rand(100),rand(100),rand(100),0xff
  layer.root.add label
  y += 20
}

Bi::start_run_loop
