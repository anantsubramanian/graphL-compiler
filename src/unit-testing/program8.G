# This program test String literal errors

Define String a, b, c
Let a be "This is a correct \t string\n"

Let b be "This is an incorrect one
Let a be "A correct one again"

If c = "Not a valid escape \b character":
Begin
  Let a be "Again not a valid #Test escape \a"
End

Let b be "Not a correct \k escape and an incorrect quote

Let d be "Valid escaped \"Quotes\" inside a \r quote"

# End of the program

