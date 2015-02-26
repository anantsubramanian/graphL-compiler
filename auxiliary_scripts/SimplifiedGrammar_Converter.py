with open('Grammar.txt') as inp:
	with open('out.txt', 'w') as out:
		for line in inp:
			if (len(line.strip()) == 0):
				continue
			line = [ w for w in line.strip().split() if len(w.strip()) > 0 ]
			
			for i in range(0,len(line)):
				if(line[i] == '|'):
					out.write("\n" + line[0] + " -> ")
					continue
				else:
					out.write(line[i] + " ")
			out.write("\n")

out.close()
inp.close()
