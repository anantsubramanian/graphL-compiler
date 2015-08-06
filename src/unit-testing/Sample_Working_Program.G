Global Int x
Global Int y
Global Float a, b

Define Int z1, i, z2
Define Int outeriter
Define Vertex u, v, w
Define String p

Define Tree T

Let x be 50
Let y be 5
Let i be 0
Let z1 be 2 + ( 3 * 5 )
Let z2 be ( 2 + 3 ) * 5
Let outeriter be 0

# Implicit type conversion check
# a is of type Float
Let a be 2 + ( 10 - y )
Print ( i )

Let T's root be u
Let T's root's String be "This is T's root's String!"

Print ( z1 )
Print ( z2 )
Print ( "" )

Print ( T's root's String )
Print ( y )

# Should print the floating point result of 2 + (10 - y)
Print ( a )
Print ( "" )
Print ( "Starting loops:" )
Print ( "" )

For y iterations:
Begin

  For 10 iterations:
  Begin

    # Will use the efficient inc instruction
    Let i be 1 + i
    If i + ( 1 - 2 ) < 5:
    Begin
      Print ( "This works!" )
    End
    Else:
    Begin
      Let x be 2 + y - (i + y)
      Print ( x )
      Print ( i )
    End

  End

  # Will use inc instead of add
  Let outeriter be 1 + outeriter

  If outeriter >= 3 :
  Begin
    Print ( "Outer iter greater than 2 now" )
  End

End

