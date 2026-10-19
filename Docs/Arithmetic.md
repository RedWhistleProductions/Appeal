# Standard arithmetic

`Add A B`, `Subtract A B`, `Multiply A B`, and `Divide A B` are built into
Appeal's standard dictionary. No module or extension is required. Each takes
two numeric values and returns one result through the expression mechanism:

```text
Print (Add 2 3)
Print (Subtract 2 5)
Print (Multiply -2 3)
Print (Divide 5 2)
Set_Var X 176
Set_Var Speed 10
Set_Var X (Add Var X Var Speed)
Print (Multiply (Add 1 2) 4)
```

These print 5, -3, -6, 2.5, and 12. Subtraction and division use the first
operand minus/divided by the second. Variables and nested expressions work as
operands. Ordinary calls discard the result; use parentheses to capture it.

Calculations use double-precision floating point, including division. Decimal
and scientific notation are accepted. Results may have normal floating-point
rounding; integers beyond 2^53 are not guaranteed exact. Whole-number results
are formatted without a decimal point or exponent for integer command arguments
such as `Graphics Paste`. Fractional results remain fractional; these commands
do not introduce integer division or implicit rounding.

Invalid operands, nonfinite numbers/results, and division by zero report an
Appeal error rather than silently substituting zero. There is no infix operator
syntax or precedence change. More advanced math can be added in an extension.

The combined animation/music example uses these commands for independent Adam
and Eve movement controls. Each character tab has an animation selector, a speed
slider (1–50 pixels per click, initially 10), and four direction buttons. Up
subtracts speed from Y; down adds it. Left subtracts from X; right adds it.
Each click then calls Paste with that character's updated coordinates. Positions
are not clamped, so characters can be moved beyond the window edges.
