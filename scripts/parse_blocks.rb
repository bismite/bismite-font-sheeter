#!/usr/bin/env ruby
require "fileutils"
require "json"

block_json = open("scripts/blocks.json","w")

blocks = []
open("scripts/Blocks.txt").each_line{|l|
  l = l.gsub(/#.*$/,"").strip
  next if l.empty?

  a = l.strip.split(";")

  name = a[1].strip
  symbol = name.downcase.gsub(/ |-/,"_").to_sym
  start,last = a[0].strip.split("..")

  p [start, last, name, symbol ]
  blocks << [ symbol, start, last, name ]
}

puts "#{blocks.size} blocks."
block_json.write JSON::pretty_generate(blocks)

# UTF8 text and codepoint
FileUtils.mkdir_p "codepoints/blocks"
blocks.each{|b|
  symbol = b[0]
  first = b[1].to_i(16)
  last = b[2].to_i(16)
  name = b[3]

  # txt = (first..last).map{|code| [code].pack("U*") }.flatten.join
  txt = (first..last).to_a.pack("U*")
  codepoint = (first..last).to_a.pack("V*")

  File.write "codepoints/blocks/#{symbol}.txt", txt
  File.write "codepoints/blocks/#{symbol}.codepoint", codepoint
}
