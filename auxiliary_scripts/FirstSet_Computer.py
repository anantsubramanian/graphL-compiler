import sys

rules = []
terminals = {}
nonterminals = {}
firstsets = {}

if len(sys.argv) <= 1 :
  print ("Please enter the path of the file..")
  exit ()

with open(sys.argv[1]) as inp:
  for line in inp:
    if (len(line.strip()) == 0):
      continue
    line = [ w for w in line.strip().split() if len(w.strip()) > 0 ]

    nonterminals [ line[0] ] = True
    firstsets [ line[0] ] = set()
    rules += [ [ line[0], line [ 2: ] ] ]
    for token in line [ 2: ]:
      if token.startswith ( 'TK_' ) :
        terminals [ token ] = True

inp.close()

# First of a terminal is itself
for terminal in terminals:
  firstsets [ terminal ] = set ( [terminal] )

# Non terminals giving rules starting with terminals
for [ nonterminal, rule ] in rules:
  if rule [0] in terminals:
    firstsets [ nonterminal ] = firstsets [ nonterminal ] | set ( [ rule [0] ] )

# Non terminals giving epsilon
for [ nonterminal, rule ] in rules:
  if rule [0] == 'e' :
    firstsets [ nonterminal ] = firstsets [ nonterminal ] | set ( [ 'e' ] )

# Iterate over non terminals while there's no change
while True:
  changed = False
  for [ nonterminal, rule ] in rules:
    prev = len ( firstsets [ nonterminal ] )
    for token in rule:
      if token in terminals:
        firstsets [ nonterminal ] |= firstsets [ token ]
        break
      elif token == 'e':
        firstsets [ nonterminal ] |= set ( [ 'e' ] )
      else:
        # Token is a non-terminal
        firstsets [ nonterminal ] |= firstsets [ token ]
        if not ( 'e' in firstsets [ token ] ):
          break 
    if not ( len ( firstsets [ nonterminal ] ) == prev ) :
      changed = True

  if not changed:
    break

with open( 'FirstSets.txt', 'w' ) as firstout:
  for token in firstsets:
    firstout.write(token + " ")
    firstlist = list ( firstsets [token] )
    firstlist.sort()
    for token in firstlist:
      firstout.write(token + " ")
    firstout.write('\n')

firstout.close()

