/*----------------------------------------------------------------------Begin of file: CTRMDES.c-----------------------------------------------------------------------*/
/*---------------------------------------------------------------------------Group #: 8-----------------------------------------------------------------------------*/
/*--------------------------------------------------------------@Author: Kapil Senthilkumar: 	500456514-----------------------------------------------------------*/
/*--------------------------------------------------------------@Author: Eric Sacco:		500447007-----------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define MAX 500


unsigned i_to_b(unsigned i);
void pRoundKeys(char *input, char output[][7]); //Produce round key for current iteration
void encryptAlg(char *pText, char rKeys[][7], char *cText); //Encrypts the plaintext using all round Keys and produces the cipher text
void decryptAlg(char *cText, char rKeys[][7], char *pText); //Decrypts the ciphertext using reverse of all round Keys 
void f(char *rPT, char *rKey, char *fRslt); //Round Function - Returns the XOR of plaintext and Round Key (6 bits)
void E(char *rPT, char *eRPT); //Expansion Function - Expands the 4 bit plaintext to 6 bits

int main(){
	
	char e_dOpt[50];
	int input = 0;

	printf("Please choose type of seed value k(keyboard), t(time), m(mouse): ");	//BONUS!!!		- Part 2 of Assignment
	while((fscanf(stdin,"%s", e_dOpt)) != EOF){
		if (strcasecmp(e_dOpt,"k") == 0 || strcasecmp(e_dOpt,"K") == 0 || 
			strcasecmp(e_dOpt,"keyBoard") == 0 || strcasecmp(e_dOpt,"KeyBoard") == 0){
			input = 1;	
			break;	
		}
		else if (strcasecmp(e_dOpt,"t") == 0 || strcasecmp(e_dOpt,"T") == 0 || 
			strcasecmp(e_dOpt,"time") == 0 || strcasecmp(e_dOpt,"Time") == 0){
			input = 0;
			break;		
		}
		else if (strcasecmp(e_dOpt,"m") == 0 || strcasecmp(e_dOpt,"M") == 0 || 
			strcasecmp(e_dOpt,"mouse") == 0 || strcasecmp(e_dOpt,"Mouse") == 0){
			input = 2;
			break;		
		}
		else
			printf("PLEASE ENTER k(keyboard) or t(time) or m(mouse) to contine: ");
	}

	
	char roundKeys[16][7]; //16 round keys - max 9 chars in length
	long int inputKey, rsltKey; 
	int index = 0, padLen = 0; //key inputed by user - index i & j
	char sKey[9], pKey[9]; //original binary representation of key
	printf("Please enter a key within the range of 0-255: ");
	scanf("%ld", &inputKey);
	
	
	rsltKey = i_to_b(inputKey); //convert int to binary
	sprintf(sKey, "%ld", rsltKey); //convert binary to string representation
		

	//Create Padding if the length is less then 8 bits
	int numOfPads = 8 - strlen(sKey);
	for ( index = 0; index < numOfPads; index++)
			pKey[index] = '0';
	strcpy(&pKey[numOfPads],sKey);	
	pKey[8] = '\0';
	
	pRoundKeys(&pKey[0], roundKeys);
	
	//Debugging Code - Test to see all 16 round keys
	/* 
	for (index = 0; index < 16; index++)
		printf("%s\n", roundKeys[index]);
	*/

	
	int index2 = 0;
	char pText[MAX], cText[MAX]; //The plaintext and cipertext
	char cpText[MAX],ccText[MAX];
	
	int numRand = 0; 
	int len = 0; 
	int ctrNum = 0, padding = 0, pos = 0;
	char ctrP[9], ctrC[9];	
	int pads = 0;
	char CTR[] = "111111111";
	
	if (input){
		/*-------------------Enciphering Algorithmn-------------------*/
		printf("Please enter a Seed Value(Exactly 1 Block: 8 chars): ");
		while ((scanf("%s", pText)) != EOF)
			if ((strlen(pText)) == 8)
				break;
			else
				printf("Please enter a Seed Value that is approximately 8 chars long: ");
	
	
		/*-------------------Mode of Operation: CTR-------------------*/
		if (pText[strlen(pText)-1] == '\n')
			pText[strlen(pText)-1] = '\0';
		len = strlen(pText);	//Length of String
	
	}
	else if (!input){
		time_t sec;
		time (&sec);
		
		struct tm beg;
		beg = *localtime(&sec);

		beg.tm_hour = 0;
    		beg.tm_min = 0;
    		beg.tm_sec = 0;
    		beg.tm_mday = 1;
		int seconds = difftime(sec, mktime(&beg));
		
		
		sprintf(pText,"%d", seconds);
		
		printf("%s\n", pText);
		if (pText[strlen(pText)-1] == '\n')
			pText[strlen(pText)-1] = '\0';
		len = strlen(pText);	//Length of Strin
		
		if (((int)len % 8) == 0)
			padding = 0;
		else
			padding = 1;

	}


	if (padding){
		
		ctrNum = (len+7)/8;
		padding = ctrNum * 8;
		for ( index = len; index < padding; index++)
			pText[index] = '0';

	}


	printf("Please enter number of pseudorandom blocks to generate: ");
	scanf("%d", &numRand);

	//Encrypt All the Counters needed for encryption	
	for ( index = 0; index < numRand; index++){
		printf("%s\n", pText);
		encryptAlg(pText, roundKeys, &cText[pos]);
		printf("%s\n", pText);
		pos+=8;
		cText[pos] = '\0';
		

		printf("Iteration: %d pseudorandom blocks: %s\n", index, &cText[pos-8]);

		for (index2 = 0; index2 < 8; index2++)
			pText[index2] = pText[index2]  ^ (CTR[index2] % 2);
		pText[8] = '\0';
		printf("%s\n",pText);
	}
		
	
	printf("|*------------------------------*|\n");
	printf(" ENCRYPTED PLAINTEXT: %s  \n", cText);
	printf("|*------------------------------*|\n");
	
	exit(0);
}

unsigned i_to_b(unsigned i) {
    if (i == 0) return 0;                    
    return (i % 2) + 10 * i_to_b(i / 2);
}

void pRoundKeys(char *input, char output[][7]){
	
	char cKey[9];					
	strcpy(&cKey[0],input);				//copy of original key

	char pc1Key[9];					//key after pc1
	char pc2Key[7];					//key after pc2

	int index = 0, rIndex = 0;	//round Index

	char l[5];					//left half
	char r[5];					//right half
	char sl[5];					//shifted left half
	char sr[5];					//shifted right half
	
	for ( rIndex = 0; rIndex < 16; rIndex++){
		
		for (index = 0; index < 7; index++)
			pc1Key[index+1] = cKey[index];
		pc1Key[0] = cKey[7];
		pc1Key[8] = '\0';
	
		//split key into 2 halfs	
		strncpy(l, pc1Key, 4);	
		strcpy(r, &pc1Key[4]);
		
		
		//Left Shift by 1 -- according to FIPS 46	
		if (rIndex == 0 || rIndex == 1 || rIndex == 8 || rIndex == 15){
			for (index = 0; index < 3; index++){
				sl[index] = l[index+1];
				sr[index] = r[index+1];
			}
			sl[3] = l[0];
			sr[3] = r[0];
			sl[4] = '\0';
			sr[4] = '\0';

			//re-create the key for next iteration 
			strcpy(cKey,sl);
			strcat(cKey,sr);

			//create new pc2Key
			strcpy(pc2Key,&sl[1]);
			strcat(pc2Key,&sr[1]);
			
		}	
			
		//Left Shift by 2 -- according to FIPS 46
		if ((rIndex >= 2 && rIndex <= 7) || (rIndex >= 9 && rIndex <= 14)){
			for (index = 0; index < 2; index++){
				sl[index] = l[index+2];
				sr[index] = r[index+2];
			}
			sl[2] = l[0];
			sr[2] = r[0];
			sl[3] = l[1];
			sr[3] = r[1];
			sl[4] = '\0';
			sr[4] = '\0';

			//re-create the key for next iteration 
			strcpy(cKey,sl);
			strcat(cKey,sr);

			//create new pc2Key
			strcpy(pc2Key,&sl[1]);
			strcat(pc2Key,&sr[1]);
		}	
		
		for ( index = 0; index < 7; index++) 
			output[rIndex][index] = pc2Key[5-index];
		output[rIndex][6] = '\0';
	}
}

void encryptAlg(char *pText, char rKeys[][7], char *cText){

	int rIndex = 0, index = 0;	//Round index, Loop index	
	char lPT[5];			//Left Plain Text
	char rPT[5];			//Right Plain Text
	char temp[5];			//holds the Left Plain Text
	char fRslt[5];			//function Result
	char xFL[5];			//Result of XOR between the f function result and Left Plain Text

	strncpy(lPT, pText, 4);
	strcpy(rPT, &pText[4]);
	lPT[4] = '\0';
	rPT[4] = '\0';	
	

	for (rIndex = 0; rIndex < 16; rIndex++){

		strcpy(temp,lPT);
		strcpy(lPT,rPT);	//L = R
		
		f(rPT,rKeys[rIndex],fRslt);
		for (index = 0; index < 5; index++)
			xFL[index] = (fRslt[index] % 2) ^ temp[index]; 	//Result of XOR between the f function result and Left Plain Text
		strcpy(rPT, xFL);	//R = L XOR f(K,R);

		printf("Round Key %d -> %s\n", rIndex, rKeys[rIndex]);
		printf("Iteration %d -> L%d: %s\n", rIndex, rIndex, lPT);
		printf("Iteration %d -> R%d: %s\n", rIndex, rIndex, rPT);
	}
	//Complete the ciphertext by combining: L16R16
	strcpy(cText, lPT);
	strcpy(&cText[4], rPT);
}

void decryptAlg(char *cText, char rKeys[][7], char *pText){

	
	int rIndex = 0, index = 0;	//Round index, Loop index	
	char lPT[5];			//Left Plain Text
	char rPT[5];			//Right Plain Text
	char temp[5];			//holds the Left Plain Text
	char fRslt[5];			//function Result
	char xFL[5];			//Result of XOR between the f function result and Left Plain Text

	strncpy(lPT, cText, 4);
	strcpy(rPT, &cText[4]);
	lPT[4] = '\0';
	rPT[4] = '\0';	
	

	for (rIndex = 15; rIndex >= 0; rIndex--){

		strcpy(temp,rPT);
		strcpy(rPT,lPT);	//R = L
		
		f(rPT,rKeys[rIndex],fRslt);
		for (index = 0; index < 5; index++)
			xFL[index] = (fRslt[index] % 2) ^ temp[index]; 	//Result of XOR between the f function result and Left Plain Text
		strcpy(lPT, xFL);	//L = R XOR f(K,L);

		printf("Round Key %d -> %s\n", rIndex, rKeys[rIndex]);
		printf("Iteration %d -> L%d: %s\n", rIndex, rIndex, lPT);
		printf("Iteration %d -> R%d: %s\n", rIndex, rIndex, rPT);
	}
	//Complete the plaintext by combining: L16R16
	strcpy(pText, lPT);
	strcpy(&pText[4], rPT);


}


void f(char *rPT, char *rKey, char *fRslt){

	int index = 0;			//Loop Index
	long int row = 0, column = 0, i_sbVal = 0; //
	char eRPT[7], xText[7], psbVal[5];

	E(rPT,eRPT);			//Expansion function for plaintext
	int sbVal = 0;			//S-Box Result

	char b_sbVal[5];		//Value of S-box in binary
	int S_BOX[4][16] =	{ 	14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
					0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
					4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
					15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13
				};
	
	//Convert the Plaintext character into a bit by using mod 2 -> then XOR with the round Key
	for( index = 0; index < 6; index++)
		xText[index] = (eRPT[index] % 2) ^ rKey[index]; 
	xText[6] = '\0';

	//printf("%s\n", xText); //Debugging Code -> Test the row and column values
	row = (((int)xText[0]-48)*pow(2,1)) + (((int)xText[5]-48)*pow(2,0)); //manually convert first and last bit into decimal
	column = (((int)xText[1]-48)*pow(2,3)) + (((int)xText[2]-48)*pow(2,2)) + (((int)xText[3]-48)*pow(2,1))+ (((int)xText[4]-48)*pow(2,0));
	//printf("%ld\n", column);//Debugging Code -> Test the row and column values

	sbVal = S_BOX[row][column];
	i_sbVal = i_to_b(sbVal);
	sprintf(b_sbVal, "%ld", i_sbVal);
	

	//Possible refactorization -> for simplicity
	//create padding if less then 4 bits
	int numOfPads = 4 - strlen(b_sbVal);
	for ( index = 0; index < numOfPads; index++)
			psbVal[index] = '0';
	strcpy(&psbVal[numOfPads],b_sbVal);	
	psbVal[4] = '\0';
	
	//Final permutation shift to the left by 1
	for (index = 0; index < 4; index++)
		fRslt[index+1] = psbVal[index];
	fRslt[0] = psbVal[3];
	fRslt[4] = '\0';
	
}

void E(char *rPT, char *eRPT){
	eRPT[0] = rPT[3];
	eRPT[1] = rPT[0];
	eRPT[2] = rPT[1];
	eRPT[3] = rPT[2];
	eRPT[4] = rPT[3];
	eRPT[5] = rPT[1];
	eRPT[6] = '\0';
}
/*----------------------------------------------------------------------End of file: CTRMDES.c-----------------------------------------------------------------------*/
