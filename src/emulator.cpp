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


const int MEMORY_SIZE = 2*1e5; 
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
    memdump << "MEMORY (First 8000 Words)\n\n";
    fr(i,0,8000){
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

struct Options {
    bool trace = false;
    bool before = false;
    bool after = false;
    bool isa = false;
    string filename;
};

Options parse_args(int argc, char* argv[]){
    Options opt;
    fr(i,1,argc){
        string arg = argv[i];
        if (arg == "-trace") opt.trace = true;
        else if (arg == "-before") opt.before = true;
        else if (arg == "-after") opt.after = true;
        else if (arg == "-isa") opt.isa = true;
        else opt.filename = arg;
    }
    return opt;
}

void print_isa(){
    cout<<"Instruction Set Architecture\n\n";
    cout<<"Instruction  Opcode  Operand\n";
    cout<<"--------------------------------\n";
    cout<<"ldc        0       value\n";
    cout<<"adc        1       value\n";
    cout<<"ldl        2       offset\n";
    cout<<"stl        3       offset\n";
    cout<<"ldnl       4       offset\n";
    cout<<"stnl       5       offset\n";
    cout<<"add        6       none\n";
    cout<<"sub        7       none\n";
    cout<<"shl        8       none\n";
    cout<<"shr        9       none\n";
    cout<<"adj        10      value\n";
    cout<<"a2sp       11      none\n";
    cout<<"sp2a       12      none\n";
    cout<<"call       13      label\n";
    cout<<"return     14      none\n";
    cout<<"brz        15      label\n";
    cout<<"brlz       16      label\n";
    cout<<"br         17      label\n";
    cout<<"HALT       18      none\n";
}

bool load_object(const string& filename, int &program_size){
    ifstream objfile(filename, ios::binary);
    if (!objfile){
        cout<<"ERROR: FILE CANNOT BE OPENED"<<endl;
        return false;
    }
    int index=0;
    int32_t value;
    while (objfile.read((char*)&value, sizeof(value))) {
        if (index >= MEMORY_SIZE) {
            cout << "PROGRAM TOO LARGE\n";
            break;
        }
        MEMORY[index++] = value;
    }
    objfile.close();
    program_size = index;
    return true;
}

void reset_machine(){
    ORIGINAL = STACK_POINT;
    REG_A = 0;
    REG_B = 0;
    PROG_CNT = 0;
    STACK_POINT = ORIGINAL;
    RUNNING = true;
}

int execute(const string& filename, int PROGRAM_SIZE, const Options& opt){
    if(opt.before) memory_dump(filename);
    reset_machine();
    // Mnemonic lookup table
    const char* mnemonics[]={
        "ldc", "adc", "ldl", "stl", "ldnl", "stnl",
        "add", "sub", "shl", "shr",
        "adj", "a2sp", "sp2a",
        "call", "return",
        "brz", "brlz", "br",
        "HALT"
    };
    int instruction_count = 0;
    bool infinite_loop = false;
    while(RUNNING){
        if(instruction_count > 100000){
            // Write log file with warning of infinte loop
            string base="";
            for (char c : filename) {
                if (c=='.') break;
                base+=c;
            }
            string logname=base+".log";
            ofstream logfile(logname);
            if(logfile){
                logfile << "Infinite loop detected (more than 100000 instructions executed, core dumped)" << '\n';
                logfile.close();
            }
            break;
        }
        if(PROG_CNT < 0 || PROG_CNT >= MEMORY_SIZE){
            cout<<"PC OUT OF BOUNDS\n";
            break;
        }
        if(PROG_CNT >= PROGRAM_SIZE){
            RUNNING = false;
            break;
        }
        int32_t instruction = MEMORY[PROG_CNT];
        int opcode = instruction & 0xFF;
        int32_t operand = instruction >> 8;   // signed shift
        PROG_CNT++;  // move to next instruction
        instruction_count++;
        if(opt.trace){
            cout << "PC: " << integer_to_hexa(PROG_CNT - 1, 8)
                 << "   SP: " << integer_to_hexa(STACK_POINT, 8)
                 << "   A: " << integer_to_hexa(REG_A, 8)
                 << "   B: " << integer_to_hexa(REG_B, 8)
                 << "          ";
            cout << mnemonics[opcode];
            // Instructions that use an operand
            if(opcode<=5 || opcode==10 || opcode==13 || (opcode>=15 && opcode<=17) || opcode==18){
                cout << " " << operand;
            }
            cout << endl;
        }
        switch(opcode){
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
                STACK_POINT += operand;
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
    if(opt.after) memory_dump(filename);
    cout << instruction_count << " number of instructions executed\n";
    return 0;
}

int main(int argc, char* argv[]){
    Options opt = parse_args(argc, argv);
    if (opt.isa){
        print_isa();
        return 0;
    }
    if (opt.filename.empty()){
        cout << "Usage: ./emulator [options] file.o\n";
        return 1;
    }
    int program_size = 0;
    if (!load_object(opt.filename, program_size)) return 1;
    return execute(opt.filename, program_size, opt);
}