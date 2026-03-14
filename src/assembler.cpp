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
vector<pair<string, string>> instruction(6000000); // store instruction
map<string, pair<int, int>>Opcode_Table;
map<string, int> symbolTable; // label->address
vector<pair<int, string>> ErrorList;
vector<pair<int, string>> WarningList;
map<string, bool> LabelDefined;
map<string,bool> LabelUsed;
map<int, string> Object_Code; // line-> machine code 
map<int, int> line_To_address; // line ->Pc

map<int, pair<bool,bool>> Line_Info; // stores has_Label,has_Instruction
vector<string> Line_Label(6000000);                     
map<int, string> Source_Line;
string filename;
int pc=0;

void opcodes(){
    Opcode_Table["data"] = {-1, 1}; // opcode number, type
    Opcode_Table["ldc"] = {0, 1};
    Opcode_Table["adc"] = {1, 1};
    Opcode_Table["ldl"] = {2, 1};
    Opcode_Table["stl"] = {3, 1};
    Opcode_Table["ldnl"] = {4, 1};
    Opcode_Table["stnl"] = {5, 1};
    Opcode_Table["add"] = {6, 0};
    Opcode_Table["sub"] = {7, 0};
    Opcode_Table["shl"] = {8, 0};
    Opcode_Table["shr"] = {9, 0};
    Opcode_Table["adj"] = {10, 1};
    Opcode_Table["a2sp"] = {11, 0};
    Opcode_Table["sp2a"] = {12, 0};
    Opcode_Table["call"] = {13, 2};
    Opcode_Table["return"] = {14, 0};
    Opcode_Table["brz"] = {15, 2};
    Opcode_Table["brlz"] = {16, 2};
    Opcode_Table["br"] = {17, 2};
    Opcode_Table["HALT"] = {18, 0};
    Opcode_Table["SET"] = {-2, 1};
}

bool is_valid_file(int arg_cnt, vector<string>arg){

    if (arg_cnt<2){
        cout<<"ERROR: No file provided"<<endl;
        return false;
    }
    string fileName=arg[1];
    if (fileName.find(".asm")==string::npos){
        cout<<"ERROR: INCORRECT FILE FORMAT" << endl;
        return false;
    }
    return true;
}
bool valid_label(string Line_Label){
    if (Line_Label.empty()){
        return false;
    }
    // First character must be letter
    if (!isalpha(Line_Label[0])){
        return false;
    }
    // Remaining characters
    size_t n=Line_Label.size();
    fr(i,1,n){
        if(!isalnum(Line_Label[i])){
            return false;
        }
    }
    return true;
}
string trim(string &s) {
    // removing trailing whitespaces
    size_t start=0;
    while(start<s.size() && (isspace((unsigned char)s[start]) || s[start]=='\r' || s[start]=='\n' || s[start]=='\t' || s[start]=='\0')){
        start++;
    }
    // removing ending whitespaces
    size_t end=s.size();
    while(end>start&&(isspace((unsigned char)s[end-1]) || s[end-1]=='\r' || s[end-1]=='\n' || s[end-1]=='\t' || s[end-1]=='\0')){
        end--;
    }
    s=s.substr(start, end-start);
    return s;
}
string integer_to_hexa(int32_t decimal, int n){
    uint32_t new_dec=static_cast<uint32_t>(decimal);
    string result="";
    string hexChars="0123456789ABCDEF";
    fr(i,0,n){
        int remainder=new_dec%16;
        result=hexChars[remainder]+result;
        new_dec=new_dec/16;
    }
    return result;
}
bool is_valid_number(string s){
    if(s.empty()){
        return false;
    }
    size_t start=0;
    // + or - sign
    if(s[0]=='+'||s[0]=='-'){
        start=1;
    }
    if(start>=s.size()){
        return false;
    }
    // checking hex number , char should lie between 0 and 9 and between a and f (inclusive)
    if (s.substr(start,2)=="0x"){
        start+=2;
        if (start>=s.size()) return false;
        int n=s.size();
        fr(i,start,n){
            char c=s[i];
            if(!((c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F'))){
                return false;
            }
        }
        return true;
    }
    // checking octal number , char should lie between 0 and 7 and between a and f (inclusive)
    if (s.substr(start,2)=="0o"){
        start += 2;
        if (start >= s.size()) return false;
        fr(i,start,s.size()){
            if (s[i]<'0' || s[i]>'7'){
                return false;
            }
        }
        return true;
    }
    // Decimal , char should lie between 0 and 9 (inclusive)
    fr(i,start,s.size()){
        if (s[i]<'0' || s[i]>'9'){
            return false;
        }
    }
    return true;
}

void Pass_One(string line, int num){
    // remove comments
    string s= "";
    size_t n=line.size();
    fr(i,0,n){
        if (line[i]==';'){
            break;
        }
        s=s+line[i];
    }
    line=s;
    //trim leading and trailing whitespaces
    line=trim(line);
    Line_Info[num] = {0, 0};
    if (!line.empty()){
        // detect Line_Label (also checking if multiple label exist in same line)
        while (true) {
            int colon_pos = -1;
            fr(i,0,line.size()){ // getting position of semicolon :
                if (line[i] == ':'){
                    colon_pos = i;
                    break;
                }
            }
            if(colon_pos == -1) break;   // no more labels
            string lab = line.substr(0, colon_pos); // extracting label name before colon
            line_To_address[num] = pc;
            lab = trim(lab); // removing extra spaces from label name

            if(lab.empty()){
                ErrorList.push_back({num, "INVALID LABEL NAME"});
            }
            else if (!valid_label(lab)){
                ErrorList.push_back({num, "INVALID LABEL NAME"});
            }
            else{
                if(symbolTable.find(lab) != symbolTable.end()){
                    ErrorList.push_back({num, "REDECLARATION OF LABEL"});
                }
                else{
                    symbolTable[lab] = pc;
                    Line_Label[num] = lab;
                    Line_Info[num].first = 1;
                }
            }

            // remove processed label
            line = line.substr(colon_pos + 1);
            line = trim(line);

            if(line.empty()){
                line_To_address[num]=pc;
                return;
            }
        }
        // fetching instruction 
        vector<string> INSTRUCTION;
        string temp = "";
        fr(i,0,line.size()){
            if(isspace((unsigned char)line[i]) || line[i]=='\0'){  // seperating instruction,numbers,etc.
                if(!temp.empty()){
                    INSTRUCTION.pb(temp);
                    temp="";
                }
            }
            else{
                temp+=line[i];
            }
        }
        if(!temp.empty()){
            INSTRUCTION.pb(temp);
        }
        if(INSTRUCTION.empty()){
            return;
        }
        if (Opcode_Table.find(INSTRUCTION[0])==Opcode_Table.end()){ // checking unknown instruction
            ErrorList.push_back({num, "UNKNOWN INSTRUCTION"});
            return;
        }
        int operandType=Opcode_Table[INSTRUCTION[0]].second;
        if (INSTRUCTION.size()>2) { 
            ErrorList.push_back({num, "EXTRA ON END OF LINE"});
            return;
        }
        if (operandType==0){ 
            if (INSTRUCTION.size()!=1){
                ErrorList.push_back({num, "WRONG OPERAND TYPE"});
                return;
            }
        }
        else if (operandType == 1){
            if (INSTRUCTION.size()!=2){
                ErrorList.push_back({num, "WRONG OPERAND TYPE"});
                return;
            }
        }
        else if (operandType==2){
            if (INSTRUCTION.size()!=2){
                ErrorList.push_back({num, "WRONG OPERAND TYPE"});
                return;
            }
        }
        else{
            ErrorList.push_back({num, "INVALID INSTRUCTION FORMAT"});
            return;
        }
        
        // Pass 1 basic validation for operand type 1 and 2
        if (operandType==1 || operandType==2) {
            string op = INSTRUCTION[1];
            if (!valid_label(op) && !is_valid_number(op)) {
                ErrorList.push_back({num, "INVALID OPERAND"});
                Line_Info[num].second = 0; // Prevent evaluating in Pass Two
                return;
            }
        }
        
        Line_Info[num].second = 1;
        line_To_address[num] = pc;
        instruction[num].first = INSTRUCTION[0];
        if (operandType==0){
            instruction[num].second = "";
        }
        else{
            instruction[num].second = INSTRUCTION[1];
        }
        if(INSTRUCTION[0]!="SET"){
            pc++;
        }
    }
}
void Pass_Two(){
    // Remove SET labels from symbol table so they're only available after definition
    for(auto& it: Line_Info){
        int lineNo = it.first;
        if(it.second.second != 1) continue;
        if(instruction[lineNo].first == "SET"){
            if(Line_Label[lineNo] != ""){
                symbolTable.erase(Line_Label[lineNo]);
            }
        }
    }
    // Process all instructions in order
    for(auto& it: Line_Info){
        int lineNo=it.first;

        if(it.second.second!=1) continue;
        string command = instruction[lineNo].first;
        int opcode =Opcode_Table[command].first;
        int operandType =Opcode_Table[command].second;
        int32_t operand=0;
        if (operandType==0){
            operand=0;
        }
        else if(operandType==1){
            string op = instruction[lineNo].second;
            if (symbolTable.find(op)!=symbolTable.end()){
                operand = symbolTable[op];
                LabelUsed[op] = true;
            }
            else if(is_valid_number(op)){
                char *endptr;
                long value = strtol(op.c_str(),&endptr,0);
                if (*endptr !='\0') {
                    ErrorList.push_back({lineNo, "NOT A NUMBER"});
                    continue;
                }
                operand = (int32_t)value;
            }
            else{
                ErrorList.push_back({lineNo, "UNDEFINED LABEL: " + op});
            }
        }
        else if(operandType==2){
            string op = instruction[lineNo].second;
            if (symbolTable.find(op) != symbolTable.end()){
                int target = symbolTable[op];
                LabelUsed[op] = true;
                int currentPC = line_To_address[lineNo];
                operand = target - (currentPC + 1);
            }
            else if(is_valid_number(op)){
                char *endptr;
                long value = strtol(op.c_str(),&endptr,0);
                if (*endptr !='\0') {
                    ErrorList.push_back({lineNo, "NOT A NUMBER"});
                    continue;
                }
                operand = (int32_t)value;
            }
            else{
                ErrorList.push_back({lineNo, "NO SUCH LABEL"});
                continue;
            }
        }

        // Pseudo Instructions: SET AND DATA HANDLING
        if(opcode == -2){ // SET: update symbol table, no object code
            // Find the label on this line and update its value
            if(Line_Label[lineNo] != ""){
                symbolTable[Line_Label[lineNo]] = operand;
            }
            Object_Code[lineNo] = integer_to_hexa(operand, 8);
            continue;
        }
        if(opcode == -1){ // data: emit value as object code
            Object_Code[lineNo] = integer_to_hexa(operand, 8);
            continue;
        }
        // 24-bit Overflow Check 
        if(operand< -(1<<23) || operand>((1<<23)-1)){
            ErrorList.push_back({lineNo, "OPERAND OUT OF RANGE"});
            continue;
        }
        // Build Instruction 
        Object_Code[lineNo] = integer_to_hexa(operand, 6) + integer_to_hexa(opcode, 2);
    }
}

void print_errors(){
    fr(i,0,ErrorList.size()){
        cout<<"LINE NUMBER "<<ErrorList[i].first<<":- "<<ErrorList[i].second<<endl;
    }
}
void print_warnings(){
    if(!WarningList.empty()){
        fr(i,0,WarningList.size()){
            cout<<"LINE NUMBER "<< WarningList[i].first<<":- "<<WarningList[i].second<<endl;
        }
    }
}
void write_symbol_table(string filename){
    ofstream symfile(filename + ".sym");

    if(!symfile.is_open()){
        cout<<"ERROR: Cannot open symbol table file\n";
        return;
    }

    symfile<<"--------------------------------\n";
    symfile<<"        SYMBOL TABLE\n";
    symfile<<"--------------------------------\n";
    symfile<<"LABEL\t\tADDRESS\n";
    symfile<<"--------------------------------\n";

    for(auto &it : symbolTable){
        symfile << left << setw(15) << it.first;
        symfile << right << setw(8) << setfill('0') << hex << uppercase << it.second << endl;
        symfile << setfill(' ');
    }

    symfile<<"--------------------------------\n";

    symfile.close();
}
void write_log(){
    string s=filename + ".log";
    ofstream logfile(s);
    if(!logfile){
        cout<<"ERROR: Unable to create log file"<<endl;
        return;
    }
    // Sort by line number
    sort(all(WarningList));
    sort(all(ErrorList));
    if(!WarningList.empty()){
        logfile<<"WARNINGS\n";
        fr(i,0,WarningList.size()){
            logfile<<"LINE NUMBER "<<WarningList[i].first<<":- "<<WarningList[i].second<<endl;
        }
        logfile<<endl;
    }
    logfile<<"ERRORS\n";
    fr(i,0,ErrorList.size()){
        logfile<<"LINE NUMBER "<<ErrorList[i].first<<":- "<<ErrorList[i].second<<endl;
    }
    logfile.close();
}
void write_list(){
    string s = filename + ".lst";
    ofstream listing_file(s);
    if(!listing_file){
        cout << "ERROR: Unable to create listing file\n";
        return;
    }
    for(auto &it : Line_Info){
        int lineNo = it.first;
        // Skip lines with no label and no instruction (comment-only or blank)
        if (it.second.first == 0 && it.second.second == 0) continue;
        // Print Address 
        listing_file << setw(8) << setfill('0')<< hex << uppercase<< line_To_address[lineNo]<< "  ";

        // Print Machine Code 
        if(it.second.second == 1){
            listing_file << setw(8) << setfill('0')<< hex << uppercase<< Object_Code[lineNo]<< "  ";
        }
        else{
            listing_file<< "        "<< "  ";
        }
        // Reset formatting for text
        listing_file << dec;
        // Print Source_Line without comments
        string src = Source_Line[lineNo];
        size_t comment_pos = src.find(';');
        if (comment_pos != string::npos) {
            src = src.substr(0, comment_pos);
        }
        listing_file << src << "\n";
    }
    listing_file.close();
}
void write_obj(){
    string s = filename + ".o";
    ofstream objfile(s, ios::binary);
    if(!objfile){
        cout << "ERROR: Unable to create object file\n";
        return;
    }
    for (auto &it: Line_Info){
        int lineNo = it.first;
        if (it.second.second != 1){
            continue;
        }
        // SET doesn't emit object code
        if (instruction[lineNo].first == "SET") continue;
        // Get hex string 
        string hexcode = Object_Code[lineNo];

        // Convert hex string to 32-bit integer
        uint32_t machine = stoul(hexcode, nullptr, 16);
        // Write 4 bytes to file
        objfile.write(reinterpret_cast<char*>(&machine), sizeof(uint32_t));
    }
    objfile.close();
}
int main(int argu_cnt, char *argv[]){
    ios::sync_with_stdio(false);
    cin.tie(NULL);
    opcodes();
    // Convert argv to vector<string>
    vector<string> arg(argv, argv + argu_cnt);
    // Validating input file 
    if (!is_valid_file(argu_cnt, arg)){
        return 0;
    }
    string inputFile = arg[1];
    // Extract filename without extension 
    filename = "";
    int i=0;
    while(i<inputFile.size() && inputFile[i]!='.'){
        filename += inputFile[i];
        i++;
    }

    // Opening file 
    ifstream file_pointer(inputFile);

    if (!file_pointer){
        ErrorList.push_back({0, ".asm FILE NOT ACCESSIBLE"});
        write_log();
        return 0;
    }

    // PASS ONE()
    string curr_line;
    int lineNumber = 1;

    while (getline(file_pointer, curr_line)){
        Pass_One(curr_line, lineNumber);
        Source_Line[lineNumber] = curr_line;
        lineNumber++;
    }

    file_pointer.close();
    write_symbol_table(filename);
    // PASS TWO()
    Pass_Two();
    // Detect unused labels
    for (auto &it : symbolTable){
        string label = it.first;
        if (!LabelUsed[label]){
            WarningList.push_back({0, "UNUSED LABEL: " + label});
        }
    }
    // Write log 
    write_log();

    // Generate output files only if no errors 
    if (ErrorList.empty()){
        write_list();
        write_obj();
    }
    return 0;
}