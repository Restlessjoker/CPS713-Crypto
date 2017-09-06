/*----------------------------------------------------------------------Begin of file: CBCMDES.c-----------------------------------------------------------------------*/
/*---------------------------------------------------------------------------Group #: 8-----------------------------------------------------------------------------*/
/*--------------------------------------------------------------@Author: Kapil Senthilkumar: 	500456514-----------------------------------------------------------*/
/*--------------------------------------------------------------@Author: Eric Sacco:		500447007-----------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX 500

//used for fixed IV
#define CTR 11110000

//used for nonce IV
#define index1 11110010
#define index2 11111000 
#define index3 11110001
#define index4 11110011
#define index5 11111111
#define index6 11010000
#define index7 11010010 
#define index8 10110011
#define index9 11110100
#define index10 10010010

unsigned i_to_b(unsigned i);
void pRoundKeys(char *input, char output[][7]); //Produce round key for current iteration
void encryptAlg(char *pText, char rKeys[][7], char *cText); //Encrpts the plaintext using all round Keys and produces the cipher text
void decryptAlg(char *cText, char rKeys[][7], char *pText); //Decrpts the ciphertext using reverse of all round Keys 
void f(char *rPT, char *rKey, char *fRslt); //Round Function - Returns the XOR of plaintext and Round Key (6 bits)
void E(char *rPT, char *eRPT); //Expansion Function - Expands the 4 bit plaintext to 6 bits

int main(){
	char e_dOpt[50];
	int encrypt = 0;

	//Obtain user input for encryption or decryption
	printf("Please enter e(encrypt) or d(decrypt): ");
	while((fscanf(stdin,"%s", e_dOpt)) != EOF){
		if (strcasecmp(e_dOpt,"e") == 0 || strcasecmp(e_dOpt,"E") == 0 || 
			strcasecmp(e_dOpt,"encrypt") == 0 || strcasecmp(e_dOpt,"Encrypt") == 0){
			encrypt = 1;	
			break;	
		}
		else if (strcasecmp(e_dOpt,"d") == 0 || strcasecmp(e_dOpt,"D") == 0 || 
			strcasecmp(e_dOpt,"decrypt") == 0 || strcasecmp(e_dOpt,"Decrypt") == 0){
			encrypt = 0;
			break;		
		}
		else
			printf("PLEASE ENTER e(encrypt) or d(decrypt) to contine\n");
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
	
	char pText[MAX], cText[MAX]; //The plaintext and cipertext
	char cpText[MAX],ccText[MAX];
	char ccpText[MAX];

	int len = 0;
	int ivIndex = 0;
	int blockCtr = 0, padding = 0, pos = 0;
	char ctrP[9], ctrC[9];
	char iv[9];
	char temp1[9];
	char temp[9];
	int pads = 0;
	int numOfPad;
	char padded[8];
	char ivSetting[50];
	int ivS = 0;
	int nonceIndex;
	
	//set iv
	//Choose type of iv setting for cbc encryption
	printf("PLEASE ENTER f(fixed) or n(nonce) to continue with your choice of iv: ");
	while((fscanf(stdin,"%s", ivSetting)) != EOF){
		if (strcasecmp(ivSetting,"f") == 0 || strcasecmp(ivSetting,"F") == 0 || 
			strcasecmp(ivSetting,"fixed") == 0 || strcasecmp(ivSetting,"Fixed") == 0){
			sprintf(iv, "%d", CTR);	
			break;	
		}
		else if (strcasecmp(ivSetting,"n") == 0 || strcasecmp(ivSetting,"N") == 0 || 
			strcasecmp(ivSetting,"nonce") == 0 || strcasecmp(ivSetting,"Nonce") == 0){
			printf("Pick a number from 0-9 to indicate placement in nonce list:	");
			scanf("%d", &nonceIndex);
			if(nonceIndex > 9) nonceIndex = 9;
			
			ivS = 1;
			
			//Grabs an iv from a defined list of nonceIvs
			if(nonceIndex == 0)
				sprintf(iv, "%d", index1);
			else if(nonceIndex == 1)
				sprintf(iv, "%d", index2);
			else if(nonceIndex == 2)
				sprintf(iv, "%d", index3);
			else if(nonceIndex == 2)
				sprintf(iv, "%d", index4);
			else if(nonceIndex == 4)
				sprintf(iv, "%d", index5);
			else if(nonceIndex == 5)
				sprintf(iv, "%d", index6);
			else if(nonceIndex == 6)
				sprintf(iv, "%d", index7);
			else if(nonceIndex == 7)
				sprintf(iv, "%d", index8);
			else if(nonceIndex == 8)
				sprintf(iv, "%d", index9);
			else
				sprintf(iv, "%d", index10);
			break;		
		}
		else
			printf("\nPLEASE ENTER f(fixed) or n(nonce) to continue with your choice of iv: ");
	}
	
	if (encrypt){
		/*-------------------Enciphering Algorithmn-------------------*/
		printf("\nPlease enter plaintext (MIN: 8 chars - MAX: 500 chars): ");
		scanf("\n");

		fgets(pText, MAX, stdin);
			
		/*-------------------Mode of Operation: CBC-------------------*/
		if (pText[strlen(pText)-1] == '\n')
			pText[strlen(pText)-1] = '\0';

		len = strlen(pText);	//Length of String
		 blockCtr = (len+7)/8; 	//Number of blocks to encrypt 
		
		numOfPad =(int) len % 8;
		
		//Determine whether we need padding - if string length is not a multiple of 8
		if (((int)len % 8) == 0)
			padding = 0;
		else
			padding = 1;
		
		//Encrypt all blocks
		for ( index = 0; index < blockCtr; index++){
			
			//Pad block if needed
			if(index == blockCtr-1){
				if(numOfPad != 0){
					for(ivIndex = numOfPad; ivIndex < 8; ivIndex++){
						pText[pos+ivIndex] = '0';
					}
					pText[pos+ivIndex] = '\0';
				}
			}
			
			//XOR the result from encrypting the IV with the plaintext
			for (ivIndex = 0; ivIndex < 8; ivIndex++){
				cText[ivIndex] = (iv[ivIndex]%2) ^ pText[pos+ivIndex];
			}
			
			//Pad with last x characters of former block
			if(index == blockCtr-1 && len > 8){
				if(numOfPad != 0){
					for(ivIndex = numOfPad; ivIndex < 8; ivIndex++){
						cText[ivIndex] = padded[ivIndex-numOfPad];
					}
					pText[pos+ivIndex] = '\0';
				}
			}
			cText[8] = '\0';
			
			encryptAlg(cText, roundKeys, &cpText[pos]);
			
			for(ivIndex=0;ivIndex<8;ivIndex++){
				iv[ivIndex] = cpText[pos+ivIndex];
			}
			
			//Swap last block with secondLastBlock
			if(index == blockCtr-1 && len > 8){
				if(numOfPad != 0){
					for( ivIndex = 0; ivIndex < numOfPad; ivIndex++){
						cpText[pos+ivIndex] = cpText[(pos-8) + ivIndex];
					}
					for(ivIndex = 0; ivIndex < 8; ivIndex++){
						cpText[(pos-8) + ivIndex] = iv[ivIndex];
					}
				}
			}
			
			//Create pad if needed, occurs only for texts of size > 8
			if(index == blockCtr-2){
				if(numOfPad != 0){
					for(ivIndex = 0; ivIndex < 8-numOfPad; ivIndex++){
						padded[ivIndex] = iv[numOfPad+ivIndex];
					}
					padded[ivIndex] = '\0';
				}
			}
			
			pos+=8;
		}
		
		//Remove additional padding so they do not appear on screen
		if (padding){
			pads = pos - len;
			for ( index = 0; index < pads; index++)
				cpText[index+len] = '\0';
		}
		
		//Output only if used nonce iv generation
		if(ivS == 1)
			printf("The following ciphertext was created using\nthe nonce iv from index %d of the list\n\n", nonceIndex);
			
		printf("|*------------------------------*|\n");
		printf("| ENCRYPTED PLAINTEXT: %s  |\n", cpText);
		printf("|*------------------------------*|\n");
	}
	else{
		/*-------------------Deciphering Algorithmn-------------------*/
		printf("\nPlease enter ciphertext (MIN: 8 chars - MAX: 500 chars): ");
		scanf("\n");
		
		fgets(cText, MAX, stdin);
		
		if (cText[strlen(cText)-1] == '\n')
			cText[strlen(cText)-1] = '\0';
 
		len = strlen(cText);
		blockCtr = (len+7)/8;
		
		numOfPad =(int) len % 8;
		
		//Determine whether we need padding - if string length is not a multiple of 8
		if (((int)len % 8) == 0)
			padding = 0;
		else
			padding = 1;	

		//Encrypt All the Counters needed for decryption
		for ( index = 0; index < blockCtr; index++){
			for(ivIndex = 0; ivIndex < 8; ivIndex++){
				temp[ivIndex] = cText[pos+ivIndex];
			}
			temp[8] = '\0';
			
			//Swap Last x bits of Cn-1 to last xbits of Cn
			if(index == blockCtr-1 && len > 8){
				if(numOfPad != 0){
					for(ivIndex = numOfPad; ivIndex < 8; ivIndex++){
						temp[ivIndex] = pText[(pos-8) + ivIndex];
					}
					temp[8] = '\0';
				}
			}
			
			decryptAlg(temp, roundKeys, &pText[pos]);
			
			//Pad block if needed
			if(index == blockCtr-1 && len > 8){
				if(numOfPad != 0){
					for(ivIndex = 0; ivIndex < 8; ivIndex++){
						temp1[ivIndex] = pText[pos + ivIndex];
					}
					temp1[8] = '\0';
					for(ivIndex = 0; ivIndex < numOfPad; ivIndex++){
						pText[pos + ivIndex] = pText[(pos-8) + ivIndex];
					}
					while(ivIndex < 8){
						pText[pos + ivIndex] = 0;
						ivIndex++;
					}
					pText[pos + ivIndex] = '\0';
					
					for(ivIndex = 0; ivIndex < 8; ivIndex++){
						pText[(pos-8) + ivIndex] = temp1[ivIndex];
					}
				}
			}
			
			pos+=8;
		}

		//XOR the result from decrypting former code blocks to the ciphertext
		
		for(index = 0; index < 8; index++){
			ccText[index] = (iv[index]%2) ^ pText[index];
		}
	
		//swap last and second last block so that we successfully xor our blocks, if we used ciphertext-stealing	
		if(numOfPad != 0){
			for(index = 0; index < numOfPad; index++){
				temp1[index] = cText[(pos-16) + index];
			}
			temp1[numOfPad] = '\0';
			
			for(index = 0; index < numOfPad; index++){
				cText[(pos-16) + index] = cText[(pos-8) + index];
			}
			
			for(index = 0; index < numOfPad; index++){
				cText[(pos-8) + index] = temp1[index];	
			}
			
			for (index = 8; index < pos; index++){
				ccText[index] = (cText[index-8]%2) ^ pText[index];
			}
		}
		
		//Remove additional padding so they do not appear on screen
		if (padding){
			pads = pos - len;
			for ( index = 0; index < pads; index++)
				ccText[index+len] = '\0';
		}	
	
		printf("|*------------------------------*|\n");
		printf("| DECRYPTED CIPHERTEXT: %s |\n", ccText);
		printf("|*------------------------------*|\n");
	}

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
	
	printf("------pText: %s \tRight: %s\n", lPT, rPT);

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
/*----------------------------------------------------------------------End of file: mDES.c-----------------------------------------------------------------------*/
