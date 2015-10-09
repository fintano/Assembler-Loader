#include "20101682.h"
#include "linkedlist.h"

char LoadMap[4000];

int Pass2_LinkingLoader_H(char* record, int* length,Linkedlist *RSymbol){

	char *tmplength;
	
	listinit(RSymbol); // RSymbol :(Reference Number , Symbol)

	listinsert(RSymbol,copy(1,6,record)); // Rsymbol Name
	listinsertmore(RSymbol,"01"); // reference number

	tmplength = copy(13,18,record);
	*length = StringToHex(tmplength);

	return 0;
}

int Pass2_LinkingLoader_R(char *record, Linkedlist *RSymbol){

	int recordLength,i,theNumberOfRSymbol;
	int necessaryBlank=0;

	record[strlen(record)-1] = '\0'; // remove line feed
	recordLength = strlen(record);
	necessaryBlank = 8-((recordLength-1)%8);
	for(i=0;i<necessaryBlank;i++){ // strcat Blank 
		strcat(record," ");
	}
	theNumberOfRSymbol = (strlen(record))/8;
	/* store RSymbol in Linkedlist */
	for(i=0;i<theNumberOfRSymbol;i++){ 
		listinsert(RSymbol,copy(8*i+3,8*i+8,record)); // reference number 
		listinsertmore(RSymbol,copy(8*i+1,8*i+2,record)); // Rsymbol Name
	}
	return 0;
}
int Pass2_LinkingLoader_T(char *record){
	int TRecordLength,i,currentLocInt;
	char *currentByte,currentLocString[10];
	value tmp1,tmp2;

	currentLocInt = StringToHex(copy(1,6,record)) + csaddress; // Modification location
	TRecordLength = StringToHex(copy(7,8,record)); // Trecord length

	tmp1.exist = 1; tmp2.exist=1;
	/* Receive a Byte and load it to memory */
	for(i=0 ; i<(TRecordLength) ; i++){
		currentByte = copy(2*i+9,2*i+10,record); 
		sprintf(currentLocString,"%X",currentLocInt+(i));
		strcpy(tmp1.value,currentLocString); strcpy(tmp2.value,currentByte);
		edit(&tmp1,&tmp2);
	}
	return 0;
}

int Pass2_LinkingLoader_M(char *record, Linkedlist *RSymbol){
	
	int ModifyLocation,q,r,ModifyValueInt,sum,carry=0;
	char *ModifyValueString;
	char *externalSymbol;
	Linkedlist ex;

	ModifyLocation = StringToHex(copy(1,6,record)) + csaddress; // Modification location
	q=ModifyLocation/16;
	r=ModifyLocation%16;
	if((externalSymbol = getAddr(copy(10,11,record),*RSymbol)) == NULL){ // Get Symbol that fit in refenrence Number
			printf("%s / Undefined Reference Symbol\n",copy(10,11,record));
			return -1;
	}
	ex = external_table[MneToOp(externalSymbol)];
	if((ModifyValueString = getAddr(externalSymbol,ex)) == NULL){ //Get ADdr of Symbol using ESTAB
			printf("%s / Undefined External Symbol\n",externalSymbol);
			return -1;
	}
	ModifyValueInt = StringToHex(ModifyValueString);
	
	/* carry out Add, Sub using Carry bit and Borrow bit */
	if(record[9] == '+'){
		
		if(r+2>0xf){ r=(r+2)%16; q++;} 
		else{r+=2;}


		carry = addAndCarryCheck((int)memory[r][q],(ModifyValueInt & 0x000000FF),carry,&sum); // 5,6 bit
		memory[r][q] = sum;
		RExcess16(&q,&r);

		carry = addAndCarryCheck((int)memory[r][q],((ModifyValueInt >> 8) & 0x000000FF),carry,&sum); // 3,4 bit
		memory[r][q] = sum;
		RExcess16(&q,&r);

		if(!strcmp(copy(7,8,record),"05")){ // 1,2 bit
			carry = addAndCarryCheck((int)memory[r][q],((ModifyValueInt >> 16) & 0x0000000F),carry,&sum);
			memory[r][q] = sum;
		}
		else if(!strcmp(copy(7,8,record),"06")){
			carry = addAndCarryCheck((int)memory[r][q],((ModifyValueInt >> 16) & 0x000000FF),carry,&sum);
			memory[r][q] = sum;
		}
	}
	else{
		if(r+2>0xf){ r=(r+2)%16; q++;}
		else{r+=2;}

		carry = subAndBorrowCheck((int)memory[r][q],(ModifyValueInt & 0x000000FF),carry,&sum);
		memory[r][q] = sum;
		RExcess16(&q,&r);

		carry = subAndBorrowCheck((int)memory[r][q],((ModifyValueInt >> 8) & 0x000000FF),carry,&sum);
		memory[r][q] = sum;
		RExcess16(&q,&r);

		if(!strcmp(copy(7,8,record),"05")){
			carry = subAndBorrowCheck((int)memory[r][q],((ModifyValueInt >> 16) & 0x0000000F),carry,&sum);
			memory[r][q] = sum;
		}
		else if(!strcmp(copy(7,8,record),"06")){
			carry = subAndBorrowCheck((int)memory[r][q],((ModifyValueInt >> 16) & 0x000000FF),carry,&sum);
			memory[r][q] = sum;
		}
	}
	return 0;
}

void RExcess16(int *q, int *r){
	if((*r)-1<0){ *q--; *r=0xf;}
	else{(*r)--;}
}
int subAndBorrowCheck(int var, int var2, int borrow, int *sum){
	if((*sum = var-var2-borrow) < 0)
		return 1;
	else
		return 0;
}
int addAndCarryCheck(int var, int var2, int carry, int *sum){
	if((*sum = var+var2+carry) >= 0x100)
		return 1;
	else
		return 0;
}

int Pass2_LinkingLoader(Linkedlist *objName){


	char *filename,line[80];
	FILE *fp;
	int CSLength,errorFlag=0;
	Linkedlist RSymbol;

	csaddress = progaddress; // store progaddr to csaddress

	/* Stored filename in Linkedlist named by objName. also open it and carry out loading one by one*/
	if(LFirst(objName,&filename)){

		if((fp = fopen(filename,"rt")) == NULL ){
			printf(" File Open Error \n");
			return -1;
		}

		while(fgets(line,80,fp) != NULL){
			if(line[0] == 'H'){ // H record
				if(Pass2_LinkingLoader_H(line, &CSLength,&RSymbol) == -1)
					errorFlag = 1;
			}
			else if(line[0] == 'R'){// R record
				if(Pass2_LinkingLoader_R(line,&RSymbol) == -1)
					errorFlag = 1;
			}
			else if(line[0] == 'T'){// T record
				if(Pass2_LinkingLoader_T(line) == -1)
					errorFlag = 1;
			}
			else if(line[0] == 'M'){// M record
				if(Pass2_LinkingLoader_M(line,&RSymbol) == -1)
					errorFlag = 1;
			}
		}
		LRemove(&RSymbol);
		csaddress = csaddress + CSLength; // Next CSaddr is Current CSaddr + length
		while(LNext(objName,&filename)){
			if((fp = fopen(filename,"rt")) == NULL ){
				printf(" File Open Error \n");
				return -1;
			}

			while(fgets(line,80,fp) != NULL){
				if(line[0] == 'H'){
					if(Pass2_LinkingLoader_H(line, &CSLength,&RSymbol) == -1 )
						errorFlag = 1;
				}
				else if(line[0] == 'R'){
					if(Pass2_LinkingLoader_R(line,&RSymbol) == -1)
						errorFlag = 1;
				}
				else if(line[0] == 'T'){
					if(Pass2_LinkingLoader_T(line) == -1)
						errorFlag = 1;
				}
				else if(line[0] == 'M'){
					if(Pass2_LinkingLoader_M(line,&RSymbol) == -1)
						errorFlag = 1;
				}
			}
			LRemove(&RSymbol);
			csaddress = csaddress + CSLength;
		}
	}
	if(!errorFlag)
		return 0;
	else
		return -1;
}

int Pass1_LinkingLoader(Linkedlist *objName){
	
	char *filename,line[80];
	FILE *fp;
	int i,CSLength,errorFlag=0;

	for(i=0;i<20;i++){
		listinit(&external_table[i]);
	}

	csaddress = progaddress; // store progaddr to csaddress

	/* Stored filename in Linkedlist named by objName. also open it and carry out loading one by one*/


	if(LFirst(objName,&filename)){

		if((fp = fopen(filename,"rt")) == NULL ){
			printf(" File Open Error \n");
			return -1;
		}

		while(fgets(line,80,fp) != NULL){
			if(line[0] == 'H'){ //H record
				if(Pass1_LinkingLoader_H(line, &CSLength) == -1 )
					errorFlag = 1;
			}
			else if(line[0] == 'D'){ //D record
				if(Pass1_LinkingLoader_D(line) == -1)
					errorFlag = 1;
			}
		}
		csaddress = csaddress + CSLength; // // Next CSaddr is Current CSaddr + length
		while(LNext(objName,&filename)){
			if((fp = fopen(filename,"rt")) == NULL ){
				printf(" File Open Error \n");
				return -1;
			}

			while(fgets(line,80,fp) != NULL){
				if(line[0] == 'H'){
					if(Pass1_LinkingLoader_H(line, &CSLength) == -1 )
						errorFlag = 1;
				}
				else if(line[0] == 'D'){
					if(Pass1_LinkingLoader_D(line) == -1)
						errorFlag = 1;
				}
			}
			csaddress = csaddress + CSLength;
		}
	}
	if(!errorFlag)
		return 0;
	else
		return -1;
}

int Pass1_LinkingLoader_H(char* record, int* length){
	
	char *CSName;
	char *tmplength;
	char tmp_csaddress[7],tmp[100];

	sprintf(tmp_csaddress,"%X",csaddress);

	CSName= copy(1,6,record);
	/* store the name of control section with csaddress */
	if(LPeek(&external_table[MneToOp(CSName)],CSName)){
		printf("%s / Duplicate Error]\n",CSName);
		return -1;
	}
	listinsert(&external_table[MneToOp(CSName)],tmp_csaddress);
	listinsertmore(&external_table[MneToOp(CSName)],CSName);
	/*store CSLength */
	tmplength = copy(13,18,record);
	*length = StringToHex(tmplength);

	sprintf(tmp,"%-20s%-20s%.*d%-20s%-20s\n",CSName,"",(int)(4-strlen(tmp_csaddress)),0,tmp_csaddress,tmplength);
	strcat(LoadMap,tmp);
	return 0;
}

int Pass1_LinkingLoader_D(char* record){
	int recordLength, theNumberOfDefineSymbol,i,defineSymbolAddrInt; 
	char *defineSymbolName, *defineSymbolAddr,tmp[100];

	recordLength = strlen(record);
	theNumberOfDefineSymbol = (recordLength-1)/12;
	
	for(i=0;i<theNumberOfDefineSymbol;i++){
		defineSymbolName = copy(i*12+1,i*12+6,record);
		defineSymbolAddr = copy(i*12+7,i*12+12,record);
		
		defineSymbolAddrInt = StringToHex(defineSymbolAddr);
		defineSymbolAddrInt += csaddress ;
		sprintf(defineSymbolAddr,"%X",defineSymbolAddrInt);
		
		/* store defined symbol with Addr */
		if(LPeek(&external_table[MneToOp(defineSymbolName)],defineSymbolName)){
			printf("%s / Duplicate Error]\n",defineSymbolName);
			return -1;
		}
		listinsert(&external_table[MneToOp(defineSymbolName)],defineSymbolAddr);
		listinsertmore(&external_table[MneToOp(defineSymbolName)],defineSymbolName);

		sprintf(tmp,"%-20s%-20s%.*d%-20s\n","",defineSymbolName,(int)(4-strlen(defineSymbolAddr)),0,defineSymbolAddr);
		strcat(LoadMap,tmp);
	}
	return 0;
}

int loader(Linkedlist *ObjName){
	
	int error_flag = 0,i;
	char tmp[100];
	
	/* store loadmap information in loadmap valuable */ 
	sprintf(LoadMap,"%-20s%-20s%-20s%-20s\n","CS","SN","address","length");
	strcat(LoadMap,"-----------------------------------------------------------------\n");

	if(Pass1_LinkingLoader(ObjName)) // PASS1 
		error_flag = 1;
	if(Pass2_LinkingLoader(ObjName)) // PASS2 
		error_flag = 1;
	// I Need Error Check
	strcat(LoadMap,"-----------------------------------------------------------------\n");
	sprintf(tmp,"%-20s%-20s%-20s%-20X\n","","","total length",csaddress-progaddress);
	strcat(LoadMap,tmp);
	LRemove(ObjName);
	for(i=0;i<20;i++)
		LRemove(&external_table[i]);

	if(!error_flag){ // Print Loadmap when error did not occur
		printf("%s",LoadMap);	
		return 0;
	}
	else
		return -1;
}

int progaddr(value *para1){

	int start;

	if(para1->exist == 1 && (start = StringToHex(para1->value)) == -1)
		return -1;

	progaddress = start; // store addr received by OS
	return 0;
}
