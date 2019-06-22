#ifndef HW3_ASSEMBLER_CODER_HPP
#define HW3_ASSEMBLER_CODER_HPP
#include "bp.hpp"
using namespace std;
#define WORD_SIZE 4
#define DIV_BY_ZERO_LABEL "div_by_zero_error"

class AssemblerCoder{

public:
    static AssemblerCoder& getInstance(){
        static AssemblerCoder INSTANCE;
        return INSTANCE;
    }
    static string to_string(int num){
        stringstream ss;
        ss<< num;
        return ss.str();
    }

    void emitStringToData(string label,string str){
         CodeBuffer::instance().emitData(label+" .asciiz \""+str+"\"");
    }

    void emitStringToDataForString(string label,string str){
        CodeBuffer::instance().emitData(label+": .asciiz "+str);

    }

    int lw(string destReg,int offset,string address="$fp"){
        return CodeBuffer::instance().emit("lw "+destReg+", "+to_string(offset)+"("+address+")");
    }

    int sw(string srcReg,int offset,string address){
        return CodeBuffer::instance().emit("sw "+srcReg+", "+to_string(offset)+"("+address+")");
    }

    int li(string destReg,int immediate){
        return CodeBuffer::instance().emit("li "+destReg+", "+to_string(immediate));
    }

    int la(string destReg,string label){
        return CodeBuffer::instance().emit("la "+destReg+", "+label);
    }

    int mul(string destReg,string reg1,string reg2){
        return CodeBuffer::instance().emit("mul "+destReg+", "+reg1+", "+reg2);
    }

    int div(string destReg,string reg1,string reg2){
        beq(reg2,"$0",DIV_BY_ZERO_LABEL);
        return CodeBuffer::instance().emit("div "+destReg+", "+reg1+", "+reg2);
    }

    int move(string destReg,string srcReg){
        return CodeBuffer::instance().emit("move "+destReg+", "+srcReg);
    }

    int subu(string destReg,string reg1,string reg2){
        return CodeBuffer::instance().emit("subu "+destReg+", "+reg1+", "+reg2);
    }

    int subu(string destReg,string reg1,int immediate){
        return CodeBuffer::instance().emit("subu "+destReg+", "+reg1+", "+to_string(immediate));
    }

    int addu(string destReg,string reg1,string reg2){
        return CodeBuffer::instance().emit("addu "+destReg+", "+reg1+", "+reg2);
    }

    int andi(string destReg,string reg,int immediate){
        return CodeBuffer::instance().emit("andi "+destReg+", "+reg+", "+to_string(immediate));
    }

    int addu(string destReg,string reg1,int immediate){
        return CodeBuffer::instance().emit("addu "+destReg+", "+reg1+", "+to_string(immediate));
    }

    int bne(string reg1,string reg2,string label=""){
        return CodeBuffer::instance().emit("bne "+reg1+", "+reg2+", "+label);
    }

    int bge(string reg1,string reg2,string label=""){
        return CodeBuffer::instance().emit("bge "+reg1+", "+reg2+", "+label);
    }

    int bgt(string reg1,string reg2,string label=""){
        return CodeBuffer::instance().emit("bgt "+reg1+", "+reg2+", "+label);
    }

    int ble(string reg1,string reg2,string label=""){
        return CodeBuffer::instance().emit("ble "+reg1+", "+reg2+", "+label);
    }

    int blt(string reg1,string reg2,string label=""){
        return CodeBuffer::instance().emit("blt "+reg1+", "+reg2+", "+label);
    }

    int beq(string reg1,string reg2,string label=""){
        return CodeBuffer::instance().emit("beq "+reg1+", "+reg2+", "+label);
    }

    int j(string label = ""){
        return CodeBuffer::instance().emit("j "+label);
    }

    int jal(string label){
        return CodeBuffer::instance().emit("jal "+label);
    }

    int jr(string reg = "$ra"){
        return CodeBuffer::instance().emit("jr "+reg);
    }

    void exitSyscall(){
        li("$v0",10);
        CodeBuffer::instance().emit("syscall");
    }

    void printSyscall(int framePinterOffset){
        lw("$a0",framePinterOffset,"$fp");
        li("$v0",4);
        CodeBuffer::instance().emit("syscall");
    }

    void printiSyscall(int framePinterOffset){
        lw("$a0",framePinterOffset,"$fp");
        li("$v0",1);
        CodeBuffer::instance().emit("syscall");
    }

    void comment(string str){
        CodeBuffer::instance().emit("#"+str);
    }

    void addLable(string label){
        CodeBuffer::instance().emit(label+":");
    }

    void exit(){
        li("$v0",10);
        CodeBuffer::instance().emit("syscall");
    }

    string genDataLabel(){
        static int labelCounter=0;
        labelCounter++;
        string label="dataLabel_"+to_string(labelCounter);
        //CodeBuffer::instance().emitData(label+":");
        return label;
    }





};
#endif //HW3_ASSEMBLER_CODER_HPP
