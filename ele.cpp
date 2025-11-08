#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
using namespace std;
// --- Data Structures for Tables ---
class Symbol
{
public:
    string name;
    int address;
};
class Literal
{
public:
    string name;
    int address;
};
class AssemblerPass1
{
    // --- Core Data Tables ---
    string opcodeTable[20][3]; // Mnemonic, Class, Code
    string regTable[4][2];     // Register, Code
    int opcodeCount;

    Symbol symtab[50];
    Literal littab[50];
    int pooltab[20];

    int symCount, litCount, poolCount;
    int LC; // Location Counter

    // --- Helper Function to get a symbol's address ---
    // Used by EQU and ORIGIN
    int getSymbolAddress(string name)
    {
        for (int i = 0; i < symCount; i++)
        {
            if (symtab[i].name == name)
            {
                return symtab[i].address;
            }
        }
        return -1; // Not found
    }
    // --- Helper function to evaluate expressions ---
    // Handles:
    // 1. Constant ("300")
    // 2. Symbol ("L1")
    // 3. Symbol + Offset ("L1+3")
    int evaluateExpression(string expression)
    {
        // Check for simple constant
        try
        {
            return stoi(expression);
        }
        catch (...)
        {
            // Not a simple integer, proceed
        }

        // Parse "symbol+offset"
        size_t plusPos = expression.find('+');
        string symName;
        int offset = 0;

        if (plusPos == string::npos)
        {
            symName = expression;
        }
        else
        {
            symName = expression.substr(0, plusPos);
            string constVal = expression.substr(plusPos + 1);
            try
            {
                offset = stoi(constVal);
            }
            catch (...)
            {
                cout << "Error: Invalid offset in expression: " << expression << endl;
                offset = 0;
            }
        }

        int symAddress = getSymbolAddress(symName);
        if (symAddress == -1)
        {
            cout << "Error: Symbol not found in expression - " << symName << endl;
            return LC; // Fallback
        }

        return symAddress + offset;
    }

public:
    // --- Constructor: Initializes all tables ---
    AssemblerPass1()
    {
        // --- 1. Imperative Statements (IS) ---
        opcodeTable[0][0] = "STOP";
        opcodeTable[0][1] = "IS";
        opcodeTable[0][2] = "00";
        opcodeTable[1][0] = "ADD";
        opcodeTable[1][1] = "IS";
        opcodeTable[1][2] = "01";
        opcodeTable[2][0] = "SUB";
        opcodeTable[2][1] = "IS";
        opcodeTable[2][2] = "02";
        opcodeTable[3][0] = "MUL";
        opcodeTable[3][1] = "IS";
        opcodeTable[3][2] = "03";
        opcodeTable[4][0] = "MOVER";
        opcodeTable[4][1] = "IS";
        opcodeTable[4][2] = "04";
        opcodeTable[5][0] = "MOVEM";
        opcodeTable[5][1] = "IS";
        opcodeTable[5][2] = "05";
        opcodeTable[6][0] = "COMP";
        opcodeTable[6][1] = "IS";
        opcodeTable[6][2] = "06";
        opcodeTable[7][0] = "BC";
        opcodeTable[7][1] = "IS";
        opcodeTable[7][2] = "07";
        opcodeTable[8][0] = "DIV";
        opcodeTable[8][1] = "IS";
        opcodeTable[8][2] = "08";
        opcodeTable[9][0] = "READ";
        opcodeTable[9][1] = "IS";
        opcodeTable[9][2] = "09";
        opcodeTable[10][0] = "PRINT";
        opcodeTable[10][1] = "IS";
        opcodeTable[10][2] = "10";

        // --- 2. Assembler Directives (AD) ---
        opcodeTable[11][0] = "START";
        opcodeTable[11][1] = "AD";
        opcodeTable[11][2] = "01";
        opcodeTable[12][0] = "END";
        opcodeTable[12][1] = "AD";
        opcodeTable[12][2] = "02";
        opcodeTable[13][0] = "ORIGIN";
        opcodeTable[13][1] = "AD";
        opcodeTable[13][2] = "03";
        opcodeTable[14][0] = "EQU";
        opcodeTable[14][1] = "AD";
        opcodeTable[14][2] = "04";
        opcodeTable[15][0] = "LTORG";
        opcodeTable[15][1] = "AD";
        opcodeTable[15][2] = "05";

        // --- 3. Declarative Statements (DL) ---
        opcodeTable[16][0] = "DC";
        opcodeTable[16][1] = "DL";
        opcodeTable[16][2] = "01";
        opcodeTable[17][0] = "DS";
        opcodeTable[17][1] = "DL";
        opcodeTable[17][2] = "02";

        opcodeCount = 18; // Total number of opcodes

        // --- 4. Register Table ---
        regTable[0][0] = "AREG";
        regTable[0][1] = "01";
        regTable[1][0] = "BREG";
        regTable[1][1] = "02";
        regTable[2][0] = "CREG";
        regTable[2][1] = "03";
        regTable[3][0] = "DREG";
        regTable[3][1] = "04";

        // --- 5. Initialize Counters ---
        symCount = litCount = 0;
        poolCount = 1;
        pooltab[0] = 0; // First pool starts at literal index 0
        LC = 0;
    }

    // --- Table Lookup Functions ---
    string getOpClass(string mnemonic)
    {
        for (int i = 0; i < opcodeCount; i++)
            if (opcodeTable[i][0] == mnemonic)
                return opcodeTable[i][1];
        return "";
    }

    string getOpCode(string mnemonic)
    {
        for (int i = 0; i < opcodeCount; i++)
            if (opcodeTable[i][0] == mnemonic)
                return opcodeTable[i][2];
        return "";
    }

    string getRegCode(string reg)
    {
        for (int i = 0; i < 4; i++)
            if (regTable[i][0] == reg)
                return regTable[i][1];
        return "00";
    }

    string getConditionCode(string cc)
    {
        if (cc == "LT")
            return "01";
        if (cc == "LE")
            return "02";
        if (cc == "EQ")
            return "03";
        if (cc == "GT")
            return "04";
        if (cc == "GE")
            return "05";
        if (cc == "ANY")
            return "06";
        return "00";
    }
    bool isOpcode(string word)
    {
        for (int i = 0; i < opcodeCount; i++)
            if (opcodeTable[i][0] == word)
                return true;
        return false;
    }

    // --- Symbol and Literal Table Management ---
    int addSymbol(string name, int address)
    {
        for (int i = 0; i < symCount; i++)
        {
            if (symtab[i].name == name)
            {
                if (address != -1)
                {
                    symtab[i].address = address;
                }
                else if (symtab[i].address == -1 && address != -1)
                {
                    symtab[i].address = address;
                }
                return i;
            }
        }

        symtab[symCount].name = name;
        symtab[symCount].address = address;
        symCount++;
        return symCount - 1;
    }
    int addLiteral(string lit)
    {
        for (int i = pooltab[poolCount - 1]; i < litCount; i++)
            if (littab[i].name == lit)
                return i;

        littab[litCount].name = lit;
        littab[litCount].address = -1;
        litCount++;
        return litCount - 1;
    }

    // --- Core Pass 1 Logic ---
    // (UPDATED) Assigns addresses to all literals in the current pool
    void assignLiterals(ofstream &out)
    {
        // --- FIX IS HERE ---
        // Check if there are any new literals to process
        int currentPoolStart = pooltab[poolCount - 1];
        if (currentPoolStart == litCount)
        {
            // This pool is empty (e.g., END called after LTORG).
            // Do NOT add a duplicate pool entry.
            return;
        }

        // Process literals as before
        for (int i = currentPoolStart; i < litCount; i++)
        {
            if (littab[i].address == -1)
            {
                littab[i].address = LC;
                string litValue = littab[i].name.substr(2, littab[i].name.length() - 3);
                out << "(" << LC << ") (DL,02) (C," << litValue << ")\n";
                LC++;
            }
        }

        // Now, add the new pool entry
        pooltab[poolCount] = litCount;
        poolCount++;
    }
    // Processes a single line of assembly code
    void processLine(string line, ofstream &out)
    {
        if (line.empty())
            return;
        string label, opcode, op1, op2;
        stringstream ss(line);
        string firstWord;

        ss >> firstWord;

        if (isOpcode(firstWord))
        {
            label = "";
            opcode = firstWord;
            ss >> op1 >> op2;
        }
        else
        {
            label = firstWord;
            ss >> opcode >> op1 >> op2;
        }

        if (!label.empty() && opcode != "START" && opcode != "EQU")
        {
            addSymbol(label, LC);
        }

        string cls = getOpClass(opcode);
        string code = getOpCode(opcode);

        if (opcode == "START")
        {
            LC = stoi(op1);
            if (!label.empty())
            {
                addSymbol(label, LC);
            }
            out << "(AD," << code << ") (C," << LC << ")\n";
        }
        else if (cls == "IS")
        {

            if (opcode == "STOP")
            {
                out << "(" << LC << ") (IS," << code << ")\n";
            }
            else if (opcode == "READ" || opcode == "PRINT")
            {
                int symIndex = addSymbol(op1, -1);
                out << "(" << LC << ") (IS," << code << ") (S," << symIndex << ")\n";
            }
            else if (opcode == "BC")
            {
                string ccCode = getConditionCode(op1);
                int symIndex = addSymbol(op2, -1);
                out << "(" << LC << ") (IS," << code << ") (C," << ccCode << ") (S," << symIndex << ")\n";
            }
            else
            {
                size_t commaPos = op1.find(',');
                string regName = op1;
                if (commaPos != string::npos)
                {
                    regName = op1.substr(0, commaPos);
                }
                string regCode = getRegCode(regName);

                out << "(" << LC << ") (IS," << code << ") (RG," << regCode << ")";

                if (op2[0] == '=')
                {
                    int litIndex = addLiteral(op2);
                    out << " (L," << litIndex << ")";
                }
                else
                {
                    int symIndex = addSymbol(op2, -1);
                    out << " (S," << symIndex << ")";
                }
                out << "\n";
            }
            LC++;
        }
        else if (opcode == "ORIGIN")
        {
            LC = evaluateExpression(op1);
            out << "(AD," << code << ") (C," << LC << ")\n";
        }
        else if (opcode == "EQU")
        {
            if (label.empty())
            {
                cout << "Error: EQU directive needs a label." << endl;
                return;
            }
            int equAddress = evaluateExpression(op1);
            addSymbol(label, equAddress);
            out << "(AD," << code << ") (C," << equAddress << ")\n";
        }
        else if (opcode == "DC")
        {
            string constVal = op1.substr(1, op1.length() - 2);
            out << "(" << LC << ") (DL," << code << ") (C," << constVal << ")\n";
            LC++;
        }
        else if (opcode == "DS")
        {
            int size = stoi(op1);
            out << "(" << LC << ") (DL," << code << ") (C," << size << ")\n";
            LC += size;
        }
        else if (opcode == "LTORG")
        {
            out << "(" << LC << ") (AD," << code << ")\n";
            assignLiterals(out);
        }
        else if (opcode == "END")
        {
            out << "(" << LC << ") (AD," << code << ")\n";
            assignLiterals(out);
        }
    }

    // --- Output Functions for Each Table ---
    void displaySymbolTable(string filename)
    {
        ofstream out(filename);
        if (!out)
        {
            cerr << "Error: Cannot open file " << filename << endl;
            return;
        }
        out << "=== SYMBOL TABLE ===\n";
        out << "Index\tName\tAddress\n";
        for (int i = 0; i < symCount; i++)
            out << i << "\t" << symtab[i].name << "\t" << symtab[i].address << "\n";
        out.close();
        cout << "Symbol table written to " << filename << endl;
    }

    void displayLiteralTable(string filename)
    {
        ofstream out(filename);
        if (!out)
        {
            cerr << "Error: Cannot open file " << filename << endl;
            return;
        }
        out << "=== LITERAL TABLE ===\n";
        out << "Index\tName\tAddress\n";
        for (int i = 0; i < litCount; i++)
            out << i << "\t" << littab[i].name << "\t" << littab[i].address << "\n";
        out.close();
        cout << "Literal table written to " << filename << endl;
    }

    void displayPoolTable(string filename)
    {
        ofstream out(filename);
        if (!out)
        {
            cerr << "Error: Cannot open file " << filename << endl;
            return;
        }
        out << "=== POOL TABLE ===\n";
        out << "Pool#\tStartIndex\n";
        for (int i = 0; i < poolCount; i++)
            out << i << "\t" << pooltab[i] << "\n";
        out.close();
        cout << "Pool table written to " << filename << endl;
    }
    // --- Main Assembly Process ---
    void assemble(string inputFile)
    {
        ifstream in(inputFile);
        if (!in)
        {
            cerr << "Error: Cannot open input file " << inputFile << endl;
            return;
        }

        ofstream out("intermediate.txt");
        if (!out)
        {
            cerr << "Error: Cannot create intermediate.txt" << endl;
            return;
        }

        cout << "Starting Pass 1..." << endl;
        string line;
        while (getline(in, line))
        {
            processLine(line, out);
        }

        in.close();
        out.close();
        cout << "Intermediate code written to intermediate.txt" << endl;

        displaySymbolTable("sym_table.txt");
        displayLiteralTable("lit_table.txt");
        displayPoolTable("pool_table.txt");

        cout << "Pass-1 completed." << endl;
    }
};

// --- Main Function ---
int main()
{
    AssemblerPass1 a;
    a.assemble("input.asm");
    return 0;
}