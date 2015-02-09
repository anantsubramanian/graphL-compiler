# Assumes that the Graph is populated

Function shortestPath (Graph G, Vertex v, Vertex u) -> Int :
Begin
  Define Vertex w
  Define Int shortestDepth
  For w in BFT (G, v):
  Begin
    If w's String = u's String:
    Begin
      Let shortestDepth be Depth
      break
    End
  End
  return shortestDepth
End

Define Int x
Define Graph G
Define Vertex v, u

Let x be shortestPath(G, v, u)

Print(x)
