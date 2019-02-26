/*
* On my honor, I have neither given nor received unauthorized aid on this assignment
*/

#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

void LoadRegistersOrDataMemory(vector<int> &arr, const char *fileName){
    string line;
    ifstream file(fileName);

    if(file.is_open()){
        while(getline(file, line)){

            // In the register file values are written as <R0,15>
            // In the data memory file values are written as: <1,4>
            bool foundComma = false;
            int value = 0;
            for(int i = 0; i < line.length(); i++){
                if(line[i] >= '0' && line[i] <= '9' && foundComma){
                    value *= 10;
                    value += line[i] - '0';
                }

                if(line[i] == ',') foundComma = true;
            }
            arr.push_back(value);
        }

        file.close();
    }
}

void LoadInstructions(vector<string> &arr, const char *fileName){
    string line;
    ifstream file(fileName);

    if(file.is_open()){
        while(getline(file, line)){
            arr.push_back(line);
        }
        file.close();
    }
}

vector<string> ParseInstruction(string instruction){
    string temp;
    vector<string> out;

    for(int i = 0; i < instruction.length(); i++){
        if(instruction[i] == '<' || instruction[i] == '>') continue;
        else if(instruction[i] == ','){
            out.push_back(temp);
            temp = "";
        }
        else temp += instruction[i];
    }

    if(temp != "") out.push_back(temp);

    return out;
}


void UpdateInstructionBuffer(vector<string> &instructions, vector<int> &registers, string &instructionBuffer){
    int rCount = 0;
    instructionBuffer = "";

    if(instructions.size() >= 1){
        string currentInstruction = instructions[0];
        instructions.erase(instructions.begin());
        vector<string> parsedInstruction = ParseInstruction(currentInstruction);
        string firstSource = parsedInstruction[2];
        string secondSource = parsedInstruction[3];

        char firstSourceValue = registers[firstSource[1] - '0'] + '0';
        char secondSourceValue = registers[secondSource[1] - '0'] + '0';

        instructionBuffer = "<" + parsedInstruction[0] + "," + parsedInstruction[1] + "," + firstSourceValue + "," + secondSourceValue + ">";
    }
}

void UpdateLoadOrArithmeticBuffer(string &instructionBuffer, string &arithmeticBuffer, string &loadBuffer){
    arithmeticBuffer = "";
    loadBuffer = "";
    if(instructionBuffer != ""){
        vector<string> parsedInstruction = ParseInstruction(instructionBuffer);

        if(parsedInstruction[0] == "LD") loadBuffer = instructionBuffer;
        else arithmeticBuffer = instructionBuffer;
    }
}

void UpdateAddressBuffer(string &addressBuffer, string &loadBuffer, vector<int> dataMemory){
    int memoryLocation = 0;
    int registerNumber;

    addressBuffer = "";
    if(loadBuffer != ""){
        vector<string> parsedInstruction = ParseInstruction(loadBuffer);
        int firstSourceOperand = 0, secondSourceOperand = 0;
        char memoryLocation;

        for(int i = 0; i < parsedInstruction[2].length(); i++){
            firstSourceOperand *= 10;
            firstSourceOperand += parsedInstruction[2][i] - '0';
        }

        for(int i = 0; i < parsedInstruction[3].length(); i++){
            secondSourceOperand *= 10;
            secondSourceOperand += parsedInstruction[3][i] - '0';
        }

        memoryLocation = (firstSourceOperand + secondSourceOperand) + '0';
        addressBuffer = "<" + parsedInstruction[1] + ","  + memoryLocation + ">";
    }
}

void UpdateResultBufferWithArithmeticBuffer(vector<string> &resultBuffer, string arithmeticBuffer){

    if(arithmeticBuffer != ""){
        string resultBufferInstruction = "";
        vector<string> parsedInstruction = ParseInstruction(arithmeticBuffer);
        int firstValue = 0, secondValue = 0, output;
        char result;

        for(int i = 0; i < parsedInstruction[2].length(); i++){
            firstValue *= 10;
            firstValue += parsedInstruction[2][i] - '0';
        }

        for(int i = 0; i < parsedInstruction[3].length(); i++){
            secondValue *= 10;
            secondValue += parsedInstruction[3][i] - '0';
        }

        if(parsedInstruction[0] == "ADD") output = firstValue + secondValue;
        else if(parsedInstruction[0] == "SUB") output = firstValue - secondValue;
        else if(parsedInstruction[0] == "AND") output = firstValue & secondValue;
        else if(parsedInstruction[0] == "OR") output = firstValue | secondValue;

        resultBufferInstruction = "<" + parsedInstruction[1] + "," + to_string(output) + ">";
        resultBuffer.push_back(resultBufferInstruction);
    }
}

void UpdateResultBufferWithAddressBuffer(vector<string> &resultBuffer, vector<int> dataMemory, string addressBuffer){

    if(addressBuffer != ""){
        string resultBufferInstruction = "";
        vector<string> parsedInstruction = ParseInstruction(addressBuffer);
        int memoryLocation = 0;
        for(int i = 0; i < parsedInstruction[1].length(); i++){
            memoryLocation *= 10;
            memoryLocation += parsedInstruction[1][i] - '0';
        }

        int memoryValue = dataMemory[memoryLocation];

        resultBufferInstruction = "<" + parsedInstruction[0] + "," + to_string(memoryValue) + ">";
        resultBuffer.push_back(resultBufferInstruction);
    }
}

void UpdateRegisterFile(vector<string> &resultBuffer, vector<int> &registers){

    if(resultBuffer.size() > 0){
        vector<string> parsedInstruction = ParseInstruction(resultBuffer[0]);
        resultBuffer.erase(resultBuffer.begin());

        int registerNumber = parsedInstruction[0][1] - '0';
        int storeValue = 0;
        for(int i = 0; i < parsedInstruction[1].length(); i++){
            storeValue *= 10;
            storeValue += parsedInstruction[1][i] - '0';
        }

        registers[registerNumber] = storeValue;
    }
}

bool DataIsBeingProcessed(vector<string> instructions, vector<string> resultBuffer, string instructionBuffer, string arithmeticBuffer, string loadBuffer, string addressBuffer){
    if(instructions.size() == 0 && resultBuffer.size() == 0 && instructionBuffer == "" && arithmeticBuffer == "" && loadBuffer == "" && addressBuffer == ""){
        return false;
    }
    else{
        return true;
    }
}

int main(){

    int currentStep = 0, numInstrutions, numRegisters, numDataMemory;
    vector<int> registers, dataMemory;
    vector<string> instructions, resultBuffer;
    string instructionBuffer, arithmeticBuffer, loadBuffer, addressBuffer;
    ofstream outputFile("simulation.txt");

    if(!outputFile.is_open()) return -1;

    LoadInstructions(instructions, "instructions.txt");
    LoadRegistersOrDataMemory(registers, "registers.txt");
    LoadRegistersOrDataMemory(dataMemory, "datamemory.txt");

    while(DataIsBeingProcessed(instructions, resultBuffer, instructionBuffer, arithmeticBuffer, loadBuffer, addressBuffer)){
        outputFile << "STEP " << currentStep << ":";
        outputFile << "\nINM:";

        for(int i = 0; i < instructions.size(); i++){
            outputFile << instructions[i];
            if(i != instructions.size()-1) outputFile << ',';
        }

        outputFile << "\nINB:";
        outputFile << instructionBuffer;

        outputFile << "\nAIB:";
        outputFile << arithmeticBuffer;

        outputFile << "\nLIB:";
        outputFile << loadBuffer;

        outputFile << "\nADB:";
        outputFile << addressBuffer;

        outputFile << "\nREB:";
        for(int i = 0; i < resultBuffer.size(); i++){
            outputFile << resultBuffer[i];
            if(i != resultBuffer.size()-1) outputFile << ",";
        }

        outputFile << "\nRGF:";
        for(int i = 0; i < registers.size(); i++){
            outputFile << "<R" << i << "," << registers[i] << ">";
            if(i != registers.size()-1) outputFile << ",";
        }

        outputFile << "\nDAM:";
        for(int i = 0; i < dataMemory.size(); i++){
            outputFile << "<" << i << "," << dataMemory[i] << ">";
            if(i != dataMemory.size()-1) outputFile << ",";
        }

        UpdateRegisterFile(resultBuffer, registers);
        UpdateResultBufferWithAddressBuffer(resultBuffer, dataMemory, addressBuffer);
        UpdateResultBufferWithArithmeticBuffer(resultBuffer, arithmeticBuffer);
        UpdateAddressBuffer(addressBuffer, loadBuffer, dataMemory);
        UpdateLoadOrArithmeticBuffer(instructionBuffer, arithmeticBuffer, loadBuffer);
        UpdateInstructionBuffer(instructions, registers, instructionBuffer);

        currentStep++;
        outputFile << "\n\n";
    }

    // Final print
    outputFile << "STEP " << currentStep << ":";
    outputFile << "\nINM:";
    outputFile << "\nINB:";
    outputFile << "\nAIB:";
    outputFile << "\nLIB:";
    outputFile << "\nADB:";
    outputFile << "\nREB:";

    outputFile << "\nRGF:";
    for(int i = 0; i < registers.size(); i++){
        outputFile << "<R" << i << "," << registers[i] << ">";
        if(i != registers.size()-1) outputFile << ",";
    }

    outputFile << "\nDAM:";
    for(int i = 0; i < dataMemory.size(); i++){
        outputFile << "<" << i << "," << dataMemory[i] << ">";
        if(i != dataMemory.size()-1) outputFile << ",";
    }

    return 0;
}
