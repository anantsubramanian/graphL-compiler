Global Vertex v, w, farthest
Global Tree T
Global Int maximum

Function diameter() -> Nothing:
Begin

  Let maximum be 0

  For w in BFT(T,v):
  Begin
    If Depth > maximum:
    Begin
      Let maximum be Depth
      Let farthest be w
    End
  End

  Let maximum be 0

  For w in BFT(T,farthest):
  Begin
    Let maximum be Depth
  End
  return Nothing
End

