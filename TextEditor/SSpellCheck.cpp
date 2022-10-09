#include "SSpellCheck.h"
#include <iostream>
#include <fstream>
SpellCheck* createSpellCheck()
{
    return new SSpellCheck;
}

SSpellCheck::~SSpellCheck() {
    freeNodes(root);
}

bool SSpellCheck::load(std::string dictionaryFile) {  //O(N)
    root = new TrieNode;
    initializeTrieNode(root);
    std::ifstream infile(dictionaryFile);
    if (!infile)
        return false;
    std::string temp, onlyvalidchar;
    while(std::getline(infile, temp)){  //O(N)
        if(temp!="") //prevents errors with there being 27 children
            insertwordintotrie(root, temp); //adds word to dictionary
    }
    return true;
}

bool SSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
    if(searchTrie(word))    //returns true if found
        return true;
    suggestions.clear();  //prepares vec for suggestions
    getsuggestions(word, max_suggestions, suggestions); //this function adds suggestions to vector
    return false;
}

void SSpellCheck::getsuggestions(std::string word, int max_suggestions, std::vector<std::string>& suggestions){
    int i=0;
    int stop = 0; //keeps track of max suggestions
    std::string copyword = word;
    for(;i<word.size();i++){    //iterates through each index in string of word passed in
        for(int ii=0;ii<26;ii++){   //changes index with every alpha
            copyword.erase(copyword.begin()+i);
            copyword.insert(i, 1, 'a' + ii);
            if(searchTrie(copyword)){   //if word, add to vector
                if(stop>=max_suggestions)
                    return;
                suggestions.push_back(copyword);
                stop++;
            }
        }
        copyword.erase(copyword.begin()+i); //tests for ' char
        copyword.insert(i, 1, '\'');
        if(searchTrie(copyword)){
            if(stop>=max_suggestions)
                return;
            suggestions.push_back(copyword);
            stop++;
        }
        copyword = word;    //resets base word for next index
    }
}

void SSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {  //O(oldP + S + W*L)
    int runner = 0; //tracks start pos
    problems.clear();
    std::string temp = "";
    for(int i=0; i<=line.size(); i++){
        if(isalpha(line[i]) || line[i] == '\''){    //if correct letter, add to word
            temp+=std::tolower(line[i]);
            runner++;
        }
        else{
            if(temp!= "" && !searchTrie(temp)){ //adds pos of incorrect word to vector
                SpellCheck::Position pos;
                pos.end = i-1;
                pos.start = i-runner;
                problems.push_back(pos);
            }
            temp = "";
            runner = 0;
        }
    }
}

void SSpellCheck::insertwordintotrie (TrieNode* root, std::string insert){
    TrieNode* currlevel = root;
    for(int i=0; i<insert.size();i++){
        if(insert[i]=='\'' && currlevel->children[26]==nullptr){    //checks if no child for '
            currlevel->children[26] = new TrieNode;
            initializeTrieNode(currlevel->children[26]);
            currlevel = currlevel->children[26];
        }
        else if(insert[i]=='\'')        //if child for ' then sets currlevel down a lvl
            currlevel = currlevel->children[26];
        else if(isalpha(insert[i]) && currlevel->children[std::tolower(insert[i])-'a'] == nullptr){ //checks if no child for alpha
            currlevel->children[std::tolower(insert[i])-'a'] = new TrieNode;
            initializeTrieNode(currlevel->children[std::tolower(insert[i])-'a']);
            currlevel = currlevel->children[std::tolower(insert[i])-'a'];
        }
        else if(isalpha(insert[i])){    //if child for alpha then sets currlevel down a lvl
            currlevel = currlevel->children[std::tolower(insert[i])-'a'];
        }
    }
    currlevel->wordend = true;
}

void SSpellCheck::initializeTrieNode (TrieNode* target){
    for(int i=0; i<27; i++){
        target->children[i] = nullptr;
    }
    target->wordend = false;
}

bool SSpellCheck::searchTrie(std::string check){  //returns true if found
    TrieNode* currlevel = root; //goes do levels to find end of word
    int i = 0;
    for(;i<check.size();i++){
        if(check[i] == '\'')    //checks if child exists
            currlevel = currlevel->children[26];
        else if(isalpha(check[i]))
            currlevel = currlevel->children[std::tolower(check[i])-'a'];
        else
            currlevel = nullptr;
        if(currlevel == nullptr)    //if ever nullptr then word not in dictionary
            return false;
    }
    if(currlevel!=nullptr && currlevel->wordend)
        return true;
    return false;
}

void SSpellCheck::freeNodes(TrieNode* killmychildren){
    if(killmychildren == nullptr)
        return;
    for(int i=0; i<27; i++){    //kills all children
        freeNodes(killmychildren->children[i]);
    }
    delete killmychildren;
}

