# skfr2
fast sudoku solver second version
Sudoku Explainer has been long ago the first code solving all sudokus with an attractive set of rules

Good sequence of hardness
Not much depending on the morph of the puzzle
…

This was enough to use it as a kind of reference to the hardness of a sudoku.

Meantime, plenty of logic ways to solve a puzzle have been used, but the rating given by Sudoku Explainer remains a first “sorting” filter.

One problem for users working on big files of puzzles is that SE is very slow. Solving one very hard puzzle can take hours.

Sudoku Fast Rating has been written 15 years ago to do nearly the same rating, but faster.

This code is now obsolete for many reasons and still too low to investigate some specific areas of logic.

The version 2 of sudoku fast rating should lead to a much better code.

The basic idea remains to be as close as possible to a clone of Sudoku Explainer.

The final expected rating is the same except in a very limited list of situations;
The expected deviation is usually 0.1.

So far, the known situations leading to a deviation are:

A UR oddity in SE where a UR hidden set has a different rating depending on a parity count having nothing to do with the logic,

A possible deviation in the processing of interleaved URs. As SE, SKFR2 does not work on degenerated (partly solved) URs.

Use of the Y chains logic in SKFR2 to be in line with X chains and XY chains.

A miss of some shorter expansion paths in SE in “dynamic mode” (including Nishio, the “one digit” dynamic mode)

In high ratings, another source of deviation is the limitation in the search.

As much as possible, the solving strategy tries to produce the same rating for all morphs.

The release of code starts far from the end of the task. SE rating goes from 1.0 to 11.9 (known) At the first release of code, SLFR2 is limited to the rating 7.5. Ratings up to 8.5 should come soon, higher rating later.

This code will have an entry for sukaku puzzles. At least one user of skfr did changes to rate sukakus,but faced crashes of the code for some hard sukakus. 

The code is released in draft mode, with still many tests to run and optimization of the process after the “hardest step not done”.

This is in line with things I have done in the past to share the burden if anybody is willing to do it.

If nobody comes, the next steps will take more time, but I hope to come to the end in the next months.

On my side, this DLL will open the door for more research in the field of “n digits end for a sudoku”. This field has shown thousands of very hard puzzles, difficult to see and sort out. 
