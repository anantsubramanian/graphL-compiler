with open('Grammar.txt') as inp:
	with open('out.txt', 'w') as out:
		d = {}
		for line in inp:
			if (len(line.strip()) == 0):
				continue
			line = [ w for w in line.strip().split() if len(w.strip()) > 0 ]
			for w in line:
				if (w[0] == 'T'):
					out.write(w.lower() + ' ')
				elif (w[0] == '<'):
					out.write(w[1:-1].upper() + ' ')
				elif (w == 'e'):
					out.write(' ')
				else:
					out.write(w + ' ')
			out.write('.\n')

out.close()
inp.close()
