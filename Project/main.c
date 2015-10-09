#include "main.h"


int opcode_sum=0;

int main(void){

	char order[80],line[80];
	Linkedlist list;
	Linkedlist objName;
	listinit(&objName);
	listinit(&list);
	initialProgram();

	while(1){
		printf("sicsim>");
		fgets(order,80,stdin);
		strcpy(line,order);
		order[strlen(order)-1] = '\0'; // \n 삭제
		if(manager(order, &list,&objName) != -1)
			listinsert(&list,line);
	}	
}

void initialProgram(void){
	
	int i,j;

	dumpCnt = 0;
	makeTable();
	progaddress = 0;
	for(i=0;i<16;i++){
		for(j=0;j<65536;j++){
			memory[i][j] = 0;
		}
	}
}

int DataPriorityComp(char *ch1, char *ch2)
{
	if(ch1[0] != ch2[0])
		return ch1[0]-ch2[0];
	else
		return ch1[1]-ch2[1];
}

int takeToken(char* order, value* para1, value* para2, value* para3, Linkedlist *objName){

	char *token = NULL;
	strcnt = 0;

	/* 초기화 */
	para1->exist = 0;
	para2->exist = 0;
	para3->exist = 0;

	token = strtok(order," ");
	while(token!=NULL){ 
		if(strcnt == 0){
			strcpy(order,token);
			if(!strcmp(order,"loader")){ // loader일 경우 objName에 넣기
				while((token = strtok(NULL," "))!=NULL){
					listinsert(objName,token);
				}
				break;
			}
		}
		else if(strcnt == 1){
			strcpy(para1->value,token);
			para1->exist = 1;
		}
		else if(strcnt == 2){
			strcpy(para2->value,token);
			para2->exist = 1;
		}
		else if(strcnt == 3){
			strcpy(para3->value,token);
			para3->exist = 1;
		}
		token = strtok(NULL," ");
		strcnt++;
	}	
	if((strcmp(order,"du")==0 || strcmp(order,"dump") == 0) && (para1->exist == 1 && para1->value[strlen(para1->value)-1] == ',' ) && para2->exist == 0){ // du 4, exception handling
		printf("Need more parameters ! \n");
		return -1;
	}
	if(para1->exist == 1 && para1->value[strlen(para1->value)-1] != ','){ // ,로 붙어있는것들 처리
		token = strtok(para1->value,",");
		strcnt--;
		while(token!=NULL){

			if(strcnt == 1) 
				strcpy(para1->value,token);
			else if(strcnt == 2){
				strcpy(para2->value,token);
				para2->exist = 1;
			}
			else if(strcnt == 3){
				strcpy(para3->value,token);
				para3->exist = 1;
			}
			token = strtok(NULL,",");
			strcnt++;
		}
	}
	else{ // , 빼고 저장
		if(strcnt >= 3){
			para1->value[strlen(para1->value)-1] = '\0';
		}
		if(strcnt >= 4){
			para2->value[strlen(para2->value)-1] = '\0';
		}
	}
	if(strlen(para1->value) == 0)
		para1->exist = 0;
	return 0;
}

int manager(char* order, Linkedlist *list, Linkedlist *objName){

	value para1,para2,para3;

	if((takeToken(order,&para1,&para2,&para3,objName)) == -1)
			return -1;

	if(strcmp(order,"help") == 0 || strcmp(order,"h")==0){
		if(strcnt > 1){
			printf("excess input\n");
			return -1;
		}
		help();
	}
	else if(strcmp(order,"d") == 0 || strcmp(order,"dir") ==0){
		if(strcnt > 1){
			printf("excess input\n");
			return -1;
		}
			dir();	
	}
	else if(strcmp(order,"hi") == 0 || strcmp(order,"history") == 0){ 
		if(strcnt > 1){
			printf("excess input\n");
			return -1;
		}
		history(order,list);
	}
	else if(strcmp(order,"q") == 0 || strcmp(order,"quit") ==0){
		if(strcnt > 1){
			printf("excess input\n");
			return -1;
		}
		printf("Terminate out program.\n ");
		exit(-1);
	}
	else if(strcmp(order,"dump") == 0 || strcmp(order,"du") == 0){
		if(strcnt > 3){
			printf("excess input\n");
			return -1;
		}
		return dump(&para1, &para2);
	}
	else if(strcmp(order,"edit") == 0 || strcmp(order,"e") == 0){
		if(strcnt > 3){
			printf("excess input\n");
			return -1;
		}
		return edit(&para1,&para2);
	}
	else if(strcmp(order,"fill") == 0 || strcmp(order,"f") == 0){
		if(strcnt > 4){
			printf("excess input\n");
			return -1;
		}
		return fill(&para1,&para2,&para3);
	}
	else if(strcmp(order,"reset") == 0){
		if(strcnt > 1){
			printf("excess input\n");
			return -1;
		}
		reset();
	}
	else if(strcmp(order,"opcode") == 0){
		if(strcnt > 2){
			printf("excess input\n");
			return -1;
		}
		return opcode(&para1);
	}
	else if(strcmp(order,"mnemonic") == 0){
		if(strcnt > 2){
			printf("excess input\n");
			return -1;
		}
		return mnemonic(&para1);
	}
	else if(strcmp(order,"opcodelist") == 0)
		opcodeList();
	else if(strcmp(order,"mnemoniclist") == 0)
		mnemonicList();
	else if(strcmp(order,"type") == 0)
		type(&para1);
	else if(strcmp(order,"assemble") == 0){
		return assemble(&para1);
	}
	else if(!strcmp(order,"symbol"))
		symbol();
	else if(!strcmp(order,"progaddr"))
		progaddr(&para1);
	else if(!strcmp(order,"loader"))
		loader(objName);
	else{
		printf("Wrong instruction\n");
		return -1;
	}

	return 0;
}

void symbol(void){

	Heap heap;
	int i;
	Data data;
	HeapInit(&heap,DataPriorityComp); //DataPriorityComp 함수는 우선순위 비교 함수

	for(i = 0;i<20;i++){
		if(LFirst(&symbol_table[i],&data)){
			data = findValue(&symbol_table[i]);
			HInsert(&heap,data);
			while(LNext(&symbol_table[i],&data)){
				data = findValue(&symbol_table[i]);
				HInsert(&heap,data);
			}
		}
	}

	while(!HIsEmpty(&heap)){
		data = HDelete(&heap);
		printf("%10s%10s\n",data,getAddr(data,symbol_table[MneToOp(data)]));
	}
}
void type(value* filename){

	FILE *fp;
	char line[80];

	if((fp = fopen(filename->value,"rt")) == NULL){
		printf(" file open error \b");
		exit(-1);
	}

	while(1){
		if(fgets(line,80,fp) == NULL)
			break;
		printf("%s",line);
	}
}

int assemble(value* filename){

	char *token;

	if(filename->exist == 0)
		printf("Need More parameters ! \n");

	token = strtok(filename->value,".");
	if((token = strtok(NULL,".")) != NULL){
		if(strcmp(token,"asm") != 0)
			printf("It's not asm file \n");
	}

	if(Pass1(filename->value) == -1 || Pass2(filename->value))
		return -1;
	return 0;
} 
int Pass2(char* filename){

	FILE *fp,*fp2,*fp3;
	char line[80],line2[10], *ptr, *token, *label, *mnemonic, *operand,*opcode,*operandAddr,fullOpcode[9],*filename_obj;
	int i,tokenCnt,isSymbol,isX=0;
	char opcode_flag=1,startAddr[7];

	/* XE Valuables */

	int indirect=0, immediate=0, base_rala=0, pc_rela=0,extended=0, litr = 0;
	int PC;
	int BaseAddr;
	int format3=0;
	int lineNum = 0;
	char *restore_operand; //buff
	char restore_mnemonic[20]; 
	char temp_fullOpcode[9];
	char *Mlist_str;
	Linkedlist Mlist;
	Linkedlist Llist;
	Linkedlist list;

	listinit(&Mlist);
	listinit(&Llist);

	if( (fp2=fopen(filename,"rt")) == NULL){ // filename.asm
		printf("file open error \n");
		exit(-1);
	}

	strtok(filename,".");
	strcat(filename,".lst");
	printf("output file : [%s], ",filename);
	if( (fp=fopen(filename,"wt")) == NULL){ // filename.lst
		printf("file open error \n");
		exit(-1);
	}
	filename_obj = (char*)malloc(sizeof(filename)+5);
	strcpy(filename_obj,filename);
	strtok(filename_obj,".");
	strcat(filename_obj,".obj");
	printf("[%s]\n",filename_obj);
	if( (fp4=fopen(filename_obj,"wt")) == NULL){ // filename.obj
		printf("file open error \n");
		exit(-1);
	}

	if( (fp3=fopen("intermediate.txt","rt")) == NULL){ // intermediate.txt
		printf("file open error \n");
		exit(-1);
	}

	/*starting line */

	tokenCnt = 0;
	if(fgets(line,80,fp2) == NULL || fgets(line2,10,fp3) == NULL){
		printf("file open error \n");
		exit(-1);
	}
	line2[strlen(line2)-1] = '\0';
	ptr = line;
	isSymbol =1;
	while( (token = strtok(ptr," \t\n")) != NULL){
		if(token[0] == '.')
			break;

		if(isSymbol == 1){
			label = token;
			isSymbol = 0;
		}
		else{
			if(tokenCnt == 0)
				mnemonic = token;
			else if(tokenCnt == 1)
				operand= token;
			tokenCnt ++;
		}
		ptr = NULL;
	}
	strcpy(startAddr,operand);
	fprintf(fp,"%d\t%10s\t%-10s\t%-10s\t%-10s\n",(lineNum+=5),line2,label,mnemonic,operand);
	/* Write Header Record */
	for( i=0 ; i<= 6-strlen(label) ;i++)
		strcat(label," ");
	fprintf(fp4,"H%s%.*d%s%06X\n",label,(int)(6-strlen(operand)), 0, operand, length);

	/*Read next lines */
	while(1){
		tokenCnt = 0;
		if(fgets(line,80,fp2) == NULL || fgets(line2,10,fp3) == NULL){
			printf("file open error \n");
			exit(-1);
		}
		line2[strlen(line2)-1] = '\0';
		ptr = line;
		if(line[0] == ' ')
			isSymbol = 0;
		else
			isSymbol = 1;

		if(line[0] == '.')
			continue;

		label = " ";
		while( (token = strtok(ptr," \t\n")) != NULL){

			if(token[0] == '.')
				break;

			if(isSymbol == 1){
				label = token;
				isSymbol = 0;
			}
			else{
				if(tokenCnt == 0)
					mnemonic = token;
				else if(tokenCnt == 1)
					operand= token;
				else if(tokenCnt == 2)
					strcat(operand,token);
				tokenCnt ++;
			}

			ptr = NULL;
		}
		if(tokenCnt == 1)
			operand = " ";
		/* label, mnemonic, operand 받기 완료*/
		/* back up*/
		restore_operand = (char*)malloc(sizeof(operand)+20);
		strcpy(restore_operand,operand);
		strcpy(restore_mnemonic,mnemonic);

		/* END 일 때 record 입력후 종료 */
		if(!strcmp(mnemonic,"END")){
				/* End 출력 */
			fprintf(fp,"%d\t%-20s\t\t%-10s\t%-10s\n",(lineNum+=5),"",mnemonic,operand);
			printLiteral(Llist,fp,fp3,line2,&opcode_flag,mnemonic);
			fprintf(fp4,"%02X",opcode_sum/2);
			fprintf(fp4,"%s\n",record);
			break;
		}
		/* format 4 */
		if(mnemonic[0] == '+'){
			extended = 1;
			mnemonic = strtok(mnemonic,"+");
		}

		if( (opcode = getAddr(mnemonic,opcode_table[MneToOp(mnemonic)])) != NULL){ // OPTAB에서 opcode 찾아내는 작업
			int tmpHex; 
			char* temp_operandAddr,*string_opcode;

			//strcpy(fullOpcode,opcode);
			
			/* format 2*/
			list = opcode_table[MneToOp(mnemonic)];
			if(!strcmp(getData(mnemonic,list),"2")){ 
				char *mnemonic_1, *mnemonic_2;
				
				mnemonic_1 = strtok(operand,",");
				mnemonic_1 = registerTr(mnemonic_1);
				if((mnemonic_2 = strtok(NULL,",")) == NULL)
					mnemonic_2 = "0";
				else
				{
					mnemonic_2 = registerTr(mnemonic_2);
				}
				strcpy(fullOpcode,opcode);
				strcat(fullOpcode,mnemonic_1);
				strcat(fullOpcode,mnemonic_2);
			}
			/* format 3/4 */
			else{ 

				int int_opcode,int_operand;

				sscanf(line2,"%X",&PC);
				sscanf(opcode,"%X",&int_opcode);

				if(operand[strlen(operand)-2] == ','){ //indexed addressing check
					strtok(operand,",");
					isX=1;
				}
				else if(operand[0] == '#'){ // immediate
					immediate = 1;
					operand = strtok(operand,"#");
				}
				else if(operand[0] == '@'){ // indirect
					indirect = 1;
					operand = strtok(operand,"@");
				}
				else if(operand[0] == '='){ // literal
					litr = 1;
					if(!LPeek(&Llist,operand)){
						listinsert(&Llist,operand);
						listinsertmore(&Llist,operand);
					}
				}

				/* operand에 해당하는 주소 값 받기 */

				if(!isdigit(operand[0])){ // 숫자면 Pass (imm)
					if(tokenCnt == 1) // operand가 없을 경우 
						operandAddr = "0000";
					else if(litr && (operandAddr = getAddr(operand,literal_table[MneToOp(operand)])) == NULL){ // 이것도 중복?
						printf("[Location %s / %s %s %s / Undifined literal]\n",line2,label,mnemonic,operand);

						/* 에러시 지금까지 쓰였던 것들 다 없앰 */

						fopen(filename,"wt"); 
						fopen(filename_obj,"wt");
						for(i=0;i<20;i++) // 그 전의 symbol_table 지우는 작업
							LRemove(&symbol_table[i]);
						return -1;
					}
					else if(!litr && (operandAddr = getAddr(operand,symbol_table[MneToOp(operand)])) == NULL){   
						printf("[Location %s / %s %s %s / Undifined symbol]\n",line2,label,mnemonic,operand);

						/* 에러시 지금까지 쓰였던 것들 다 없앰 */

						fopen(filename,"wt"); 
						fopen(filename_obj,"wt");
						for(i=0;i<20;i++) // 그 전의 symbol_table 지우는 작업
							LRemove(&symbol_table[i]);
						return -1;
					}
					sscanf(operandAddr,"%X",&tmpHex);
				}

				if(isX){ //indexed addressing

					if(!extended){
						PC+=3;
						if((tmpHex-PC) > 2048 || (tmpHex-PC) < -2048){ // base
							tmpHex -= BaseAddr;
							base_rala = 1;
						}
						else{
							tmpHex -= PC; // disp
							pc_rela = 1;
						}
					}
					indirect = 1; immediate = 1; // n,i
					strcat(operand,",X");
				}
				else if(immediate){ // immediate

					if(isdigit(operand[0])){
						sscanf(operand,"%d",&int_operand);
						tmpHex = int_operand;
					}
					else{
						if(!extended){
							PC+=3;

							if((tmpHex-PC) > 2048 || (tmpHex-PC) < -2048){ // base
								tmpHex -= BaseAddr;
								base_rala = 1;
							}
							else{
								tmpHex -= PC; // disp
								pc_rela = 1;
							}
						}
					}
				}
				else if(indirect){ // indirect
					if(!extended){
						PC+=3;
						if((tmpHex-PC) > 2048 || (tmpHex-PC) < -2048){ // base
							tmpHex -= BaseAddr;
							base_rala = 1;
						}
						else{
							tmpHex -= PC; // disp
							pc_rela = 1;
						}
					}
				}
				else if(litr){ // literal
					if(!extended){
						PC+=3;
						if((tmpHex-PC) > 2048 || (tmpHex-PC) < -2048){ // base
							tmpHex -= BaseAddr;
							base_rala = 1;
						}
						else{
							tmpHex -= PC; // disp
							pc_rela = 1;
						}
					}
					immediate =1; indirect =1;
				}
				else{ // left
					if(!extended && tokenCnt != 1){
						PC+=3;
						if((tmpHex-PC) > 2048 || (tmpHex-PC) < -2048){ // base
							tmpHex -= BaseAddr;
							base_rala = 1;
						}
						else{
							tmpHex -= PC; // disp
							pc_rela = 1;
						}
					}
					immediate =1; indirect =1;
				}
				
				/* flag 포함 */
				if(extended){
					tmpHex &= 0xfffff;
					tmpHex = tmpHex | (isX<<23) | (base_rala<<22)|(pc_rela<<21)|(extended<<20);
				}
				else{
					tmpHex &= 0xfff;
					tmpHex = tmpHex | (isX<<15) | (base_rala<<14)|(pc_rela<<13)|(extended<<12);
				}

				int_opcode = int_opcode | (indirect<<1) | (immediate);

				temp_operandAddr = (char*)malloc((extended)? sizeof(operandAddr)+3 :sizeof(operandAddr)+1);
				sprintf(temp_operandAddr,"%04X",tmpHex);
				string_opcode = (char*)malloc(sizeof(opcode)+sizeof(operandAddr)+1);
				sprintf(string_opcode,"%X",int_opcode);
				strcat(string_opcode,temp_operandAddr);
				strcpy(fullOpcode,string_opcode);

				free(temp_operandAddr);
				free(string_opcode);
				if(!extended)
					format3 = 1;
			}
		}
		else if (strcmp(mnemonic,"BYTE") == 0){

			strcpy(fullOpcode,sendByteString(operand));

		}
		else if(strcmp(mnemonic,"WORD") == 0){
			char tmp_string[7];
			int tmp_int;

			sscanf(operand,"%d",&tmp_int);
			sprintf(tmp_string,"%06X",tmp_int);
			strcpy(fullOpcode,tmp_string);

		}
		else if(strcmp(mnemonic,"RESW") ==0 || strcmp(mnemonic,"RESB") == 0){
			strcpy(fullOpcode," ");
		}
		else if(!strcmp(mnemonic,"BASE")){
			sscanf(getAddr(operand,symbol_table[MneToOp(operand)]),"%X",&BaseAddr); // base값 int로 저장
			strcpy(line2,"");
			strcpy(fullOpcode,"");
		}
		else if(!strcmp(mnemonic,"LTORG")){
			printLiteral(Llist,fp,fp3,line2,&opcode_flag,mnemonic);
			LRemove(&Llist);
			continue;
		}
		else if(!strcmp(mnemonic,"ORG")){
			strcpy(line2,"");
		}
		/* assemble 완료 */

		/* operand 6자리 맞춰주기 */
		if(format3){
			sprintf(temp_fullOpcode,"%.*d",(int)(6-strlen(fullOpcode)),0);
			strcat(temp_fullOpcode,fullOpcode);
			strcpy(fullOpcode,temp_fullOpcode);
			format3 = 0;
		}
		/* + 복원 */
		operand = restore_operand;
		mnemonic = restore_mnemonic;
		
		fprintf(fp,"%d\t%10s\t%-10s\t%-10s\t%-10s\t%-10s\n",(lineNum+=5),line2,label,mnemonic,operand,fullOpcode);
		indirect=0, immediate=0, base_rala=0, isX = 0, pc_rela=0, extended=0; litr = 0;// flag 초기화 
		free(restore_operand);

		/* object code 입력 */

		if(!strcmp(fullOpcode,""))
			continue;

		Trecord(&opcode_flag,mnemonic,fullOpcode,line2);

		/* store M record */
		if(extended && operand[0] != '#'){
			char MRecord[10],modifyingLoc_str[7];
			int modifyingLoc_int = PC+1;

			strcpy(MRecord,"M");
			sprintf(modifyingLoc_str,"%06X",modifyingLoc_int);
			strcat(MRecord,modifyingLoc_str);

			strcat(MRecord,"05");
			listinsert(&Mlist,MRecord);
		}
	}

	/* M record */
	if(LFirst(&Mlist,&Mlist_str)){
		fprintf(fp4,"%s\n",Mlist_str);
		while(LNext(&Mlist,&Mlist_str)){
			fprintf(fp4,"%s\n",Mlist_str);
		}
	}
	/* E record */
	fprintf(fp4,"E%.*d%s\n",(int)(6-strlen(startAddr)),0,startAddr);
	
	free(filename_obj);
	fclose(fp);
	fclose(fp2);
	fclose(fp3);
	fclose(fp4);
	return 0;
}

void Trecord(char *opcode_flag, char *mnemonic, char *fullOpcode, char* line2){
		if(*opcode_flag == 0){
			if(strcmp(mnemonic,"RESW") ==0 || strcmp(mnemonic,"RESB") == 0 || (opcode_sum+strlen(fullOpcode))/2 > 0x1E ){
				fprintf(fp4,"%02X",opcode_sum/2);
				fprintf(fp4,"%s\n",record);
				*opcode_flag =1;
			}
			else{
				strcat(record,fullOpcode);
				opcode_sum += strlen(fullOpcode);
			}
		}
		if(*opcode_flag == 1 && strcmp(mnemonic,"RESW") && strcmp(mnemonic,"RESB") && strcmp(mnemonic,"ORG") && strcmp(mnemonic,"EQU")){
			/*T record 처음 부분 */
			opcode_sum = 0;
			fprintf(fp4,"T%.*d%s",(int)(6-strlen(line2)),0,line2);
			strcpy(record,fullOpcode);
			opcode_sum += strlen(fullOpcode);
			*opcode_flag = 0;
		}
}


void printLiteral(Linkedlist Llist,FILE *printLine, FILE *getLine, char *line2, char* opcode_flag, char *mnemonic){
	char *literal_str;
	char *temp_literal_str;

	if(LFirst(&Llist,&literal_str)){
		temp_literal_str = (char*)malloc(sizeof(literal_str)+1);
		strcpy(temp_literal_str,literal_str);
		strcpy(temp_literal_str,sendByteString(temp_literal_str));
		fprintf(printLine,"\t%10s\t%-10s\t%-10s\t%-10s\t%-10s\n",line2,"*",literal_str,"",temp_literal_str);
		Trecord(opcode_flag,mnemonic,temp_literal_str,line2);
		while(LNext(&Llist,&literal_str)){
			temp_literal_str = (char*)malloc(sizeof(literal_str)+1);
			strcpy(temp_literal_str,literal_str);
			strcpy(temp_literal_str,sendByteString(temp_literal_str));
			if(fgets(line2,10,getLine) == NULL){
				printf(" No data \n");
				exit(-1);
			}
			line2[strlen(line2)-1] = '\0';
			fprintf(printLine,"\t%10s\t%-10s\t%-10s\t%-10s\t%-10s\n",line2,"*",literal_str,"",temp_literal_str);
			Trecord(opcode_flag,mnemonic,temp_literal_str,line2);
		}
	}
}

Data sendByteString(char *operand){

	char* token;
	//char BYTE_operand[20];

	//strcpy(BYTE_operand,operand);
	token = strtok(operand,"='");

	if(token[0] == 'C'){ // C 형식 처리 

		char tmp[3],*tmp_string;
		int i;
		token = strtok(NULL,"'");
		tmp_string = (char*)malloc(sizeof(operand)*2+1);
		for(i=0;i<strlen(token);i++){
			sprintf(tmp,"%X",token[i]);
			if(i==0)
				strcpy(tmp_string,tmp);
			else
				strcat(tmp_string,tmp);
		}
		//tmp_string[2*i] = '\0';
		//operand = BYTE_operand;
		return tmp_string;
	}
	else if(token[0] == 'X'){ //X 형식 처리
		token = strtok(NULL,"'");
		//operand=BYTE_operand;
		return token;
	}
	printf("[%s / Neither C nor X]\n",operand);
	exit(-1);
}


Data registerTr(char* mnemonic){
	if(!strcmp(mnemonic,"A"))
		return "0";
	else if(!strcmp(mnemonic,"X"))
		return "1";
	else if(!strcmp(mnemonic,"L"))
		return "2";
	else if(!strcmp(mnemonic,"B"))
		return "3";
	else if(!strcmp(mnemonic,"S"))
		return "4";
	else if(!strcmp(mnemonic,"T"))
		return "5";
	else if(!strcmp(mnemonic,"F"))
		return "6";
	else if(!strcmp(mnemonic,"PC"))
		return "8";
	else if(!strcmp(mnemonic,"SW"))
		return "9";
	else
		return NULL;
}

/* list의 Addr이 아닌 다른 Data를 얻는 함수 */

Data getData(char* operand, Linkedlist list){  

	Data addr;

	if(LFirst(&list,&addr)){
		if( strcmp(findValue(&list),operand) == 0){
			return findFormat(&list);
		}
		while(LNext(&list,&addr)){
			if(strcmp(findValue(&list),operand) == 0){
				return findFormat(&list);
			}
		}
	}
	return NULL;
}
/* list에서 operand에 해당하는 Addr을 얻는 함수 */
Data getAddr(char* operand, Linkedlist list){

	Data addr;
 
	if(LFirst(&list,&addr)){
		if( strcmp(findValue(&list),operand) == 0){
			return addr;
		}
		while(LNext(&list,&addr)){
			if(strcmp(findValue(&list),operand) == 0){
				return addr;
			}
		}
	}
	return NULL;
}


int Pass1(char* filename){

	FILE *fp,*fp2;
	char *token, line[80], *ptr,loc_string[10];
	int i,loc_int=0,isSymbol,tokenCnt=0,operand_int,startingAddr,ProgramLength,error_flag =0;
	char *label,*opcode,*operand_string;

	/* XE */
	int isformat4=0;
	int EQUExpression;
	int EQUErrorflag;
	int ORGFlag = 0;
	int originalAddr;
	Linkedlist Llist;

	Linkedlist list;

	listinit(&Llist);
	for(i=0;i<20;i++) // 그 전의 symbol_table 지우는 작업
		LRemove(&symbol_table[i]);

	strcat(filename,".asm");	
	if( ((fp = fopen(filename,"rt")) == NULL) || ((fp2=fopen("intermediate.txt","wt"))== NULL)){
		printf("file open error \n");
		exit(-1);
	}

	fgets(line,80,fp);
	ptr = line;
	tokenCnt = 0;
	/*  Read starting line */
	while( (token = strtok(ptr," \t\n")) != NULL){

		if(tokenCnt == 1)
			opcode = token;
		else if(tokenCnt == 2){
			sscanf(token,"%X",&loc_int);
			fprintf(fp2,"%04X\n",loc_int);
			startingAddr = loc_int;
		}
		ptr = NULL;
		tokenCnt++;
	}
	/* Read next lines */
	do{
		tokenCnt = 0;
		if(fgets(line,80,fp) == NULL)
			break;
		ptr = line;
		if(line[0] == ' ')
			isSymbol = 0;
		else
			isSymbol = 1;

		if(line[0] == '.'){
			fprintf(fp2,"%04X\n",loc_int);
			continue;
		}
		label = NULL;
		while( (token = strtok(ptr," \t\n")) != NULL){

			if(token[0] == '.') // 주석 처리
				break;

			if(isSymbol == 1){ // label이 존재하는가
				label = token;
				isSymbol = 0;
			}
			else{
				if(tokenCnt == 0)
					opcode = token;
				else if(tokenCnt == 1)
					operand_string = token;
				tokenCnt ++;
			}

			ptr = NULL;
		}

		/* SYMTAB에 symbol을 넣는 작업 */
			

		if(label != NULL){

			if(LPeek(&symbol_table[MneToOp(label)],label)){
				printf("[Location %X / %s %s %s / Duplicate Error]\n",loc_int,label,opcode,operand_string);
				error_flag = 1;
			}
			else{

				/* decide type of symbols to A,R */
				if(!strcmp(opcode,"EQU")){
					EQUExpression = loc_int;
					EQUErrorflag = EQU(operand_string,&EQUExpression);
					fprintf(fp2,"%04X\n",EQUExpression);
					sprintf(loc_string,"%X",EQUExpression);
					listinsert(&symbol_table[MneToOp(label)],loc_string);
					listinsertmore(&symbol_table[MneToOp(label)],label);
					if(EQUErrorflag == -1){
						printf("[Location %X / %s %s %s / EQU Error]\n",loc_int,label,opcode,operand_string);
						error_flag = 1;
					}
					else if(EQUErrorflag == 1)
						listinsertformat(&symbol_table[MneToOp(label)],"R");
					else
						listinsertformat(&symbol_table[MneToOp(label)],"A");
					continue;
				}
				else if(operand_string[0] == '#' && isdigit(operand_string[1])){
					sprintf(loc_string,"%X",loc_int);
					listinsert(&symbol_table[MneToOp(label)],loc_string);
					listinsertmore(&symbol_table[MneToOp(label)],label);
					listinsertformat(&symbol_table[MneToOp(label)],"A");
				}
				else{
					sprintf(loc_string,"%X",loc_int);
					listinsert(&symbol_table[MneToOp(label)],loc_string);
					listinsertmore(&symbol_table[MneToOp(label)],label);
					listinsertformat(&symbol_table[MneToOp(label)],"R");
				}
			}
		}

		if(operand_string[0] == '='){
			listinsert(&Llist,operand_string);
		}

		fprintf(fp2,"%04X\n",loc_int);

		if(opcode[0] == '+'){
			isformat4 = 1;
			opcode = strtok(opcode,"+");
		}

		/* OPTAB에 opcode를 찾는 작업 */
		list = opcode_table[MneToOp(opcode)]; 
		if(LPeek(&list,opcode)){
			char* temp_format;
			list = opcode_table[MneToOp(opcode)]; 
			temp_format = getData(opcode,list);

			if(!strcmp(temp_format,"1"))
				loc_int += 1;
			else if(!strcmp(temp_format,"2"))
				loc_int += 2;
			else if(isformat4 == 1){
				loc_int += 4;
				isformat4 = 0;
			}
			else
				loc_int += 3;

		}
		else if(strcmp(opcode,"WORD")==0)
			loc_int += 3;
		else if(strcmp(opcode,"RESB")==0){
			sscanf(operand_string,"%d",&operand_int);
			loc_int += operand_int;
		}
		else if(strcmp(opcode,"RESW")==0){
			sscanf(operand_string,"%d",&operand_int);
			loc_int += (operand_int*3);
		}
		else if(strcmp(opcode,"BYTE")==0){
			token = strtok(operand_string,"'");
			if(operand_string[0] == 'C'){ // C 형식 처리 
				token = strtok(NULL,"'");
				loc_int += (strlen(token));
			}
			else if(operand_string[0] == 'X'){ //X 형식 처리
				token = strtok(NULL,"'");
				loc_int += (strlen(token)/2);
			}
		}
		else if(!strcmp(opcode,"BASE")){
			; // 나중에 다시 처리하기 
		}
		else if(strcmp(opcode,"END")==0){
			putLiteral(Llist,&loc_int);
			ProgramLength = loc_int - startingAddr;
			fprintf(fp2,"%X\n",ProgramLength);
			break;
		}
		/* LITTAB에 literal을 넣는 작업 */
		else if(!strcmp(opcode,"LTORG") || !strcmp(opcode,"END")){
			putLiteral(Llist,&loc_int);
		}
		else if(!strcmp(opcode,"ORG")){
			ORG(operand_string,&ORGFlag,&loc_int,&originalAddr);
		}
		else{
			printf("[Location %X / %s %s %s / Cannot find the opcode ]\n",loc_int,label,opcode,operand_string);
			error_flag = 1;
		}
	}while(strcmp(opcode,"END") != 0);
	length = loc_int - startingAddr;
	if(error_flag){
		for(i=0;i<20;i++) // 오류시 symbol_table 지우는 작업
			LRemove(&symbol_table[i]);
		return -1;
	}
	fclose(fp2);
	fclose(fp);
	return 0;
}
void ORG(char *operand, int *ORGFlag, int *loc_int, int *originalAddr){

	int newLoc;

	if(!(*ORGFlag)){
		*originalAddr = *loc_int;
		sscanf(getAddr(operand,symbol_table[MneToOp(operand)]),"%X",&newLoc);
		*loc_int = newLoc;
		*ORGFlag = 1;
	}
	else{	
		*loc_int = *originalAddr; 
		*ORGFlag = 0;
	}
}
int EQU(char* operand, int *expression){
	
	/* 0 : Absolute 1 :relative */
	int i,j,int_token,preToken; 
	int positiveRelativeCnt = 0;
	int negativeRelativeCnt = 0;
	int relativeCnt;
	char token[10];
	char preOperand = '+', preMoreOperand;
	Linkedlist list;

	if(!strcmp(operand,"*")){
		return 1;
	}
	else if(isStringDouble(operand)){ // operand가 constant 일때 absulute return
		return 0;
	}
	else{ //operand가 expression일 때
		*expression = 0;
		for(i=0,j=0;i<=strlen(operand);i++,j++){
			if(i == strlen(operand) || (operand[i] == '+' || operand[i] == '-' || operand[i] == '*' || operand[i] == '/' )){
				
				token[j] = '\0';
				j=-1;
				if(isStringDouble(token)){ // token이 숫자면
					sscanf(token,"%X",&int_token);
					if((operand[i] == '*' || operand[i] == '/')){
						preMoreOperand = preOperand;
						preToken = int_token;
					}
					else{
						*expression+=int_token;
					}
					preOperand = operand[i]; // 연산자 저장
					continue;
				}
				if(!LPeek(&symbol_table[MneToOp(token)],token)){ //token이 SYMTAB에 존재하지 않으면
					return -1;
				}
				list = symbol_table[MneToOp(token)];
				
				if(!strcmp(getData(token,list),"R")){ //token이 Relative이면
					
					sscanf(getAddr(token,list),"%X",&int_token);

					if(preOperand == '*' || preOperand == '/'){
						return -1;
					}
					else if(preOperand == '+'){
						*expression+=int_token;
						positiveRelativeCnt++;
					}
					else if(preOperand == '-'){
						*expression-=int_token;
						negativeRelativeCnt++;
					}
				}
				else{ // token이 Absolute이면

					sscanf(getAddr(token,list),"%X",&int_token);

					if(preOperand == '*' || preOperand == '/'){
						if(preMoreOperand == '+'){
							if(preOperand == '*') *expression += (preToken * int_token);
							else *expression += (preToken / int_token );
						}
						else{
							if(preOperand == '*') *expression += (preToken * int_token);
							else *expression += (preToken / int_token );
						}
					}
					else if(preOperand == '+'){
						*expression+=int_token;
					}
					else if(preOperand == '-'){
						*expression-=int_token;
					}
				}
				preOperand = operand[i]; // 연산자 저장
			}
			else
				token[j] = operand[i]; // 한글자씩 받기
		}
		relativeCnt = positiveRelativeCnt - negativeRelativeCnt ;

		if(relativeCnt == 0){
			return 0;
		}
		else if(relativeCnt == 1){
			return 1;
		}
		else{
			return -1;
		}
	}
}

/* 문자열이 숫자인지 판단 하는 함수 */

int isStringDouble(char *s) {

	int i;
	size_t size = strlen(s);

	if (size == 0) return 0; // 0바이트 문자열은 숫자가 아님

	for (i = 0; i < (int)size; i++) {
		if (s[i] == '.' || s[i] == '-' || s[i] == '+') continue;
		if (s[i] < '0' || s[i] > '9') return 0; // 알파벳 등이 있으면 숫자 아님
	}
	return 1; // 그밖의 경우는 숫자임
}

void putLiteral(Linkedlist Llist,int *loc_int){

	char *literalOperand;
	char *tmp_literalOperand;
	char *token,loc_string[10];
	int operandLen;

	if(LFirst(&Llist,&literalOperand)){
		tmp_literalOperand = (char*)malloc(sizeof(literalOperand)+1);
		strcpy(tmp_literalOperand,literalOperand);
		token = strtok(tmp_literalOperand,"='");
		if(token[0] == 'C'){ // C 형식 처리 
			token = strtok(NULL,"'");
			operandLen = (strlen(token));
		}
		else if(token[0] == 'X'){ //X 형식 처리
			token = strtok(NULL,"'");
			operandLen = (strlen(token)/2);
		}
		if(!LPeek(&literal_table[MneToOp(literalOperand)],literalOperand)){
			sprintf(loc_string,"%X",*loc_int);
			listinsert(&literal_table[MneToOp(literalOperand)],loc_string);
			listinsertmore(&literal_table[MneToOp(literalOperand)],literalOperand);
			*loc_int += operandLen;
		}

		while(LNext(&Llist,&literalOperand)){
			tmp_literalOperand = (char*)malloc(sizeof(literalOperand)+1);
			strcpy(tmp_literalOperand,literalOperand);
			token = strtok(tmp_literalOperand,"='");
			if(token[0] == 'C'){ // C 형식 처리 
				token = strtok(NULL,"'");
				operandLen = (strlen(token));
			}
			else if(token[0] == 'X'){ //X 형식 처리
				token = strtok(NULL,"'");
				operandLen = (strlen(token)/2);
			}
			if(!LPeek(&literal_table[MneToOp(literalOperand)],literalOperand)){
				sprintf(loc_string,"%X",*loc_int);
				listinsert(&literal_table[MneToOp(literalOperand)],loc_string);
				listinsertmore(&literal_table[MneToOp(literalOperand)],literalOperand);
				*loc_int += operandLen;	
			}
		}			
	}
}

char* copy(int start, int end, char* array){

	int i,j;
	char* selected = (char*)malloc(sizeof(char)*(end-start+2));

	for(i=start,j=0;i<=end;i++,j++)
		selected[j] = array[i];
	selected[j]='\0';
	return selected;
}


int opcode(value* para1){

	char *opcode;
	char mnemonic[10];
	Linkedlist list;	

	if(para1->exist == 0){
		printf("Need more parameters ! \n");
		return -1;
	}

	strcpy(mnemonic,(para1->value));
	list = opcode_table[MneToOp(mnemonic)]; 
	if(LFirst(&list,&opcode)){
		if( strcmp(findValue(&list),mnemonic) == 0){
			printf("opcode is %s\n",opcode);
			return 0;
		}
		while(LNext(&list,&opcode)){
			if(strcmp(findValue(&list),mnemonic) == 0){
				printf("opcode is %s\n",opcode);
				return 0;
			}
		}
	}
	return -1;
}

/* 함수 : mnemonic(value* para1) */
/* 목적 : opcode를 입력하면 해당하는 mnemonic을 출력한다. */
/* 리턴값 : -1 : 에러 0 : 정상 */

int mnemonic(value* para1){

	char *mnemonic;
	char opcode[10];

	Linkedlist list;

	if(para1->exist == 0){
		printf("Need more parameters ! \n");
		return -1;
	}

	strcpy(opcode,(para1->value));
	list = mnemonic_table[OpToMne(opcode)];

	if(LFirst(&list,&mnemonic)){
		if(strcmp(findValue(&list),opcode) == 0){
			printf("mnemonic is %s\n",mnemonic);
			return 0;
		}
		while(LNext(&list,&mnemonic)){
			if(strcmp(findValue(&list),opcode) == 0){
				printf("mnemonic is %s\n",mnemonic);
				return 0;
			}
		}
	}
	return -1;
}

void opcodeList(void){
	int i;
	Data data;
	for(i = 0;i<20;i++){
		printf("%d : ",i);

		if(LFirst(&opcode_table[i],&data)){
			printf("[%s,%s]",findValue(&opcode_table[i]),data);

			while(LNext(&opcode_table[i],&data)){
				printf(" -> [%s,%s]",findValue(&opcode_table[i]),data);
			}
		}
		printf("\n");
	}
}

void mnemonicList(void){
	int i;
	Data data;
	for(i = 0;i<20;i++){
		printf("%d : ",i);

		if(LFirst(&mnemonic_table[i],&data)){
			printf("[%s,%s]",findValue(&mnemonic_table[i]),data);

			while(LNext(&mnemonic_table[i],&data)){
				printf(" -> [%s,%s]",findValue(&mnemonic_table[i]),data);
			}
		}
		printf("\n");
	}
}

int OpToMne(char* op){

	int hash,i;

	hash = 7;

	for(i=0;i<strlen(op);i++)
		hash = hash*17+op[i];
	return hash%MAX_HASH_SIZE;
}


int MneToOp(char* str){

	int hash,i;

	hash = 7;

	for(i=0;i<strlen(str);i++)
		hash = hash*29+str[i];
	return hash%MAX_HASH_SIZE;
}

int StringToHex(char* string){

	int len,i,hex;

	len = strlen(string);
	for(i=0;i<len;i++){
		if((string[i]<48 || string[i]>57) && (string[i]<60 || string[i]>70) && (string[i]<97 || string[i] > 102)){
			printf("Please enter the hexa number ! \n");
			return -1;
		}
	}	

	sscanf(string,"%X",&hex);
	return hex;
}


int fill(value* para1, value* para2, value* para3){

	int i,start,end,constant;

	if(para1->exist == 0 || para2->exist == 0 || para3 ->exist == 0){ // parameter가 하나라도 없을 시 에러
		printf("Need more parameters ! \n");
		return -1;
	}

	/* String을 Hex로 바꿔줌 */

	if((start = StringToHex(para1->value)) == -1 )
		return -1;
	if((end = StringToHex(para2->value)) == -1)
		return -1;
	if((constant = StringToHex(para3->value)) == -1)
		return -1;

	/* 메모리 넘어갈 시 체크 */
	if(start > 0xfffff || end > 0xfffff){
		printf("Memory Access Error\n");
		return -1;
	}


	for(i=start;i<=end;i++){
		memory[i%16][i/16] = constant;
	}

	return 0;
}

void reset(void){
	
	int i,j;

	for(i=0;i<16;i++){
		for(j=0;j<66536;j++){
			memory[i][j] = 0;
		}
	}
}


int edit(value* para1, value* para2){

	int address,constant;
	if(para1->exist == 0 || para2->exist == 0){
		printf("Need more parameters ! \n");
		return -1;
	}

	/* String을 Hex로 바꿔줌 */

	if((address = StringToHex(para1->value)) == -1)
		return -1;
	if((constant = StringToHex(para2->value)) == -1)
		return -1;
	/* 메모리 넘어갈 시 체크 */
	if(address > 0xfffff){
		printf("Memory Access Error\n");
		return -1;
	}

	memory[address%16][address/16] = constant;

	return 0;
}


int dump(value* para1, value* para2){

	int start,end,floor_start,floor_end,i,j;

	/* String을 Hex로 바꿔줌 */
	if(para1->exist == 1 && (start = StringToHex(para1->value)) == -1)
		return -1;
	if(para2->exist == 1 && (end = StringToHex(para2->value)) == -1)
		return -1;

	if(para1->exist == 0 && para2->exist == 1){
		printf("Need more parameters ! \n");
		return -1;
	}
	/* start가 안들어왔을 시 */
	if(para1->exist == 0){ 
		start = dumpCnt;
	}
	else
		dumpCnt = 0;
	if(para2->exist == 0)
		end = 0x9f+start;
	/* 메모리 넘어갈 시 체크 */
	if(start > 0xfffff || end > 0xfffff){
		printf("Memory Access Error\n");
		return -1;
	}
	if(start > end){
		printf("Start address is more grater than End address\n");
		return -1;
	}

	floor_start = start/16;
	floor_end = end/16;
	for(i=floor_start ; i<=floor_end ; i++){
		printf("%05X ",i*16);
		for(j=0;j<16;j++){
			if(start>(16*i+j) || end<(16*i+j))
				printf("   ");
			else
				printf("%02X ",memory[j][i]);
		}
		printf("; ");
		for(j=0;j<16;j++){
			if(memory[j][i]<0x20 || memory[j][i]>0x7e)
				printf(".");
			else
				printf("%c",memory[j][i]);
		}
		printf("\n");
	}
	dumpCnt = end+1;
	return 0;
}


void help(void){
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\n");
	printf("e[dit] address, value\nf[ill] start, end, value\nreset\n");
	printf("opcode mnemonic\nmnemonic opcode\nopcodelist\nmnemoniclist\n");
	printf("assemble filename\ntype filename\nsymbol\ndisassemble filename\n");
}


/* 함수 : dir(void) */
/* 목적 : 현재 디렉터리에 있는 파일들을 출력한다.  */
/* 리턴값 : 없음 */


void dir(void){
	DIR *dp;
	struct dirent *dir;
	struct stat buf;
	int linefeedcnt=0;

	if((dp= opendir(".")) == NULL){
		printf("directory open error\n");
		exit(-1);
	}

	while((dir=readdir(dp)) != NULL){
		if(dir->d_ino == 0)
			continue;
		lstat(dir->d_name,&buf);

		if(S_ISDIR(buf.st_mode)) // 폴더이면
			printf("%16s/",dir->d_name);
		else if(buf.st_mode & S_IXUSR) // 실행파일이면
			printf("%16s*",dir->d_name);
		else
			printf("%16s",dir->d_name);
		linefeedcnt++;
		if(linefeedcnt == 3){ //line 마다 3개씩만 출력
			printf("\n");
			linefeedcnt =0;
		}
	}
	if(linefeedcnt != 3)
		printf("\n");
	closedir(dp);
}

void history(char *order, Linkedlist *list){

	char *data;
	int cnt=0;

	if(LFirst(list,&data)){
		printf("%16d %s\n",++cnt,data);
		while(LNext(list,&data)){
			printf("%16d %s\n",++cnt,data);
		}
	}
	printf("%16d %s\n",++cnt,order);
}


void makeTable(void){

	FILE *fp;
	char line[30],*token;
	int i;
	char *mnemonic,*opcode,*format;

	for(i=0;i<20;i++){
		listinit(&opcode_table[i]);
		listinit(&mnemonic_table[i]);
		listinit(&symbol_table[i]);
		listinit(&literal_table[i]);
	}
	if((fp = fopen("opcode.txt","rt")) == NULL ){ 
		printf("file open error\n");
		exit(-1);
	}

	while(1){
		if(fgets(line,30,fp)==NULL)
			break;

		token = strtok(line," \t");
		if(token != NULL){
			opcode = (char*)malloc(sizeof(token)+1);
			strcpy(opcode,token);
			token = strtok(NULL," \t");
			mnemonic = (char*)malloc(sizeof(token)+1);
			strcpy(mnemonic,token);
			token = strtok(NULL," \t");
			format = (char*)malloc(sizeof(token)+1);
			strcpy(format,token);
			listinsert(&opcode_table[MneToOp(mnemonic)],opcode);
			listinsertmore(&opcode_table[MneToOp(mnemonic)],mnemonic);
			listinsert(&mnemonic_table[OpToMne(opcode)],mnemonic);
			listinsertmore(&mnemonic_table[OpToMne(opcode)],opcode);
			listinsertformat(&opcode_table[MneToOp(mnemonic)],format);
		}
	}
	free(opcode);
	free(mnemonic);
	fclose(fp);
}

