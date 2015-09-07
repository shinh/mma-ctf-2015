#!/usr/bin/env ruby

require './ctfutils'

buf = ' ' * 3745

pcap = File.read('splitted/splitted.pcap')
reg = Regexp.new("Partial Content\\s+.*?Content-Length: (\\d+).*?Content-Range: bytes (\\d+)-(\\d+).*?\r\n\r\n".encode("ASCII-8BIT"), Regexp::MULTILINE)
pcap.scan(reg) do
  len = $1.to_i
  st = $2.to_i
  ed = $3.to_i
  #buf = buf[0,st] + $'[0,len] + buf[ed..-1]
  buf[st,len] = $'[0,len]
end

print buf
