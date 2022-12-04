### Earley algorithm implementation

This CLI runner can determine if the given words are producible from the given formal context-free grammar.

#### Grammar syntax

- Terminal symbols - English lowercase letters
- Non-terminal symbols - English uppercase letters
- Rule must have the following syntax: `A => AbObA` - non-terminal symbol on the left-hand side, sequence of terminal and non-terminal symbols on the right-hand side
- To denote empty right-hand side `.` must be used (e.g. `X => .`)
- Only one rule must be present on the line
- Starting non-terminal symbol is `S`
- Initial rule (with `S` on the LHS) must have single non-terminal symbol on the right-hand side (e.g. `S => X`)
- Exactly one initial rule must be present

#### Input format
Grammar rules must respect the syntax described above. For the rest just follow the directions from the runner. 
