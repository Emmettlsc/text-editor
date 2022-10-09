#ifndef SUNDO_H_
#define SUNDO_H_

#include "Undo.h"
#include <stack>

class SUndo : public Undo {
public:
    SUndo () {  }
    void submit(Action action, int row, int col, char ch = 0);
    Action get(int& row, int& col, int& count, std::string& text);
    void clear();

private:
    struct undointructions{
        Action mAction;
        std::string mbatch;
        int mrow, mcol, mcount;
    };
    std::stack<undointructions> UndoStack;
};

#endif // SUNDO_H_
