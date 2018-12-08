#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iomanip>
#include <map>
using namespace std;

map<string, string> optable;
map<string, int> symtable;
map<int, string> obcode;
const char *del = "\t";
char name[10]; 
int loc = 0;
int locr[1000] = {0};
int p = 0;
int start = 0;
int len = 0;
string first;

//read optable file and create a opcode table 
void createoptab(){
	ifstream input("optable.txt", ifstream::in);
	string line;
	if(!input){
		cout << "optable does not exist" << endl;
		return;
	}
	
	while(getline(input, line)){
		char tmp[20];
		strcpy(tmp, line.c_str());//convert string to char
		//cut the line
		char *s = strtok(tmp, del);
		while(s != NULL){
		  	char *key = s;
		  	s = strtok(NULL, del);
		  	char *value = s;
		  	s = strtok(NULL, del);
		  	//convert char to string and add to optable
		  	string keyString(key);
		  	string valueString(value);
		  	optable[keyString] = valueString;
		}	
	}
	input.close();
}
//find the code is in optable or not
bool isOpcode(char *s){
	string str(s);
	map<string,string>::iterator iter;
	iter = optable.find(str);
	if(iter == optable.end())
		return false;
	else
		return true;
}
//find the code is in symtable or not
bool inSymcode(char *s){
	string str(s);
	map<string,int>::iterator iter;
	iter = symtable.find(str);
	if(iter == symtable.end())
		return false;
	else
		return true;
}
//calculate the number of token
int calToken(string str){
	char tmp[40];
	int i = 1;
	strcpy(tmp, str.c_str());
	char *s = strtok(tmp, del);
	while(s != NULL){
		s = strtok(NULL, del);
	 	s = strtok(NULL, del);
		i++;
	}	
	return i;
}
//calculate the location
void calLoc(char *s, int num){
	char *label;
	char *opcode;
	char *operand;
	int num_op = 0;
	if(num == 2){
		opcode = s;
		operand = strtok(NULL, del);
	}
	else if(num == 3){
		label = s;
		opcode = strtok(NULL, del);
		operand = strtok(NULL, del);
		//search symtable for label
		if(inSymcode(label) == true){//set error flag 
			cout << label << ":" << "duplicate symbol" << endl;
		}
		else{//insert label and location to symtable
			string str(label);
			symtable[str] = loc;
		}
	}
	//search optable for opcode
	if(isOpcode(opcode) == true){//found
		loc+=3;
		locr[p++] = loc;
	}
	else if(strcmp(opcode, "WORD") == 0){
		loc+=3;
		locr[p++] = loc;
	}
	else if(strcmp(opcode, "RESW") == 0){
		num_op = atoi(operand);
		loc = loc + 3 * num_op;
		locr[p++] = loc;
	}
	else if(strcmp(opcode, "RESB") == 0){
		num_op = atoi(operand);
		loc = loc + num_op;
		locr[p++] = loc;
	}
	else if(strcmp(opcode, "BYTE") == 0){
		if(operand[0] == 'X'){
			loc+=1;
			locr[p++] = loc;
		}
		else if(operand[0] == 'C'){
			num_op = strlen(operand)-3;
			loc = loc + num_op; 
			locr[p++] = loc;
		}
	}
	else
		cout <<opcode<<" "<<isOpcode(opcode)<<":"<< "invalid operation code"<<endl;	
}
//create a symbol table
void createSymtab(){
	ifstream input("input.txt", ifstream::in);
	string line;
	char tmp[40];
	if(!input){
		cout << "input file does not exist" << endl;
		return;
	}
	
	//first line
	getline(input, line);
	strcpy(tmp, line.c_str());
	//name of the program 
	char *s = strtok(tmp, del);
	strcpy(name, s);
	//the start label 
	s = strtok(NULL, del);
	if(strcmp(s,"START") == 0){
		//location of start
		s = strtok(NULL, del);
		int hex;
		sscanf(s, "%x", &hex);//convert s to int(hex)
		loc = hex;// start of location
		start = loc;
		locr[p++] = hex;
	}
	else{
		loc = 0;
		locr[p++] = 0;
	}
	
	//read code
	while(getline(input, line)){
		strcpy(tmp, line.c_str());
		int num = calToken(line);
		s = strtok(tmp, del);
		//continue until "END"
		if(strcmp(s,"END") != 0){
			//ignore comment
			if(strcmp(s, ".") != 0){
				calLoc(s, num);
			}
		}
		else{
			//first executable instruction
			first = strtok(NULL, del);
		}
	}
	len = loc - start;
	input.close();
}
//create object code
void createObcode(){
	ifstream input("input.txt", ifstream::in);
	string line;
	int i = 1;//line
	char tmp[100];
	if(!input){
		cout << "input file does not exist" << endl;
		return;
	}
	//print
	printf("Line\tLoc\tSource statement\t\tObject code\n");
	cout << 5*i << '\t';
	i++;
	cout << hex << start <<'\t';
	
	//first line
	getline(input, line);
	strcpy(tmp, line.c_str());
	//name of the program 
	char *s = strtok(tmp, del);
	strcpy(name, s);
	//read start 
	s = strtok(NULL, del);
	if(strcmp(s,"START") == 0){
		cout << line << endl;
	}
	else{
		return;
	}
	
	for(int j = 0;j < p; j++){
		//print line
		cout << dec << 5*i << '\t';
		i++;
		char obtmp[20];
		getline(input, line);
		strcpy(tmp, line.c_str());
		s = strtok(tmp, del);
		//opcode != END
		if(strcmp(s,"END") != 0){
			//not comment
			if(strcmp(s, ".") != 0){
				//print loc
				cout << hex << uppercase << locr[j] << '\t';
				cout << line << "\t\t";
				
				//search optab for opcode 
				int num = calToken(line);
				strcpy(tmp, line.c_str());
				s = strtok(tmp, del);
				if(num == 3){
					s = strtok(NULL, del);
				}
				if(isOpcode(s) == true){
					string str(s);
					string optmp = optable[str];
					strcpy(obtmp, optmp.c_str());
					
					//search symtab for operand
					s = strtok(NULL, del);
					if(s == NULL){
						strcat(obtmp, "0000");
						obcode[j] = strupr(obtmp);
						cout << '\t' << obcode[j] << endl;
						continue;
					}
					else if(strcmp(s, "BUFFER,X") == 0){
						int k = symtable["BUFFER"] + 32768;//+8000(16)
						char ctmp[20];
						itoa(k, ctmp, 16); 
						strcat(obtmp, ctmp);
					}
					else if(inSymcode(s) == true){
						string strtmp(s);
						char ctmp[20];
						itoa(symtable[strtmp], ctmp, 16); 
						strcat(obtmp, ctmp);
					}	
				}
				else{//opcode  = "BYTE" or "WORD" or "RESW" or "RESB"
					if(strcmp(s, "BYTE") == 0){
						s = strtok(NULL, del);
						if(s[0] == 'X'){
							char stmp[20];
							stmp[0] = s[2];
							int i = 3;
							int j = 1;
							while(i < strlen(s)-1){//just copy operand
								stmp[j] = s[i];
								i++;
								j++;
							}
							strcpy(obtmp, stmp);
						}
						else if(s[0] == 'C'){//convert char to ASCII 
							int asc = s[2];
							char ctmp[20];
							itoa(asc, ctmp, 16); 
							strcpy(obtmp, ctmp);
							int i = 3;
							while(i < strlen(s)-1){
								int asc = s[i];
								char ctmp[20];
								itoa(asc, ctmp, 16); 
								strcat(obtmp, ctmp);
								i++;
							}
						}
						
					}
					else if(strcmp(s, "WORD") == 0){
						s = strtok(NULL, del);
						if(strlen(s)!=6){//insert 0 until the number of string = 6
							int hex = atoi(s);
							char ctmp[20];
							itoa(hex, ctmp, 16); 
							int insert0 = 6 - strlen(ctmp);
							strcpy(obtmp, "0");
							while(insert0 > 1){
								strcat(obtmp, "0");
								insert0--;
							}
							strcat(obtmp, ctmp);
						}
						else{
							strcpy(obtmp, s);
						}
					}
					else if(strcmp(s, "RESW") == 0 || strcmp(s, "RESB") == 0){
						s = strtok(NULL, del);
						strcpy(obtmp, "RE");
						obcode[j] = strupr(obtmp);
						cout << endl;
						continue;
					}
					
				}
				obcode[j] = strupr(obtmp);
				cout << obcode[j] << endl;
			}
			else{
				cout << line << endl;
				j--;
				continue;
			}
		}
		else{
			cout << "\t" << line << endl;
		}
	}
	
	input.close();
	
} 
//create a object file
void createObfile(){
	ofstream output("obFile.txt", ifstream::out);
	char s[1000];
	if(!output){
		cout << "error!" << endl;
		return;
	}
	//header record
	output << "H" << name << " ";
	output << setw(6) << setfill('0') << hex << uppercase << start;
	output << setw(6) << setfill('0') << hex << uppercase << len;
	output << endl;
	
	//text record
	int i = 0;
	char str[1000];
	string tmp;
	while(1){
		int begin = locr[i];
		tmp = obcode[i];
		if(strcmp(tmp.c_str(), "RE") == 0){
				i++;
				continue;
		}
		output << "T" << setw(6) << setfill('0') << hex << uppercase << locr[i];
		strcpy(str, tmp.c_str());
		i++;
		while(strlen(str)+5 < 60){//10-69
			tmp = obcode[i];
			if(strcmp(tmp.c_str(), "RE") == 0){
				i++;
				break;
			}
			strcat(str, tmp.c_str());
			i++;
			if(i > p-2)
				break;
		}
		int l = locr[i]-begin;
		output << setw(2) << setfill('0') << hex << uppercase << l;
		output << str << endl;
		if(i > p-2)
			break;
	}
	
	
	//end record
	output << "E" << setw(6) << setfill('0') << hex << uppercase << symtable[first] << endl;
	
	output.close();
}
int main(int argc, char** argv) {
	//pass 1 
	createoptab();
	createSymtab();
	//pass2
	createObcode();
	createObfile();
	return 0;

}
