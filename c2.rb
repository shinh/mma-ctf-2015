c = '36 36 2a 64 4b 4b 4a 21 1e 4b 1f 20 1f 21 4d 4b 1b 1d 19 4f 21 4c 1d 4a 4e 1c 4c 1b 22 4f 22 22 1b 21 4c 20 1d 4f 1f 4c 4a 19 22 1a 66'

c.split.each do |v|
  v = v.hex
  if v >= 0x0 && v <= 0x99
    putc (v - 0x2a + 'A'.ord).chr
  else
    putc '?'
  end
end

