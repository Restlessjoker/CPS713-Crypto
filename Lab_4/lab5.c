#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>

#define MAX 500

#define CTR 1111111100000000LL

unsigned int sBox[4][4][4] = { { {1, 0, 0, 1}, {0, 1, 0, 0}, {1, 0, 1, 0}, {1, 0, 1, 1}}, { {1, 1, 0, 1}, {0, 0, 0, 1}, {1, 0, 0, 0}, {0, 1, 0, 1} }, 
				{ {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 1, 1} }, { {1, 1, 0, 0}, {1, 1, 1, 0}, {1, 1, 1, 1}, {0, 1, 1, 1} } };
unsigned int sBoxD[4][4][4] = { { {1,0,1,0}, {0,1,0,1}, {1,0,0,1}, {1,0,1,1} }, { {0,0,0,1}, {0,1,1,1}, {1,0,0,0}, {1,1,1,1} }, { {0,1,1,0}, {0,0,0,0}, {0,0,1,0}, {0,0,1,1} }, { {1,1,0,0}, {0,1,0,0}, {1,1,0,1}, {1,1,1,1} } }; 

unsigned int d_roundKeys[2][2][4];
unsigned int testInput [2][2][4]= { {{1, 0 ,0, 1}, {0, 0, 1, 0}}, {{0, 0, 0, 0}, {0, 0, 0, 0}} };
unsigned int testInput2 [2][2][4] = { { {0,1,0,1}, {0,1,0,1} }, { {1,1,0,0}, {0,1,0,0} } };

unsigned int keyTest [2][2][4] = { { {0,1,1,0}, {1,1,0,1} }, { {1,0,1,1}, {0,0,1,1} } };

char e_dOpt[50];

int decToBinary(unsigned int, unsigned int *, int);
void shiftRows(unsigned int [2][2][4], char);
void mixColumns(unsigned int [2][2][4], char);
void binaryMultiply(long int, long int);
int binaryAddition(int, int);
void xOR(unsigned int *, unsigned int *, unsigned int *);
void multi(unsigned int *, unsigned int *, unsigned int *);
void nibbleSub(unsigned int [2][2][4], char);
void aes(unsigned int [2][2][4], char);
void addRoundKey(unsigned int [2][2][4], unsigned int [2][2][4]);
char binToDec(unsigned int[8]);
void pRoundKeys(unsigned int output[][16]);
void gFunction(unsigned int *, unsigned int *);

int main(){
	int i=0;
	int j=0;
	int length;
	char in[400];
	char action;
	
	unsigned int test[4];
	unsigned int t1[4] = {1, 1, 1, 1};
	unsigned int t2[4] = {1, 1, 1, 1};	
	
	//Obtain user input for encryption or decryption
	printf("Please enter e(encrypt) or d(decrypt): ");
	while((fscanf(stdin,"%s", e_dOpt)) != EOF){
		if (strcasecmp(e_dOpt,"e") == 0 || strcasecmp(e_dOpt,"E") == 0 || 
			strcasecmp(e_dOpt,"encrypt") == 0 || strcasecmp(e_dOpt,"Encrypt") == 0){
			action = 'e';	
			break;	
		}
		else if (strcasecmp(e_dOpt,"d") == 0 || strcasecmp(e_dOpt,"D") == 0 || 
			strcasecmp(e_dOpt,"decrypt") == 0 || strcasecmp(e_dOpt,"Decrypt") == 0){
			action = 'd';
			break;		
		}
		else
			printf("PLEASE ENTER e(encrypt) or d(decrypt) to continue: ");
	}
	
	unsigned int roundKeys[2][16]; //3 keys - 16 bits long
	unsigned int inputKey, rsltKey[16]; 
	unsigned int pKey[17];
	int index = 0;
	
	printf("Please enter a key within the range of 0-65,535: ");
	scanf("%d", &inputKey);
	decToBinary(inputKey,&roundKeys[0][0],16);
	
	pRoundKeys(&roundKeys[0]);
	char pText[17];	
	unsigned int bText[16][2][4];
	int prePos = 0, curPos = 0 ;
	unsigned int temp[8];
	int index2 = 0;
	

	for(index = 0; index < 4; index++)
			d_roundKeys[0][0][index] = roundKeys[0][index];
		
		for(index = 0; index < 4; index++)
			d_roundKeys[0][1][index] = roundKeys[0][index+4];
		
		for(index = 0; index < 4; index++)
			d_roundKeys[1][0][index] = roundKeys[0][index+8];
	
		for(index = 0; index < 4; index++)
			d_roundKeys[1][1][index] = roundKeys[0][index+12];
		
		for(index = 0; index < 4; index++)
			d_roundKeys[2][0][index] = roundKeys[1][index];
		
		for(index = 0; index < 4; index++)
			d_roundKeys[2][1][index] = roundKeys[1][index+4];
		for(index = 0; index < 4; index++)
			d_roundKeys[3][0][index] = roundKeys[1][index+8];
		
		for(index = 0; index < 4; index++)
			d_roundKeys[3][1][index] = roundKeys[1][index+12];
		
		for(index = 0; index < 4; index++)
			d_roundKeys[4][0][index] = roundKeys[2][index];
	
		for(index = 0; index < 4; index++)
			d_roundKeys[4][1][index] = roundKeys[2][index+4];
		
		for(index = 0; index < 4; index++)
			d_roundKeys[5][0][index] = roundKeys[2][index+8];
		
		for(index = 0; index < 4; index++)
			d_roundKeys[5][1][index] = roundKeys[2][index+12];
		
			
		
	char plText[MAX], cText[MAX]; //The plaintext and cipertext
	char cpText[MAX],ccText[MAX];
	int ctrNum;
	int len = 0, maxLen;
	int padding;
	
	int pads = 0;
	
	printf("Please enter plaintext (MAX: 500 chars): ");
	scanf("\n");

	fgets(plText, MAX, stdin);
	
	if (plText[strlen(plText)-1] == '\n')
		plText[strlen(plText)-1] = '\0';
	
	len = strlen(plText);	//Length of String
	ctrNum = (len+15)/16; 	//Number of Counters to encrypt 
	maxLen = ctrNum*16;
	
	//pad to next block
	for(i=len; i < maxLen; i++){
		plText[i] = '0';
	}
	plText[i] = '\0';
	
	//Determine wheather we need padding - if string length is not a multiple of 8
	if (((int)len % 16) == 0)
		padding = 0;
	else
		padding = 1;

	unsigned int strIn[maxLen];
	
	for(i = 0; i < maxLen; i++){
		strIn[i] = (unsigned int)plText[i] - 48;
		
		printf("JUST ADDED: %u\n", strIn[i]);
	}
	
	
    printf("length: %d\n", len);
	//length=i;
	/*if(len%2 == 1){
		len++;
		strIn[len-1] = 1;
		strIn[len] = '\0';
	}*/
	
	printf(" strIn: ");
	for(i = 0; i < maxLen; i++){
		printf("%u", strIn[i]);
	}
	printf("\n");
	
	int t;
	unsigned int block [2][2][4];
    unsigned int input [maxLen/8][2][4];
	char ctrPl[17];
	unsigned int ctrP[16];
	//long unsigned int testCtr = 1111111100000000;
	int ctrTest = 1200;
	
	for(i = 0; i < ctrNum; i++){
		/*sprintf(ctrPl, "%lu", testCtr);
		printf("CTR IS: %s\n", ctrPl);
		
		for(j=0; j<16;j++){
			ctrP[j] = (unsigned int) ctrPl[j] - 48;
			printf("Converted: %u\n", ctrP[j]);
		}*/
		printf("LOOP #%d\n", i);
		decToBinary(ctrTest++, &ctrP[0], 16); 
		
		for(j=0; j<16;j++){
			printf("Converted: %u\n", ctrP[j]);
		}
		
		/*printf(" strIn: ");
			for(t = 0; t < maxLen; t++){
				printf("%u", strIn[t]);
			}
			printf("\n");*/
			
		for(j=0;j<2;j++){
			block[j][0][0] = ctrP[8*j];
			block[j][0][1] = ctrP[8*j + 1];
			block[j][0][2] = ctrP[8*j + 2];
			block[j][0][3] = ctrP[8*j + 3];
	
			block[j][1][0] = ctrP[8*j + 4];
			block[j][1][1] = ctrP[8*j + 5];
			block[j][1][2] = ctrP[8*j + 6];
			block[j][1][3] = ctrP[8*j + 7];

		}
		aes(block, 'e');

		for(j=0;j<2;j++){
						input[i+j][0][0] = block[j][0][0];
                        input[i+j][0][1] = block[j][0][1];
                        input[i+j][0][2] = block[j][0][2];
                        input[i+j][0][3] = block[j][0][3];

                        input[i+j][1][0] = block[j][1][0];
                        input[i+j][1][1] = block[j][1][1];
                        input[i+j][1][2] = block[j][1][2];
                        input[i+j][1][3] = block[j][1][3];
		}
				/*printf(" strIn: ");
			for(t = 0; t < maxLen; t++){
				printf("%u", strIn[t]);
			}
			printf("\n");*/
	}
			
	int k;
	unsigned int ciText[maxLen], cpiText[maxLen];
	int strCtr = 0;
	
	printf("Final Output is ");
	for(i=0;i<maxLen/8;i++){
		 for(k=0;k<2;k++){
         	       for(j=0;j<4;j++){
         	               ciText[strCtr] = input[i][k][j];
						   strCtr++;
         	       }
	    }
	}
	ciText[strCtr] = '\0';
		printf("strCtr == %d\n", strCtr);
		
		printf(" ENCRYPTED PLAINTEXT BEFORE: ");
		for(i = 0; i < maxLen; i++){
			printf("%u", ciText[i]);
		}
		printf("\n");
		
		for (i = 0; i < maxLen; i++)
			cpiText[i] = strIn[i]  ^ ciText[i];
		
			printf(" cpiText: ");
			for(i = 0; i < maxLen; i++){
				printf("%u", cpiText[i]);
			}
			printf("\n");
			
			printf(" strIn: ");
			for(i = 0; i < maxLen; i++){
				printf("%u", strIn[i]);
			}
			printf("\n");
			
			printf(" ciText: ");
			for(i = 0; i < maxLen; i++){
				printf("%u", ciText[i]);
			}
			printf("\n");
		//Remove additional padding so they do not appear on screen
		if (padding){
			pads = maxLen - len;
			for ( i = 0; i < pads; i++)
				cpiText[i+len] = '\0';
		}	
		
		if(action == 'e'){
			printf("|*------------------------------*|\n");
			printf(" ENCRYPTED PLAINTEXT: ");
			for(i = 0; i < len; i++){
				printf("%u", cpiText[i]);
			}
			printf("\n");
			printf("|*------------------------------*|\n");
		}
		else{
			printf("|*------------------------------*|\n");
			printf(" DECRYPTED CIPHERTEXT: ");
			for(i = 0; i < len; i++){
				printf("%u", cpiText[i]);
			}
			printf("\n");
			printf("|*------------------------------*|\n");
		}
		//printf("%c\n", out[i]);
}

void pRoundKeys(unsigned int output[][16]){
	int index = 0;
	unsigned int w2[8], w3[8], w4[8], w5[8];
	unsigned int gw1[8], gw3[8], rgw1[8], rgw3[8];
	unsigned int Rcon1[8] = {1,0,0,0,0,0,0,0};
	unsigned int Rcon2[8] = {0,0,1,1,0,0,0,0};
	
	//WORD 2
	gFunction(&output[0][8],&gw1[0]);
	for( index = 0; index < 8; index++)
		rgw1[index] = Rcon1[index]^gw1[index];
	for( index = 0; index < 8; index++)
		w2[index] = output[0][index]^rgw1[index];
	
	//WORD 3
	for( index = 0; index < 8; index++)
		w3[index] = output[0][index+8]^w2[index];
	
	//WORD 4
	gFunction(&w3[0],&gw3[0]);
	for( index = 0; index < 8; index++)
		rgw3[index] = Rcon2[index]^gw3[index];
	for( index = 0; index < 8; index++)
		w4[index] = w2[index]^rgw3[index];
	
	//WORD 5
	for( index = 0; index < 8; index++)
		w5[index] = w4[index]^w3[index];
	//Key Generation 
	for (index = 0; index < 8; index++){
		output[1][index] = w2[index];
	}
	for (index = 0; index < 8; index++)
		output[1][index+8] = w3[index];
	for (index = 0; index < 8; index++)
		output[2][index] = w4[index];
	
	for (index = 0; index < 8; index++)
		output[2][index+8] = w5[index];
		
	
}

void gFunction(unsigned int *input, unsigned int *output){
	int index = 0;
	unsigned int row = 0, column = 0;
	unsigned int sBox_val = 0;
	unsigned int temp1[4], temp2[4];
	unsigned int sTempL[4], sTempR[4];
	int count = 0;
	unsigned int sBox[5][4] = 	{	9,4,10,11,
					13,1,8,5,
					6,2,0,3,
					12,14,15,7
					
				};	
	
	for (index = 0; index < 4; index++)
			temp1[index] = input[index];
	for (index = 4; index < 8; index++)
			temp2[index-4] = input[index];
	
	//Left Half Substitution 
	row = (temp1[0] * pow(2,1)) + (temp1[1] * pow(2,0));
	column = (temp1[2] * pow(2,1)) + (temp1[3] * pow(2,0));
	sBox_val = sBox[row][column];
	count = decToBinary(sBox_val, &sTempL[0],4);
	
	//Right Half Substitution 	
	row = (temp2[0] * pow(2,1)) + (temp2[1] * pow(2,0));
	column = (temp2[2] * pow(2,1)) + (temp2[3] * pow(2,0));
	sBox_val = sBox[row][column];
	count = decToBinary(sBox_val, &sTempR[0],4);
	
	for (index = 0; index < 4; index++)
		output[index] = sTempL[index];
	for (index = 0; index < 4; index++)
		output[index+4] = sTempR[index];
	
}

char binToDec(unsigned int array[8]){
    int i= 1;
    int j=1;
    int indexCtr=0;
	int total = 0;
	int k = 7;
	int p = pow(2, k);
	//unsigned int arr[8];
	
	printf("SAMPLE OUTPUT: ");
	
	for(i = 0; i < 8; i++)
			printf("%u", array[i]);
	
	printf("\n");
	
    while(k>=0){
		total += array[7-k]*p;
		
		k--;
		p = pow(2, k);
	}
	
	printf("TOTAL! %d, %c \n", total, (char) total);
    return (char) total;
}


//Perform all of our aes operations in the required order per round
void aes(unsigned int in[2][2][4], char action){
	
	unsigned int currentRoundKey[2][2][4];
	
	int i;
	int j;

	//Encyption Order
	if(action == 'e'){
		//Initial Add Round Key
		
		for(i=0;i<2;i++){
			for(j=0;j<2;j++){
				currentRoundKey[i][j][0] = d_roundKeys[i][j][0];
				currentRoundKey[i][j][1] = d_roundKeys[i][j][1];
				currentRoundKey[i][j][2] = d_roundKeys[i][j][2];
				currentRoundKey[i][j][3] = d_roundKeys[i][j][3];		
			}			
		}

		addRoundKey(in, currentRoundKey);
    
	        for(i=0;i<2;i++){
    		        for(j=0;j<2;j++){
                                currentRoundKey[i][j][0] = d_roundKeys[i+2][j][0];
                                currentRoundKey[i][j][1] = d_roundKeys[i+2][j][1];
                                currentRoundKey[i][j][2] = d_roundKeys[i+2][j][2];
                                currentRoundKey[i][j][3] = d_roundKeys[i+2][j][3];
                        }
                }

		//Round 1
		nibbleSub(in, action);
		shiftRows(in, action);
		mixColumns(in, action);
		addRoundKey(in, currentRoundKey);
	
	        for(i=0;i<2;i++){
               	       for(j=0;j<2;j++){
                                currentRoundKey[i][j][0] = d_roundKeys[i+4][j][0];
                                currentRoundKey[i][j][1] = d_roundKeys[i+4][j][1];
                                currentRoundKey[i][j][2] = d_roundKeys[i+4][j][2];
                                currentRoundKey[i][j][3] = d_roundKeys[i+4][j][3];
                        }
                }

		//Round 2 
		nibbleSub(in, action);
		shiftRows(in, action);
		addRoundKey(in, currentRoundKey);
	}
	else{//Decryption Order

	       for(i=0;i<2;i++){
                        for(j=0;j<2;j++){
                                currentRoundKey[i][j][0] = d_roundKeys[i+4][j][0];
                                currentRoundKey[i][j][1] = d_roundKeys[i+4][j][1];
                                currentRoundKey[i][j][2] = d_roundKeys[i+4][j][2];
                                currentRoundKey[i][j][3] = d_roundKeys[i+4][j][3];
                        }
                }

		//Inital Add Round Key
                addRoundKey(in, currentRoundKey);

	         for(i=0;i<2;i++){
                        for(j=0;j<2;j++){
                                currentRoundKey[i][j][0] = d_roundKeys[i+2][j][0];
                                currentRoundKey[i][j][1] = d_roundKeys[i+2][j][1];
                                currentRoundKey[i][j][2] = d_roundKeys[i+2][j][2];
                                currentRoundKey[i][j][3] = d_roundKeys[i+2][j][3];
                        }
                }

                //Round 1
                shiftRows(in, action);
		nibbleSub(in, action);
		addRoundKey(in, currentRoundKey);
                mixColumns(in, action);

	       for(i=0;i<2;i++){
                        for(j=0;j<2;j++){
                                currentRoundKey[i][j][0] = d_roundKeys[i][j][0];
                                currentRoundKey[i][j][1] = d_roundKeys[i][j][1];
                                currentRoundKey[i][j][2] = d_roundKeys[i][j][2];
                                currentRoundKey[i][j][3] = d_roundKeys[i][j][3];
                        }
                }

                //Round 2
                shiftRows(in, action);
		nibbleSub(in, action);
                addRoundKey(in, currentRoundKey);
	}
	
}

//Perform an addRoundKey operation - XOR our round key to our plaintext block
void addRoundKey(unsigned int in[2][2][4], unsigned int key[2][2][4]){
	int i;
	int j;
	
	for(i=0;i<2;i++){
		for(j=0;j<2;j++){
			in[i][j][0] = in[i][j][0]^key[i][j][0];
			in[i][j][1] = in[i][j][1]^key[i][j][1];
			in[i][j][2] = in[i][j][2]^key[i][j][2];
			in[i][j][3] = in[i][j][3]^key[i][j][3];
		}
	}
	
}

//Perform a nibble substution operation
void nibbleSub(unsigned int in[2][2][4], char action){
	int i;
	int k;
	int j;
	
	printf("Passed In:\n");
	for(i = 0; i < 2; i++){
		for(j = 0; j < 2; j++){
			for(k = 0; k < 4; k++)
				printf("%u", in[i][j][k]);
		printf(" ");
		}
		printf("\n");
	}
	
	if(action == 'e'){ 
		for(k=0;k<2;k++){
			unsigned int row1, row2, col1, col2;

			for(i=0;i<2;i++){

				row1 = in[k][i][0];
				row2 = in[k][i][1];
				col1 = in[k][i][2];
				col2 = in[k][i][3]; 
				
				printf("Row1 %u: Row2 %u: Col1 %u: Col2 %d\n",row1, row2, col1, col2, k);	
				printf("Made it here on loop %d\n", k);	
				in[k][i][0] = sBox[(2*row1) + row2][(2*col1) + col2][0];
				in[k][i][1] = sBox[(2*row1) + row2][(2*col1) + col2][1];
				in[k][i][2] = sBox[(2*row1) + row2][(2*col1) + col2][2];
				in[k][i][3] = sBox[(2*row1) + row2][(2*col1) + col2][3];
			}
	
		}
	}
	else{
		for(k=0;k<2;k++){
                	unsigned int row1, row2, col1, col2;

                	for(i=0;i<2;i++){
                  	      row1 = in[k][i][0];
                  	      row2 = in[k][i][1];
                  	      col1 = in[k][i][2];
                  	      col2 = in[k][i][3];

                      	      in[k][i][0] = sBoxD[(2*row1) + row2][(2*col1) + col2][0];
                       	      in[k][i][1] = sBoxD[(2*row1) + row2][(2*col1) + col2][1];
                	      in[k][i][2] = sBoxD[(2*row1) + row2][(2*col1) + col2][2];
                	      in[k][i][3] = sBoxD[(2*row1) + row2][(2*col1) + col2][3];
                	}

        	}
	}

}

//Performs the shiftrow operation on our plaintext
void shiftRows(unsigned int in [2][2][4], char action){
	int i;
	int j;
 	int newIndex;
	unsigned int temp[2][4];

	//Shift rows to the left
	if(action == 'e'){
		for(i = 1; i < 2; i++){

			temp[0][0] = in[i][0][0];
			temp[0][1] = in[i][0][1];
			temp[0][2] = in[i][0][2];
			temp[0][3] = in[i][0][3];
		
			temp[1][0] = in[i][1][0];
			temp[1][1] = in[i][1][1];
			temp[1][2] = in[i][1][2];
			temp[1][3] = in[i][1][3];
			
			for(j = 0; j < 2; j++){
				if((j-i) < 0){ 
					newIndex = (j-i) + 2;	
				}
				else{
					newIndex = j-i;
				}
				
				in[i][j][0] = temp[newIndex][0];
				in[i][j][1] = temp[newIndex][1];
				in[i][j][2] = temp[newIndex][2];
				in[i][j][3] = temp[newIndex][3];	
			}
		}
	}
	else{//Shift rows to the right
		for(i = 1; i < 2; i++){

                	temp[0][0] = in[i][0][0];
	                temp[0][1] = in[i][0][1];
        	        temp[0][2] = in[i][0][2];
        	        temp[0][3] = in[i][0][3];
	
	                temp[1][0] = in[i][1][0];
	                temp[1][1] = in[i][1][1];
	                temp[1][2] = in[i][1][2];
        	        temp[1][3] = in[i][1][3];
                
               		 for(j = 0; j < 2; j++){
                       		if((j+i) > 1){
                                	newIndex = (j+i) - 2;
                        	}
                       		else{
                                	newIndex = j+i;
                        	}

                      	  	in[i][j][0] = temp[newIndex][0];
                        	in[i][j][1] = temp[newIndex][1];
                      		in[i][j][2] = temp[newIndex][2];
                       		in[i][j][3] = temp[newIndex][3];
                	}
        	}

	}

	
}

//Apply the mixColumns operation for the rounds
void mixColumns(unsigned int in [2][2][4], char action){
	
	
	unsigned int tBox[2][2][4];
	
	unsigned int newArray[4];

        int j;
        int i;
        unsigned int S00[4], S01[4], S10[4], S11[4];
        unsigned int mS00[4], mS01[4], mS10[4], mS11[4];
        unsigned int m2S00[4], m2S01[4], m2S10[4], m2S11[4];

	if(action == 'e'){
		tBox[0][0][0] = 0;
		tBox[0][0][1] = 0;
		tBox[0][0][2] = 0;
		tBox[0][0][3] = 1;
		
		tBox[0][1][0] = 0;
       		tBox[0][1][1] = 1;
        	tBox[0][1][2] = 0;
	        tBox[0][1][3] = 0;
	
		tBox[1][0][0] = 0;
      		tBox[1][0][1] = 1;
	        tBox[1][0][2] = 0;
       		tBox[1][0][3] = 0;

		tBox[1][1][0] = 0;
	        tBox[1][1][1] = 0;
	        tBox[1][1][2] = 0;
	        tBox[1][1][3] = 1;
	}
	else{
		tBox[0][0][0] = 1;
        	tBox[0][0][1] = 0;
        	tBox[0][0][2] = 0;
        	tBox[0][0][3] = 1;
	
	        tBox[0][1][0] = 0;
	        tBox[0][1][1] = 0;
        	tBox[0][1][2] = 1;
        	tBox[0][1][3] = 0;

        	tBox[1][0][0] = 0;
        	tBox[1][0][1] = 0;
        	tBox[1][0][2] = 1;
        	tBox[1][0][3] = 0;

        	tBox[1][1][0] = 1;
        	tBox[1][1][1] = 0;
        	tBox[1][1][2] = 0;
        	tBox[1][1][3] = 1;
	}
	
	printf("TestForVal:\n");
	for(j=0;j<4;j++){
		printf("%u", in[0][0][j]); 
	}
	printf("\n");
	
	//Perform operations on each nibble for matrix multiplication
	multi(tBox[0][1], in[1][0], mS00);
	multi(tBox[0][0], in[0][0], m2S00);
	xOR(mS00, m2S00, S00);

	multi(tBox[0][1], in[1][1], mS01);
	multi(tBox[0][0], in[0][1], m2S01);
	xOR(mS01, m2S01, S01);

	multi(tBox[1][1], in[1][0], mS10);
	multi(tBox[1][0], in[0][0], m2S10);
	xOR(mS10, m2S10, S10);

	multi(tBox[1][0], in[0][1], mS11);
	multi(tBox[1][1], in[1][1], m2S11);
	xOR(mS11, m2S11, S11);

	printf("TestMixedClumns:\n");
	for(j=0; j<4; j++){
		printf("%d", mS00[j]);
	}
	printf("\n");

	//Update our Plaintext
	
	in[0][0][0] = S00[0];
	in[0][0][1] = S00[1];
	in[0][0][2] = S00[2];
	in[0][0][3] = S00[3];
	
	in[0][1][0] = S01[0];
	in[0][1][1] = S01[1];
	in[0][1][2] = S01[2];
	in[0][1][3] = S01[3];
	
	in[1][0][0] = S10[0];
	in[1][0][1] = S10[1];
	in[1][0][2] = S10[2];
	in[1][0][3] = S10[3];
	
	in[1][1][0] = S11[0];
	in[1][1][1] = S11[1];
	in[1][1][2] = S11[2];
	in[1][1][3] = S11[3];
}	

//XORS the specified binary numbers
void xOR(unsigned int num[4], unsigned int num2[4], unsigned int newArray[4]){
	int i;
	for(i=0;i<4;i++){
		newArray[i] = num[i]^num2[i];
	}
}

//Performs binary multiplication over GF(2^4) finite field with 2 specified binary numbers
void multi(unsigned int num[4], unsigned int num2[4], unsigned int newArray[4]){
	unsigned int n1[4];
	n1[0] = num[0];
	n1[1] = num[1];
	n1[2] = num[2];
	n1[3] = num[3];
	
	unsigned int n2[4];
	n2[0] = num2[0];
	n2[1] = num2[1];
	n2[2] = num2[2];
	n2[3] = num2[3];
	
	unsigned int irreduc[5] = {1, 0, 0, 1, 1}; //Irreducible value for GF(2^4)
       
	int i;
	int j= 0;
	int newSize;	
	int indexCtr;
        unsigned int list[4][7];
	unsigned int multArray[7];

	//Beginning to find the answer for the multiplication portion binary multiplication over GF(2^4)
        int ctr=0;
        int l;
        int number = 6;

        for(i=3;i>=0;i--){
                for(l=0; l<ctr; l++){
                        list[ctr][6-l] = 0;
                   	printf("Added 0 at %d\n", 6-l);
                }
                for(j=3;j>=0;j--){
                        list[ctr][6-l] = n1[j]*n2[i];
                        printf("ADDED: %d----%d\n", list[ctr][6-l], l);
                        l++;
                }
                while(l<7){
                        printf("adding 0 to start at index %d\n", l);
                        list[ctr][6-l] = 0;
                        l++;
                }
                printf("Attempt1\n");
                for(j=0;j<7;j++){
                        printf("%d", list[ctr][j]);
                }

                printf("\n\n");
        	ctr++;
        }
		
	multArray[6] = list[0][6]^list[1][6]^list[2][6]^list[3][6];
	multArray[5] = list[0][5]^list[1][5]^list[2][5]^list[3][5];
	multArray[4] = list[0][4]^list[1][4]^list[2][4]^list[3][4];
	multArray[3] = list[0][3]^list[1][3]^list[2][3]^list[3][3];
        multArray[2] = list[0][2]^list[1][2]^list[2][2]^list[3][2];
        multArray[1] = list[0][1]^list[1][1]^list[2][1]^list[3][1];
        multArray[0] = list[0][0]^list[1][0]^list[2][0]^list[3][0];
	
	int first = 0;

	while(multArray[first] != 1 && first < 7){
		first++;	
	}
	
	
	int unsigned newBinary[7-first];
        newSize = 7-first;
	for(i=0;i<7-first;i++){
		newBinary[i] = multArray[first+i];
		printf("Added to newBinary %d\n", multArray[first+i]);
	}
	//End of multiplication portion of binary multiplication

	if(7-first >= 5){
		//Here we will reduce by our irreducible binary number
		unsigned int window[6];
		unsigned int remainder[5];
		unsigned int tempW[5];
		unsigned int final[4];
		
		int globalFirstOne = 0;
		int firstOne = -1;
		for(i=0; i<5; i++){
			window[i] = newBinary[i];
		}
		
		printf("First Window\n");
		for(i=0;i<5;i++){
			printf("%u", window[i]);
		}
		printf("\n");
		
		while(1){
			firstOne = -1;
			for(i=0;i<5;i++){
				if((window[i] == 1 && irreduc[i] == 1) || (window[i] == 0 && irreduc[i] == 0)){	
					remainder[i] = 0;
				}
				else{
					remainder[i] = 1;
					
					if(firstOne == -1){
						printf("Made it here\n");
						firstOne = i;
						globalFirstOne += firstOne;
					}
				}
			
			}
			
			printf("Remaining\n");
			for(i=0; i<5; i++){
				printf("%u", remainder[i]);
			}
			printf("\n");
			
			//Check to see if the remainder for our division is less than 5 characters. If no, then carry over enough of the unchecked bits to create a new window of length 5.
			//Divide this window by the irreducible
			if(newSize-globalFirstOne >= 5){
				for(i=0; i<5-firstOne; i++){
					window[i] = remainder[firstOne+i];
				}
				printf("Values: %d %d %d\n", globalFirstOne, i, firstOne);
				for(j=0;j<5-i;j++){
					window[i+j] = newBinary[globalFirstOne+(i+j)];
				}
				
				printf("New Window\n");
	
				for(i=0;i<5;i++){
					printf("%d", window[i]);
				}	
				printf("\n");
			}
			else{//If the remainder has a length of 4 or less, than pad 0s accordingly and return remainder of length 4 bits ex ( remainder: 11 = 0011)
				int size = newSize-globalFirstOne;
				
				
				for(i=0;i<4-size;i++){
					newArray[i]=0;
				}
				printf("TEST: %d  %d %d %d %d\n", firstOne, i, 4-size, size-i, globalFirstOne);
				for(j=0;j<5-firstOne;j++){
					newArray[i+j] = remainder[firstOne+j];
				}
				int cIndex = i+j;
				
				printf("index: %d\n", cIndex);
				int k = 0;
				for(i=cIndex;i<4;i++){
					newArray[i] = newBinary[globalFirstOne+(5-firstOne)+k];
					k++;
				}
				

				for(i=0;i<4;i++){
					printf("%u",newArray[i]);
				}
				printf("\n");

				return;
			}
		}
	}
	else{//We do not have to divide by our irreducible
		printf("Assignment:\n"); 
		
		for(i=0; i<4-newSize;i++){
			newArray[i] = 0;
		}
		for(j=0;j<4-i;j++){
			newArray[i+j] = newBinary[j];
			printf("%u", newArray[i+j]); 
		}
		printf("\n");
		return;
	}
}

int decToBinary(unsigned int num, unsigned int *array, int len){
 	int i= 1;
 	int j=1;
	int t;
	printf("Sent in number %u\n", num);
 	int remaining=num;
 	int indexCtr=0;
	long int temp[len];
	int index = 0;
	
 	while(i<=num)
 		 i=pow(2, j++);
 	j--;
 	i=pow(2, --j);
 	
 	while(i>0){
  		if(i <= remaining){
   			temp[indexCtr] = 1;
   			remaining = remaining - i;
			
  		}
  		else
   			temp[indexCtr] = 0;
  		indexCtr++;
  		i=pow(2, --j);
		
		printf("Damnit Kapil: %d, Power: %d \n", i, j);
	}
	j = 1;
	for (index = len-1; index >= len - indexCtr; index--){
		array[index] = temp[indexCtr-j];
		j++;
	}	
	while(j<=len){
		array[len-j] = 0;
		j++;
	}
	
	printf("FINAL: ");
	for(t = 0; t < 8; t++)
			printf("%u", array[t]);
	printf("\n");
	
	return indexCtr;
}
