#include <algorithm>
#include <array>
#include <iostream>

using Tape = std::array<int, 100>;
using Output = std::array<int, 1000>;

template <int NMoves, typename SMPrev, typename SMBase>
struct ChooseSMPrev {
  using type = SMPrev;
};
template <typename SMPrev>
struct ChooseSMPrev<0, SMPrev, void> {
  using type = SMPrev;
};
template <typename SMPrev, typename SMBase>
struct ChooseSMPrev<0, SMPrev, SMBase> {
  using type = SMBase;
};

template <int NMoves, typename Program, typename SMBase = void>
struct StateMachine {
  using SMPrev =
      typename ChooseSMPrev<NMoves - 1,
                            StateMachine<NMoves - 1, Program, SMBase>,
                            SMBase>::type;
  static constexpr int program_index = []() {
    if (Program::program[SMPrev::program_index] == ']' &&
        SMPrev::tape[SMPrev::tape_index] != 0) {
      // Find matching opening bracket to jump to
      int v = 0;
      for (int i = SMPrev::program_index - 1; i >= 0; --i) {
        if (Program::program[i] == ']')
          ++v;
        else if (Program::program[i] == '[') {
          if (v == 0)
            return i;
          --v;
        }
      }
    } else {
      return std::min<int>(SMPrev::program_index + 1,
                           Program::program.size() - 1);
    }
  }();
  static constexpr int tape_index = []() {
    return SMPrev::tape_index +
           (Program::program[SMPrev::program_index] == '>') -
           (Program::program[SMPrev::program_index] == '<');
  }();
  static constexpr int output_index = []() {
    return SMPrev::output_index +
           (Program::program[SMPrev::program_index] == '.');
  }();
  static constexpr int input_index = []() {
    return SMPrev::input_index +
           (Program::program[SMPrev::program_index] == ',');
  }();
  static constexpr Tape tape = [](Tape tape) {
    if constexpr (Program::program[SMPrev::program_index] == ',') {
      tape[SMPrev::tape_index] = Program::input[SMPrev::input_index];
    } else {
      tape[SMPrev::tape_index] =
          tape[SMPrev::tape_index] +
          (Program::program[SMPrev::program_index] == '+') -
          (Program::program[SMPrev::program_index] == '-');
    }
    return tape;
  }(SMPrev::tape);
  static constexpr Output output = [](Output output, Tape tape) {
    if (Program::program[SMPrev::program_index] == '.') {
      output[SMPrev::output_index] = tape[SMPrev::tape_index];
    }
    return output;
  }(SMPrev::output, SMPrev::tape);
};

template <typename Program>
struct StateMachine<0, Program> {
  static constexpr int tape_index = 0;
  static constexpr int program_index = 0;
  static constexpr int output_index = 0;
  static constexpr int input_index = 0;
  static constexpr Tape tape = {};
  static constexpr Output output = {};

  static_assert(Program::program[Program::program.size() - 1] == ' ');
};

struct EmptyProgram {
  static constexpr std::array program = {' '};
  static constexpr int NMoves = 0;
};
template <int RHS>
struct Add {
  static constexpr std::array program = {'[', '<', '+', '>', '-', ']', ' '};
  static constexpr int NMoves = 6 * RHS;
};
template <int N>
struct Input {
  static constexpr std::array program = {',', ' '};
  static constexpr std::array input = {N};
  static constexpr int NMoves = 1;
};
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

template <typename SM, typename Program>
struct Run {
  static constexpr int tape_index =
      StateMachine<Program::NMoves, Program, SM>::tape_index;
  static constexpr int program_index = 0;
  static constexpr int output_index =
      StateMachine<Program::NMoves, Program, SM>::output_index;
  static constexpr int input_index = 0;
  static constexpr Tape tape = StateMachine<Program::NMoves, Program, SM>::tape;
  static constexpr Output output =
      StateMachine<Program::NMoves, Program, SM>::output;

  static_assert(Program::program[Program::program.size() - 1] == ' ');
};

template <typename SM>
void printSM() {
  std::cout << SM::tape_index << ' ' << SM::program_index << ' '
            << SM::output_index << '\n';
  for (auto t : SM::tape)
    std::cout << t << ' ';
  std::cout << '\n';
  for (int i = 0; i < SM::output_index; ++i)
    std::cout << SM::output[i] << ' ';
  std::cout << '\n';
}

int main() {
  using SMInput =
      Run<Run<Run<StateMachine<0, EmptyProgram>, Input<2>>, MoveRight<1>>,
          Input<3>>;
  using SMAdd = Run<SMInput, Add<3>>;
  printSM<SMAdd>();
  return 0;
}
