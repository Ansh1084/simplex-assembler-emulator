#include <bits/stdc++.h>
using namespace std;
#define pb push_back
#define all(x) x.begin(), x.end()
#define endl '\n'
#define fastio() ios::sync_with_stdio(false); cin.tie(NULL);
typedef vector<int> vi;
typedef pair<int, int> pii;
#ifdef LOCAL
#include <chrono>
#define debug(x) cerr << #x << ": " << x << endl;
#else
#define debug(x)
#endif
#define fr(i, a, b) for (size_t i = a; i < b; ++i)

const int MEMORY_SIZE = 1 << 20; // 1M words
//  Registers 
int32_t REG_A = 0;
int32_t REG_B = 0;
int32_t PROG_CNT = 0;
int32_t STACK_POINT = MEMORY_SIZE - 1; // assuming stack grows downward
int ORIGINAL; 
//  Memory 
vector<int32_t> MEMORY(MEMORY_SIZE, 0);

//  Execution State 
bool RUNNING = true;
string integer_to_hexa(int32_t number, size_t width){
    unsigned int value = (unsigned int)number; // handle negative properly
    string hexResult = "";
    char hexDigits[] = "0123456789ABCDEF";
    fr(i,0,width){
        int digit = value&0xF;   // take last 4 bits
        hexResult+=hexDigits[digit];
        value = value>>4;        // shift right 4 bits
    }
    reverse(hexResult.begin(), hexResult.end());
    return hexResult;
}
void memory_dump(string filename){
    string base = "";
    for (char c:filename){
        if (c=='.'){
            break;
        }   
        base+=c;
    }
    base += "_memdump.txt";
    ofstream memdump(base);
    if (!memdump){
        cout<<"Error creating memory dump file"<<endl;
        return;
    }

    memdump << "MEMORY (First 100 Words)\n\n";

    fr(i,0,100){
        // Print address at start of every row
        if(i%8==0){
            memdump << integer_to_hexa(i, 8) << ": ";
        }
        // Print memory word
        memdump << integer_to_hexa(MEMORY[i], 8) << "  ";
        // Newline after every 8 values
        if((i+1)%8==0){
            memdump<<"\n";
        }
    }

    memdump << "\n\nREGISTERS \n\n";
    memdump << "REG_A      = " <<REG_A<<endl;
    memdump << "REG_B      = " <<REG_B<<endl;
    memdump << "PROG_CNT   = " <<PROG_CNT<<endl;
    memdump << "STACK_POINT= " <<STACK_POINT<<endl;
    memdump.close();
}
int main(int argc, char* argv[]){
    string filename;
    bool trace = false;
    bool before = false;
    bool after = false;
    bool isa = false;
    // Parse command line args
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-trace")
            trace = true;
        else if (arg == "-before")
            before = true;
        else if (arg == "-after")
            after = true;
        else if (arg == "-isa")
            isa = true;
        else
            filename = arg;
    }
    if (isa) {
        cout << "Supported Instructions:\n";
        cout << "ldc adc ldl stl ldnl stnl\n";
        cout << "add sub shl shr\n";
        cout << "adj a2sp sp2a\n";
        cout << "call return\n";
        cout << "brz brlz br\n";
        cout << "HALT\n";
        return 0;
    }
    if (filename.empty()){
        cout << "Usage: ./emulator [options] file.o\n";
        return 1;
    }
    // LOAD OBJECT FILE (Binary) 
    ifstream objfile(filename, ios::binary);
    if (!objfile){
        cout<<"ERROR: FILE CANNOT BE OPENED"<<endl;
        return 0;
    }
    int index=0;
    while (objfile.read(reinterpret_cast<char*>(&MEMORY[index]), sizeof(int32_t))) {
        if (index>=MEMORY_SIZE) {
            cout<<"PROGRAM TOO LARGE\n";
            break;
        }
        index++;
    }
    objfile.close();
    ORIGINAL = STACK_POINT;

    REG_A = 0;
    REG_B = 0;
    PROG_CNT = 0;
    STACK_POINT = ORIGINAL;
    if(before){
        memory_dump(filename);
    }
    // Execution
    PROG_CNT = 0;
    RUNNING = true;
    int instruction_count = 0;
    while(RUNNING){
        if (PROG_CNT < 0 || PROG_CNT >= MEMORY_SIZE) {
            cout << "PC OUT OF BOUNDS\n";
            break;
        }
        int32_t instruction = MEMORY[PROG_CNT];

        int opcode  = instruction & 0xFF;
        int32_t operand = instruction>>8;   // signed shift
        PROG_CNT++;  // move to next instruction
        instruction_count++;
        if (trace) {
            cout << "PC:" << integer_to_hexa(PROG_CNT - 1, 8)
                 << " SP:" << integer_to_hexa(STACK_POINT, 8)
                 << " A:" << integer_to_hexa(REG_A, 8)
                 << " B:" << integer_to_hexa(REG_B, 8)
                 << endl;
        }
        switch (opcode) {
            case 0:  // ldc
                REG_B = REG_A;
                REG_A = operand;
                break;

            case 1:  // adc
                REG_A += operand;
                break;

            case 2: {  // ldl
                int32_t addr = STACK_POINT + operand;
                if (addr < 0 || addr >= MEMORY_SIZE) {
                    cout << "MEMORY ACCESS VIOLATION\n";
                    RUNNING = false;
                    break;
                }
                REG_B = REG_A;
                REG_A = MEMORY[addr];
                break;
            }

            case 3: {  // stl
                int32_t addr = STACK_POINT + operand;
                if (addr < 0 || addr >= MEMORY_SIZE) {
                    cout << "MEMORY ACCESS VIOLATION\n";
                    RUNNING = false;
                    break;
                }
                MEMORY[addr] = REG_A;
                REG_A = REG_B;
                break;
            }

            case 4: {  // ldnl
                int32_t addr = REG_A + operand;
                if (addr < 0 || addr >= MEMORY_SIZE) {
                    cout << "MEMORY ACCESS VIOLATION\n";
                    RUNNING = false;
                    break;
                }
                REG_A = MEMORY[addr];
                break;
            }

            case 5: {  // stnl
                int32_t addr = REG_A + operand;
                if (addr < 0 || addr >= MEMORY_SIZE) {
                    cout << "MEMORY ACCESS VIOLATION\n";
                    RUNNING = false;
                    break;
                }
                MEMORY[addr] = REG_B;
                break;
            }

            case 6:  // add
                REG_A = REG_B + REG_A;
                break;

            case 7:  // sub
                REG_A = REG_B - REG_A;
                break;

            case 8:  // shl
                REG_A = REG_B << REG_A;
                break;

            case 9:  // shr
                REG_A = REG_B >> REG_A;
                break;

            case 10:  // adj
                STACK_POINT -= operand;
                if (STACK_POINT < 0 || STACK_POINT >= MEMORY_SIZE) {
                    cout << "STACK OVERFLOW/UNDERFLOW\n";
                    RUNNING = false;
                }
                break;

            case 11:  // a2sp
                STACK_POINT = REG_A;
                REG_A = REG_B;
                break;

            case 12:  // sp2a
                REG_B = REG_A;
                REG_A = STACK_POINT;
                break;

            case 13:  // call
                REG_B = REG_A;
                REG_A = PROG_CNT;
                PROG_CNT += operand;
                break;

            case 14:  // return
                PROG_CNT = REG_A;
                REG_A = REG_B;
                break;

            case 15:  // brz
                if (REG_A == 0) {
                    PROG_CNT += operand;
                }
                break;

            case 16:  // brlz
                if (REG_A < 0) {
                    PROG_CNT += operand;
                }
                break;

            case 17:  // br
                PROG_CNT += operand;
                break;

            case 18:  // HALT
                RUNNING = false;
                break;

            default:
                cout << "INVALID OPCODE\n";
                RUNNING = false;
        }
    }

// Memory dump after
    if (after) {
        memory_dump(filename);
    }
    cout << instruction_count << " number of instructions executed\n";
    return 0;
}