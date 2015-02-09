Define Graph G, G2
Define Vertex v
Define Edge e1, e2, e3

For v in G:
Begin
  Let G2 be G2 + v
End

For e1 in G:
Begin
  For e2 in G:
  Begin
    If e1's dest's int = e2's source's int:
    Begin
      Let e3 be [e1's source, e2's dest]
      Let G2 be G2 + e3
    End
  End
End
