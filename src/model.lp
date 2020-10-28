\ENCODING=ISO-8859-1
\Problem name: IloCplex

Minimize
 obj1: x1 + x2
Subject To
 c1: x3 >= 1
 c2: 101.07209777832 x1 - 101.07209777832 x3 - 1056 x4  = 0
 c3: 101.07209777832 x1 - 1056 x2 - 101.07209777832 x3  = 0
 c4: x1 - x3 >= 2.112
 c5: x1 - x3 <= 1056
Bounds
 0 <= x1 <= 1000000
 0 <= x2 <= 1000000
 0 <= x3 <= 1000000
 0 <= x4 <= 1000000
End
