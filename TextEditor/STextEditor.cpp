#include "STextEditor.h"
#include "Undo.h"
#include <iostream>
#include <fstream>


TextEditor* createTextEditor(Undo* un)
{
    return new STextEditor(un);
}

STextEditor::STextEditor(Undo* undo)    //O(1)
 : TextEditor(undo) {
        mrow = mcol = 0;
     mlines.push_back("");  //when nothing loaded, this prevents out of bounds access
     currline = mlines.begin();
}

STextEditor::~STextEditor()     //O(N)
{
    mlines.clear();
}

bool STextEditor::load(std::string file) { //O(M+N+U)
    std::ifstream infile(file);
    if (!infile)
        return false;
    reset();    //O(M+U)
    std::string temp;
    if(infile.peek() == std::ifstream::traits_type::eof()){ //returns true if empty
        temp = "";
        mlines.push_back(temp);
    }
    else {  //only if it is not empty will the lines be added
        while(std::getline(infile, temp)){  //O(N)
            if(temp.size()>1 && temp[temp.size()-1]=='\r'){  //removes RC
                temp.pop_back();
            }
            mlines.push_back(temp); //adds lines to text editor
        }
    }
    mrow = mcol = 0;
    currline = mlines.begin();
    return true;
}

bool STextEditor::save(std::string file) {    // O(M) M=num of char in editor
    std::ofstream outfile(file);
    if(!outfile)
        return false;
    std::list<std::string>::iterator it = mlines.begin();
    while(it!=mlines.end()){    //saves each line to target file
        outfile<<*it<<'\n';
        it++;
    }
    return true;
}

void STextEditor::reset() { // O(N+U) U=num items in undo stack
    mrow = mcol = 0;
    getUndo()->clear(); //clears undo stack
    mlines.clear();
    currline = mlines.begin();
}

void STextEditor::move(Dir dir) {  // O(1)
    if(dir == TextEditor::Dir::UP && mrow!=0){
        mrow--;
        currline--;
        if(mcol > static_cast<int>(currline->size()))
            mcol =static_cast<int>(currline->size());
    }
    else if(dir == TextEditor::Dir::DOWN && mrow < mlines.size()-1){    //down
        mrow++;
        currline++;
        if(mcol > static_cast<int>(currline->size()))
            mcol =static_cast<int>(currline->size());
    }
    else if(dir == TextEditor::Dir::LEFT && mcol!=0){       //left
        mcol--;
    }
    else if(dir == TextEditor::Dir::LEFT && mrow!=0){
        currline--;
        mcol = static_cast<int>(currline->size());
        mrow--;
    }
    else if(dir == TextEditor::Dir::RIGHT && mcol!=static_cast<int>(currline->size())){     //right
        mcol++;
    }
    else if(dir == TextEditor::Dir::RIGHT && mrow!=mlines.size()-1){
        currline++;
        mcol = 0;
        mrow++;
    }
    else if(dir == TextEditor::Dir::HOME){      //home
        mcol = 0;
    }
    else if(dir == TextEditor::Dir::END){       //end
        mcol = static_cast<int>(currline->size());
    }
    
}

void STextEditor::del() { // O(L)
    std::list<std::string>::iterator temp = currline;
    temp++;
    if(mcol != currline->size()){
        char tempchar = (*currline)[mcol];
        currline->erase(mcol, 1);
        getUndo()->submit(Undo::Action::DELETE, mrow, mcol, tempchar);
    }
    else if(temp != mlines.end()){//checks if at end of doc
        std::string s1, s2;
        s1 = *currline; //sets s1 to line that will be deleted
        currline++; //moves currline to next line
        s2 = *currline; //sets s2 to line that will be merged
        currline--;
        currline = mlines.erase(currline);
        *currline = s1 + s2;
        getUndo()->submit(Undo::Action::JOIN, mrow, mcol, '\n');
    }

}

void STextEditor::backspace() {       // O(L) or O(L1 + L2)
    if(mcol > 0){   //if only char removed
        char temp = (*currline)[mcol-1];
        currline->erase(mcol-1, 1);
        mcol--;
        getUndo()->submit(Undo::Action::DELETE, mrow, mcol, temp);
    }
    else if(currline == mlines.begin() && mcol == 0) //if not viable
        return;
    else{   //joins lines
        std::string s1, s2;
        s2 = *currline; //sets s1 to line that will be deleted
        currline--; //moves currline to next line
        s1 = *currline; //sets s2 to line that will remain
        currline = mlines.erase(currline);
        *currline = s1 + s2;
        mrow--;
        mcol = static_cast<int>(s1.size());
        getUndo()->submit(Undo::Action::JOIN, mrow, mcol, '\n'); //submits after mrow changes
    }
}

void STextEditor::insert(char ch) {   //needs O(L) where L is length of line of text
    if(ch != '\t') {
        currline->insert(mcol, 1, ch);  //insert(size_type pos, size_type n, char c)
        mcol++;
        getUndo()->submit(Undo::Action::INSERT, mrow, mcol, ch);
    }
    else {
        currline->insert(mcol, 4, ' ');
        for(int i=0;i<4;i++){
            mcol++;
            getUndo()->submit(Undo::Action::INSERT, mrow, mcol, ' ');}
    }
}

void STextEditor::enter() {     // O(L) L= length of line of text
    std::string s1, s2;
    if(mcol==0)
        s1 = "";
    else
        s1 = currline->substr(0,mcol);
    
    s2 = currline->substr(mcol);
    *currline = s2;
    mlines.insert(currline, s1);
    getUndo()->submit(Undo::Action::SPLIT, mrow, mcol, '\n');           //only for enter does submit happen before change
    mrow++;
    mcol = 0;
}

void STextEditor::getPos(int& row, int& col) const {  // O(1)
    row = mrow; col = mcol;
}

int STextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
    if(startRow<0 || numRows<0 || startRow>mlines.size())
        return -1;
    lines.clear();
    int totRows;
    int mlinessize = static_cast<int>(mlines.size());
    if(startRow + numRows>mlinessize){
        totRows = mlinessize - startRow;
    }
    else
        totRows = numRows;
    int tempmrow = mrow;
    std::list<std::string>::const_iterator it = currline;             //sets iterator to currline to meed big O requirement
    while(tempmrow!=startRow){
        if(tempmrow>startRow){
            it--;
            tempmrow--;
        }
        else{
            it++;
            tempmrow++;
        }
    }
    int count = 0;
    while(count!=totRows){
        if(it == mlines.end())      //fills lines with editor's lines
            break;
        lines.push_back(*it);
        count++;
        it++;
    }
    return totRows;
}

void STextEditor::undo() {
    Undo::Action Action_;
    int row_, col_, count_;
    std::string text_;
    Action_ = getUndo()->get(row_, col_, count_, text_);
    if(Action_ == Undo::Action::ERROR)
        return;
    while(mrow!=row_){  //sets currline to correct undo location
        if(mrow>row_){
            mrow--;
            currline--;
        }
        else{
            mrow++;
            currline++;
        }
    }
    if(Action_ == Undo::Action::INSERT){        //adds string to line at passed in colum
        (*currline).insert(col_, text_);
        mcol = col_;
    }
    else if(Action_ == Undo::Action::DELETE){
        (*currline).erase(col_-1, count_);//note that .erase() begins erasing char at the pos passed in and submit gives pos after insert takes place
        mcol = col_-1;
    }
    else if(Action_ == Undo::Action::SPLIT){        //same as enter
        mcol = col_;
        mrow = row_;
        std::string s1, s2;
        if(mcol==0)
            s1 = "";
        else
            s1 = currline->substr(0,mcol);
        s2 = currline->substr(col_);
        *currline = s2;
        mlines.insert(currline, s1);
        currline--;
    }
    else if(Action_ == Undo::Action::JOIN){ //same as del when joining lines
        std::string s1, s2;
        mcol = col_;
        mrow = row_;
        s1 = *currline; //sets s1 to line that will be deleted
        currline++; //moves currline to next line
        s2 = *currline; //sets s2 to line that will remain
        currline--;
        currline = mlines.erase(currline);
        *currline = s1 + s2;
    }
        return;
}
