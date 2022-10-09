#include "SUndo.h"

Undo* createUndo()
{
    return new SUndo;
}

void SUndo::submit(const Action action, int row, int col, char ch) {
    if(!UndoStack.empty() && UndoStack.top().mAction == action && action == Action::DELETE && UndoStack.top().mcol == col){ //for del
        UndoStack.top().mbatch+=ch;
        return;
    }
    else if(!UndoStack.empty() && UndoStack.top().mAction == action && action == Action::DELETE && UndoStack.top().mcol-1 == col){      //for backspace
        UndoStack.top().mbatch.insert(0,1,ch);
        UndoStack.top().mcol = col;
        return;
    }
    else if(!UndoStack.empty() && UndoStack.top().mAction == action && action == Action::INSERT && UndoStack.top().mcol+UndoStack.top().mcount == col){  //for insert
        UndoStack.top().mcount++;
    }
    else {
        undointructions temp;   //sets undoinstruction to passed in vals and adds to stack
        temp.mAction = action;
        temp.mcount = 1;
        temp.mrow = row;
        temp.mcol = col;
        temp.mbatch = ch;
        UndoStack.push(temp);
    }
}

SUndo::Action SUndo::get(int& row, int& col, int& count, std::string& text) {   //O(1)
    if(UndoStack.empty())   //returns if empty
        return Action::ERROR;
    row = UndoStack.top().mrow;
    col = UndoStack.top().mcol;
    text = "";
    if(UndoStack.top().mAction==Action::INSERT) //only changes count if Seditor needs to delete chars
        count = UndoStack.top().mcount;
    else
        count = 1;
    if(UndoStack.top().mAction==Action::INSERT){
        UndoStack.pop();
        return Action::DELETE;}
    if(UndoStack.top().mAction==Action::DELETE){
        text = UndoStack.top().mbatch;
        UndoStack.pop();
        return Action::INSERT;
    }
    if(UndoStack.top().mAction==Action::SPLIT){
        UndoStack.pop();
        return Action::JOIN;}
    if(UndoStack.top().mAction==Action::JOIN){
        UndoStack.pop();
        return Action::SPLIT;}
    return Action::ERROR;
}

void SUndo::clear() {  //O(N)
    while(!UndoStack.empty()){  //pops every element in stack
        UndoStack.pop();
    }
}
