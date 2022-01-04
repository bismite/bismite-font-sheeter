#!/usr/bin/env ruby

utf8_txt = open("codepoints/jis0208.txt","w")
codepoint = open("codepoints/jis0208.codepoint","w")

codes_number=0
open("scripts/JIS0208.TXT").each_line{|l|
  l.gsub!(/#.*$/,"")
  a = l.strip.split("\t")
  # a[0] : SJIS
  # a[1] : JIS X 0208
  # a[2] : Unicode
  next if a.empty?
  dat = a[2].to_i(16)

  utf8_txt.write [dat].pack("U*")
  codepoint.write [dat].pack("V*")
  codes_number+=1
}
puts "#{codes_number} codes"
utf8_txt.close
codepoint.close
