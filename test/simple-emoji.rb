#!/usr/bin/env bismite-run
DIR="build/test/simple-emoji"
FONT="test/fonts/noto-emoji/NotoColorEmoji.ttf"

`mkdir -p #{DIR}`
cmd =<<EOS
./build/bin/bismite-font-sheeter #{DIR}/simple-emoji.png 2048 1 \
  #{DIR}/emoji.dat 1 \
    - #{FONT} 32 0.0 \
  > #{DIR}/log.txt
EOS
`#{cmd}`

Bi::init 480,320, title:__FILE__
texture = Bi::Texture.new "#{DIR}/simple-emoji.png"

layer = Bi::Layer.new
layer.root = Bi::Node.new
Bi::layers.add_layer layer
layer.set_texture 0, texture

font = Bi::Font.new(texture,File.read("#{DIR}/emoji.dat"))
label = Bi::Label.new font
label.set_text "⏩⌛👺🀄🟥🌹💥"

label.set_position 10, Bi.h/2
label.set_color 0,0,0,0
label.set_background_color rand(100),rand(100),rand(100),0xff
layer.root.add label

Bi::start_run_loop
