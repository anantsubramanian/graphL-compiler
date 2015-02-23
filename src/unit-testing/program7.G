# This program tests Integer and Floating Point Literal errors
Define Int x, y, z
Define Float c

# These lines should show Floating Point errors
Let c be 0.2.3
Let c be 1 - 0.2.3.4
Let c be c + 2.3a
Let c be c * 2.3a+2
Let c be c / 2.3a4
Let c be 2.a3
Let c be x + 2.
Let c be y - 2.aa
Let c be z * 2.aa3
Let c be x / 2.+2
Let c be c + 2. + 4

If c > 0 :
Begin
  Let c be 1.g25 * c
End

# These lines show integer errors
Let x be y + z + 2a
Let y be 400a4
Let z be 434

