ReadMe.txt
This folder contains 6 files:
1. ReadMe.txt
2. CTRMDES-PRNG.c 
3. CTRMDES-PRNG
4. compile.txt
5. CTRAES-PRNG.C
6. CTRAES-PRNG
7. report.txt
8. results.txt


CTRMDES-PRNG.c  -> contains the source code for the encryption and decryption of the CTRM-DES-PRNG block cipher.  

CTRMDES-PRNG -> the binary file, compiled from the source code: CTRMDES-PRNG.c

CTRAES-PRNG.c  -> contains the source code for the encryption and decryption of the CTR-AES-PRNG block cipher.  

CTRAES-PRNG -> the binary file, compiled from the source code: CTRAES-PRNG.c

compile.txt -> instructions to compile and run the source code: CTRMDES-PRNG.c and CTRAES-PRNG.c

report.txt -> Which statisical tests we used. Output is stored in results.txt

results.txt ->  results from several sample inputs presented to the algorithm and the respective outputs generated by the algorithm. This is for both encryption and decryption of several different plaintext and cipher texts.

IMPORTANT NOTE: We have done the BONUS part two of the assignment for both keyboard strokes and time. We have made a best effort to do mouse movements for a seed value, but were unable to track the movements over a terminal.

IMPORTANT NOTE: Our algorithm is efficient in that, it converts the inputed plaintext or cipher text to binary before it computes the XOR between each round key. Instead, of assuming the plaintext will always be in binary form or the user forgets to input a binary plaintext or cipher text, our algorithm will simply convert their string into binary using mod 2 for every character. 


