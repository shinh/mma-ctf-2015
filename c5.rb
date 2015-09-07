#!/usr/bin/env ruby

require './ctfutils'

c = '60 00 0c 3a 1e 52 02 53 02 51 0c 5d 56 51 5a 5f 5f 5a 51 00 05 53 56 0a 5e 00 52 05 03 51 50 55 03 04 52 04 0f 0f 54 52 57 03 52 04 4e'.split

a = [*'0'..'9'] + [*'a'..'f']

d = 'MMA{e000000000000000000000000000000000000000}'
n = 5
while d[n] != '}'
  a.each do |ch|
    d[n] = ch
    resp = `curl -d 's=#{d}' http://bow.chal.mmactf.link/~scs/crypt5.cgi 2> /dev/null`
    r = resp[/\/h1>(.*?) </, 1].split
    STDERR.puts "try: #{d} got: #{r}"
    if r[n] == c[n]
      puts d
      break
    end
  end

  n += 1
end
