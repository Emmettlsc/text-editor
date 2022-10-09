#ifndef STextEdITOR_H_
#define STextEdITOR_H_

#include "TextEditor.h"
#include <list>


class Undo;

class STextEditor : public TextEditor {
public:

    STextEditor(Undo* undo);
    ~STextEditor();
    bool load(std::string file);        //need to show text
    bool save(std::string file);
    void reset();
    void move(Dir dir);
    void del();
    void backspace();
    void insert(char ch);
    void enter();
    void getPos(int& row, int& col) const;
    int getLines(int startRow, int numRows, std::vector<std::string>& lines) const;     //need to show text
    void undo();

private:
    //the lines of text being edited
    std::list<std::string> mlines;
    
    //current editing position
    int mrow, mcol;
    
    //iterator to current line
    std::list<std::string>::iterator currline;
};

#endif // STextEdITOR_H_
