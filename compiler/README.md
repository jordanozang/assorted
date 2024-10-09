
Some of my explorations relating to compilers and languages.

Some things here:

- Simple Regex implementation (ascii)
    - Implemented a simple FSA class. It has support for concatenation, union, and closure operations via Thompson's constructions. Determinization is implemented to convert the resulting non-deterministic FSA to a determininstic FSA, which can then be used for testing input strings. Minimization is a TODO.
    - Wrote a lexer + recursive descent parser that converts regex expressions into FSAs in place.
    - Implemented Features
        - Closure operation (*)
        - Alternation operation (|)
        - Grouping expressions (())
        - Concatenation (xy -> concat(x,y))
    - TODO Features
        - Bind variables to sub-expressions. Planning to do this in the FSA by adding arcs with a new special grouping character.
        - Optional operator (?)
        - 1 or more of preceding (+)
        - n repetition of preceding ({n})
- Implementation of a very simple handwritten lexer and recursive descent parser laying the groundwork for future work in writing a compiler and/or interpreter.

