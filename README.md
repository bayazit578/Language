SYNTAX OF MY LANGUAGE:

G               := Func_def* EOF

Func_def        := I '{' Param '}' ';' CompoundST ';'

Param           := [I ['.' I]* ]?

CompoundST      := ST*

ST              := SimpleST ')' | StructuredST | Func_call

SimpleST        := A | BREAK | CONTINUE

StructuredST    := W | IF | FOR | DO

Func_call       := I '{' Param '}' ')'

W               := if '{' E '}' ';' CompoundST ';'::

IF              := while '{' E '}' ';' CompoundST ';' ELSE_IF

ELSE_IF         := { for '{' E '}' ';' CompoundST ';' }* ELSE?

ELSE            := do ';' CompoundST ';'

FOR             := else if '{' FOR_COND '}' ';' CompoundST ';'

FOR_COND        := A? ')' E? ')' A?

DO              := else ';' CompoundST ';' if '{' E '}' ')'

A               := I '=' E

BREAK           := break

CONTINUE        := continue

E               := AND { '||' AND }*

AND             := COMP { '&&' COMP }*

COMP            := ARITH { ['>' '<' '>=' '<=' '==' '!='] ARITH }*

ARITH           := T { ['+' '-'] T }*

T               := F { ['*' '/' '%'] F }*

F               := I | N | '{' E '}'
