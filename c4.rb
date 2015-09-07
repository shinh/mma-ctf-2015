#!/usr/bin/env ruby

c = 'e3 e3 83 21 33 96 23 43 ef 9a 9a 05 18 c7 23 07 07 07 c7 9a 04 33 23 07 23 ef 12 c7 04 96 43 23 23 18 04 04 05 c7 fb 18 96 43 ef 43 ff'

tbl = '04 c7 23 c3 18 96 05 9a 07 12 83 2c 1a 1b 6e 5a a0 52 3b d6 b3 29 e3 2f 84 53 d1 00 ed 20 fc b1 5b 6a cb be ef aa fb 43 4d 33 85 45 f9 02 7f 50 3c 9f a8 51 a3 40 8f 92 9d 38 f5 bc b6 da'.split

a = [*'0'..'9'] + [*'A'..'Z'] + [*'a'..'z']

c.split.each do |v|
  i = tbl.index(v)
  if i
    putc a[i]
  else
    putc '?'
  end
end
