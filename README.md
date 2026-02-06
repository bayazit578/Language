SYNTAX OF MY LANGUAGE:

G := CompoundST EOF

CompoundST := ST*

ST := SimpleST ')' | StructuredST

SimpleST := A | BREAK | CONTINUE

StructuredST := W | IF | FOR | DO

W := if '{' E '}' ';' CompoundST ';'

IF := while '{' E '}' ';' CompoundST ';' ELSE_IF 

ELSE_IF := { for '{' E '}' ';' CompoundST ';' }* ELSE?

ELSE := do ';' CompoundST ';'

FOR := else if '{' A? ')' E? ')' A? '}' ';' CompoundST ';'

DO := else ';' CompoundST ';' if '{' E '}' ')'

A := I '=' E

BREAK := break

CONTINUE := continue

E := AND { '||' AND }*

AND := COMP { '&&' COMP }*

COMP := ARITH { ['>' '<' '>=' '<=' '==' '!='] ARITH }*

ARITH := T { ['+' '-'] T }*

T := F { ['*' '/' '%'] F }*

F := I | N | '{' E '}'
