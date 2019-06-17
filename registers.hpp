#ifndef HW3_REGISTERS_HPP
#define HW3_REGISTERS_HPP
#define NUMBER_OF_REG (18)
#define TEMP_REG_END (10)
#define TEMP_REG_START (0)
#define STORED_REG_START TEMP_REG_END
#define STORED_REG_END NUMBER_OF_REG
#include <string>
using namespace std;
class Registers{
private:
    bool bitmap[NUMBER_OF_REG];
    string names[NUMBER_OF_REG];
    Registers():bitmap(),names(){
        for(int i=TEMP_REG_START;i<TEMP_REG_END;i++) {
            names[i] = "$t" + std::to_string(i);
            bitmap[i]=false;
        }
        for(int i=STORED_REG_START;i<STORED_REG_END;i++){
            names[i] = "$s" + std::to_string(i-STORED_REG_START);
            bitmap[i]=false;
        }
    }
public:
    static Registers& getInstance(){
        static Registers INSTANCE;
        return INSTANCE;
    }

    vector<string> getUsedRegisters(){
        vector<string> used;
        for(int i=0;i<NUMBER_OF_REG;i++) {
            if(bitmap[i]){
                used.push_back(names[i]);
            }
        }
        return used;
    }

    string regAlloc(){
        //look for the first free register to be used
        for(int i=0;i<NUMBER_OF_REG;i++){
            if(!bitmap[i]){
                bitmap[i]=true;
                return names[i];
            }
        }
        throw std::bad_alloc(); //all registers are used
    }

    Registers& regFree(string& name){
        for(int i=0;i<NUMBER_OF_REG;i++){
            if(names[i]==name)
                bitmap[i]= false;
        }
        name="";
    }

};

#endif //HW3_REGISTERS_HPP
