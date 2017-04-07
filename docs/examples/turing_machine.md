#<cldoc:Examples::Turing Machine>
## Code
```c++
#include "./deprecated_serializable.hpp"

#include <vector>
#include <string>

enum movements {
  left,
  right,
  stop
};

class Alphabet : public json::deprecated_serializable {
private:
  char empty;
  std::vector <char> alphabet;
  SERIAL_START
    empty,
    alphabet
  SERIAL_END
public:
  bool checkSymbolExists (char symbol) {
    for (char c : alphabet)
      if (c == symbol)
        return true;
    if (symbol == empty)
      return true;
    else
      return false;
  }
  const char getEmpty () { return empty; }
};

class Tape : public json::deprecated_serializable {
private:
  std::vector <char> tapeRight;
  std::vector <char> tapeLeft;
  Alphabet* alphabet;
  int head;
  SERIAL_START
    tapeRight
  SERIAL_END
public:
  inline char read () { return getSymbolAt(head); }
  inline void write (char symbol) {  getSymbolAt(head) = symbol; }

  bool setAlphabet (Alphabet* al) {
    alphabet = al;
    for (char c : tapeRight)
      if(!al->checkSymbolExists(c))
        return false;
    return true;
  }

  void setHead (int h) {
    head = 0;
    if (h > 0)
      for (int i = 0; i < h; i++)
        moveRight();
    else if (h < 0)
      for (int i = -h - 1; i >= 0; i--)
        moveLeft();
  }

  char& getSymbolAt (int pos) {
    if (pos < 0)
      return tapeLeft[-pos - 1];
    else
      return tapeRight[pos];
  }

  void move (int movement) {
    switch (movement) {
      case movements::left:
        moveLeft();
        break;
      case movements::right:
        moveRight();
        break;
    }
  }

  void moveLeft () {
    if (-head - 1 == tapeLeft.size() - 1) {
      tapeLeft.push_back(alphabet->getEmpty());
    }
    head--;
  }

  void moveRight () {
    if (head == tapeRight.size() - 1) {
      tapeRight.push_back(alphabet->getEmpty());
    }
    head++;
  }

  void print () {
    int min = (-tapeLeft.size()) * (tapeLeft.size() > 0);
    for (int i = min; i != tapeRight.size(); i++) {
      if (i == head)
        std::cout << "[" << getSymbolAt(i) << "] ";
      else
        std::cout << getSymbolAt(i) << " ";
    }
    std::cout << "\n\n\n";
  }
};

class Transition : public json::deprecated_serializable {
private:
  char innerSymbol;
  int movement;
  char outterSymbol;
  int nextState;

  SERIAL_START
    innerSymbol,
    movement,
    outterSymbol,
    nextState
  SERIAL_END
public:
  char& getInner () { return innerSymbol; }
  char& getOutter () { return outterSymbol; }
  int getMovement () { return movement; }
  int getNextState () { return nextState; }

  bool checkConsistent (int nStates, Alphabet* alphabet) {
    return alphabet->checkSymbolExists(innerSymbol)  &&
           alphabet->checkSymbolExists(outterSymbol) &&
           nextState <= nStates && nextState > -1;
  }
};

class State : public json::deprecated_serializable {
private:
  std::vector <Transition> transitions;
  bool acceptanceState;
  SERIAL_START
    "transitions", transitions,
    "acceptance", acceptanceState
  SERIAL_END
public:
  Transition* getTransitionForChar (char symbol) {
    for (int i = 0; i < transitions.size(); i++)
      if (transitions[i].getInner() == symbol)
        return &(transitions[i]);
    return nullptr;
  }
  bool isAcceptanceState () { return acceptanceState; }
  bool checkConsistency (int nStates, Alphabet* alphabet) {
    for (Transition t : transitions)
      if (!t.checkConsistent(nStates, alphabet))
        return false;
    return true;
  }
};

class TM : public json::deprecated_serializable {
private:
  std::vector <State> states;
  Alphabet alphabet;
  Tape* tape;
  int actualState;
  std::string explanation;
  int head;
  SERIAL_START
    "states",       states,
    "actual_state", actualState,
    "alphabet",     alphabet,
    "explanation",  explanation,
    "head",         head
  SERIAL_END
public:

  bool checkConsistency () {
    for (int i = 0; i < states.size(); i++)
      if(!states[i].checkConsistency(states.size(), &alphabet)) {
        std::cout << "The state " << i << " contains a bad defined transition" << std::endl;
        return false;
      }
    if (!tape->setAlphabet(&alphabet)) {
      std::cout << "Tape contains symbols that are not defined in alphabet" << std::endl;
      return false;
    }
    tape->setHead(head);
    return true;
  }

  void start (Tape* t) {
    tape = t;
    if (checkConsistency()) {
      std::cout << explanation << "\n\n";
      compute ();
      checkEnd();
    }
  }

  State* getStateAt (int i) {
    if (actualState >= states.size() || actualState < 0)
      return nullptr;
    return &states[i];
  }

  void compute () {
    while (1) {
      print();
      Transition* t = getStateAt(actualState)->getTransitionForChar(tape->read());
      if (t == nullptr)
        return;
      tape->write(t->getOutter());
      tape->move(t->getMovement());
      actualState = t->getNextState();
    }
  }

  void print () {
    std::cout << "Actual state: " << actualState << std::endl;
    tape->print();
  }

  void checkEnd () {
    print();
    if (states[actualState].isAcceptanceState())
      std::cout << "is accepted" << std::endl;
    else
      std::cout << "is not accepted" << std::endl;
  }
};

int main (void) {
  TM turingMachine;
  Tape tape;
  turingMachine.serializeIn("file.json", "M1");
  tape.serializeIn         ("file.json", "T1");
  turingMachine.start(&tape);
}
```
## File
```json
{
    "M1" : {
        "states" : [
            {
                "transitions" : [
                    ["a", 1, "b", 1],
                    ["b", 0, "a", 1],
                    ["c", 0, "d", 1]
                ],
                "acceptance" : false
            },
            {
                "transitions" : [
                    ["$", 1, "c", 0],
                    ["a", 1, "c", 0],
                    ["c", 3, "c", 2]
                ],
                "acceptance" : false
            },
            {
                "transitions" : [],
                "acceptance" : true
            }
        ],
        "alphabet" : [
            "$",
            [
                "a", "b", "c", "d"
            ]
        ],
        "actual_state" : 0,
        "explanation" : "Esta máquina de turing puede hacer muchas cosas",
        "head" : 0
    },
    "T1" : [
        ["a", "a", "a", "a", "a", "a", "c"]
    ],
    "T2" : [
        ["b", "b", "c", "a", "a", "a", "c"]
    ]
}
```
