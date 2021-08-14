# Brainfuck compiler- C++ template edition

This project is aimed at implementing a [Brainfuck](https://en.wikipedia.org/wiki/Brainfuck) compiler in C++ which compiles and runs brainfuck programs at compile time.

# StateMachine class
The implementation is based on `StateMachine<NMoves, Program, SMBase>` class. It holds program state (memory tape, output tape, current tape index etc.) after executing `NMoves`  `Program` instructions on `SMBase` (initial `StateMachine` state).  

# Defining programs
Each `Program` class should have following static data members:
* `static constexpr std::array<char, N> program` being space-terminated of brainfuck characters
* `static constexpr NMoves` being an upper bound of instructions needed to finish the program

Additionally, if `program` contains `,`, `Program` should contain `static constexpr std::array<int, N> input` member, which will be used to feed the program during its execution. 

To define programs, which run on the results of another programs, use provided `Run<SM, Program>` class, which runs `Program` on state defined by `SM`.

# Examples
```
struct EmptyProgram {
  static constexpr std::array program = {' '};
  static constexpr int NMoves = 0;
};
using EmptyStateMachine = StateMachine<0, EmptyProgram>;
```
```
// Feeds one integer onto machine tape
template <int N>
struct Input {
  static constexpr std::array program = {',', ' '};
  static constexpr std::array input = {N};
  static constexpr int NMoves = 1;
};
using StateMachineWithA3OnTape = Run<EmptyStateMachine, Input<3>>;
std::cout << StateMachineWithA3OnTape::tape[0]; // 3
```
```
// Moves tape pointer to the right N times
template <int N>
struct MoveRight {
  static constexpr std::array program = []() {
    std::array<char, N + 1> ret;
    ret.fill('>');
    ret[N] = ' ';
    return ret;
  }();
  static constexpr int NMoves = N;
};
using SMWithTwoInts = Run<StateMachineWithA3OnTape , MoveRight<1>>, Input<4>>;
```
```
// Adds values of the current tape cell and cell to the left.
// Stores the result in the cell to the left
template <int RHS>
struct Add {
  static constexpr std::array program = {'[', '<', '+', '>', '-', ']', ' '};
  static constexpr int NMoves = 6 * RHS;
};
using SMSum = Run<SMWithTwoInts, Add<SMWithTwoInts::tape[SMWithTwoInts::tape_index]>>;
std::cout << StateMachineWithA3OnTape::tape[0]; // 7