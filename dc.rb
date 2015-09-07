#!/usr/bin/env ruby

# for Digital Circuit

require './ctfutils'

def get_c1(v)
  res = `vvp ./digital_circuit/program +input=#{$input + v * 2 ** (128+64)}`
  res = res.force_encoding('ASCII-8BIT')
  if res !~ /\(c1\)=\s*(\d+)/
    raise res
  end
  $1.to_i
end


mul = 0b00000000000000000000000000000000000000000000000000100010010010000100010101001100010011000100111100100001001000010010001000100001
xorv = 0b00001110111000011011000000000101000110001011000101111110100100000111110001100000110010110000100011000101110010000001011110110010
c40 = invmod(mul, 2**128) * xorv % (2**128)

c20 = 6442758005744298871 * 2**128

$input = c40 + c20

base = get_c1(0)

a = []
64.times{
  a << []
}

diffs = []

STDERR.puts '%b' % base
64.times{|i|
  diff = base ^ get_c1(2 ** i)
  diffs << diff

  d = diff
  j = 0
  while d != 0
    if d % 2 == 1
      a[j] << i
    end
    d >>= 1
    j += 1
  end
}

STDERR.puts a.inspect
STDERR.puts diffs.inspect

c10 = 0

c = base
64.times{|b|
  if a[b].size != 1
    raise
  end

  if a[b][0] != b
    raise
  end

  if (c >> b) % 2 == 1
    STDERR.puts "#{b} #{1<<b} #{diffs[b]}"
    c10 += 1 << b
    c ^= diffs[b]
  end

  a = a.map{|cands|
    cands - [b]
  }
}

STDERR.puts c
STDERR.puts c10

c10 = 0
[1,2,4,5,6,12,13,14,16,17,18,22,25,27,28,30,33,34,36,37,38,40,43,44,
45,49,52,53,57,60,62].each do |b|
  c10 |= 1 << b
end

STDERR.puts '%064b' % base

STDERR.puts '%064b'%get_c1(c10)
STDERR.puts '%064b'%get_c1(c10 | (1 << 62))

c10 *= 2**(128+64)

#  l = ('%b'%diff).size
#  puts "#{i} #{diff} %b vs %b" % [diff, ('%b'%base)[-l+1..-1]]
#}

#c10 = 2 * 2**(128+64)
puts c10 + c20 + c40
