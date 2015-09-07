#!/usr/bin/env ruby

require './ctfutils'
require 'zlib'

if $prod
  pipe = popen('nc mix-nuts.chal.mmactf.link 12393')
else
  pipe = popen('python perfect-matching/server.py')
end

while true
  pipe.get
  len = pipe.get.to_i
  edges = []
  Zlib::Inflate.inflate(pipe.read(len)).each_line do |line|
    edges << line.split.map(&:to_i)
  end
  pipe.get

  num_nodes, num_edges = edges.shift
  puts "num_nodes=#{num_nodes} num_edges=#{num_edges}"

  used_nodes = {}
  matches = []
  edges.each do |n1, n2|
    nn1 = n1 == 0 ? 0 : n1 - num_nodes
    nn2 = n2 == 0 ? 0 : n2 - num_nodes
    [[nn1, n2], [nn2, n1], [n1, n2]].sort.each do |i1, i2|
      if !used_nodes[i1] && !used_nodes[i2]
        used_nodes[i1] = true
        used_nodes[i2] = true
        matches << [i1, i2]
        break
      end
    end
    break if matches.size == num_nodes / 2
  end

  if matches.size != num_nodes / 2
    puts "hmm #{matches.size} #{num_nodes}"
    raise
  end

  i = 0
  matches.each do |v1, v2|
    #STDERR.puts "rb: #{i}/#{matches.size} #{v1} #{v2}"
    pipe.puts "#{v1} #{v2}"
    i += 1
  end

  pipe.get

end

__END__

  nodes = {}
  edges.each_with_index do |n, i|
    n.each do |x|
      nodes[x] = [] if !nodes[x]
      nodes[x] << i
    end
  end

  matches = []

  p nodes.size

  nodes.each do |n, e|
    next if e.size != 1

    e = e[0]
    matches << edges[e]

    nodes.delete(n)
  end

  while !nodes.empty?
    p nodes.size

    n, e = nodes.min_by{|n, e|e.size}
    raise if e.empty?

    re = e[0]

    edges[re].each do |rn|
      puts "remove?: #{re} #{rn}"
      nodes[rn].each do |rre|
        edges[rre].each do |rrn|
          puts "remove edge: #{rre} from #{rrn}"
          nodes[rrn].delete(rre)
        end
      end
    end

    edges[re].each do |rn|
      puts "remove: #{rn}"
      nodes.delete(rn)
    end
  end


  exit
end
