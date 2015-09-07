#!/usr/bin/env ruby

require './ctfutils'

c = File.read('cannotberun.exe')
c[0x3c] = 0xe8.chr

print c
