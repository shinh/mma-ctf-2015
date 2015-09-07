#!/usr/bin/env ruby

code_size = 41

cc = ''

cc += "##{code_size}"
t = "#35.:\\,#1-:!!#"
cc += t + "#{t.size+5}*~@_"

#cc += "##{code_size}"
#t = "#35.#1-:,#92.#35.#35,#46.#44.:!!#"
#cc += t + "#{t.size+5}*~@_"

cc += "##{code_size}"
t = "#1-^.:!!#"
cc += t + "#{t.size+5}*~@;"

code = cc
if code.size > code_size
  raise
end
while code.size < code_size
  code += ' '
end

c = ''
d = ''

code.reverse.each_byte{|b|
  raise if b == 0
  d += "##{b}"
}
c += d

#code_size.times{|i|
#  c += "##{code_size-i-1}"
#  c += "\\"
#  c += "#35.,"
#}

#c += cc

print c + code

__END__

code = 'dc'

c = '#0'
d = ''
code.each_byte{|b|
  d += "##{b}^:#999*@^#35.,"
}

puts c + d + code

__END__

code = '__:!#99*@#35.,dc'

c = '#1'
d = ''

code.reverse.each_byte{|b|
  d += "##{b}^"
}

c += d
c += "#^#^##{d.size+10}*~"
c += "@"

puts c + code
