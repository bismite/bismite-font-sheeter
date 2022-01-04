#!/usr/bin/env ruby

# s: int16_t
# S: uint16_t
# l: int32_t
# L: uint32_t

# typedef struct {
#   uint32_t version;
#   uint16_t font_size;
#   int16_t height;
#   int16_t ascent;
#   int16_t descent;
#   uint32_t glyph_count;
# } FontHeader;
#
# typedef struct {
#     uint16_t x;
#     uint16_t y;
#     uint16_t w;
#     uint16_t h;
#     int16_t base_x;
#     int16_t base_y;
#     int16_t advance_x;
#     int16_t advance_y;
#     uint32_t codepoint;
#     uint32_t reserved;
# } GlyphLayout;

# 128bit 16byte
def get_header(dat)
  version,font_size,height,ascent,descent,glyph_count = dat.unpack("LSsssL")
  {
    version:version,
    font_size:font_size,
    height:height,
    ascent:ascent,
    descent:descent,
    glyph_count:glyph_count
  }
end

# 24bytes
def get_glyph(dat)
  return unless dat
  x,y,w,h,base_x,base_y,ad_x,ad_y,cp,reserved = dat.unpack("SSSSssssLL")
  {
    xywh: [x,y,w,h],
    base:[base_x,base_y], advanse:[ad_x,ad_y],
    char:[cp].pack("U"), codepoint:("%x"%cp)
  }
end

dat = File.open(ARGV.first,"rb").read # read as binary
filesize = File.size(ARGV.first)
header = get_header dat
p header
puts "dat length: #{dat.size}"
header[:glyph_count].times{|i|
  start_at = 16+i*24
  end_at = start_at + 24
  g = get_glyph dat[start_at..end_at]
  puts "#{i+1}/#{header[:glyph_count]}(#{start_at}/#{filesize}bytes) #{g}"
}
