/*
    ////////////////////////////////////////////////////////
    Computer System Architecture Team Project

    Team Name : 5
    Assignment : Assembler
    Project period : 2021-10-06 ~ 2021-11-15
    ////////////////////////////////////////////////////////

    [Work history]

    <2021-10-24> - 최우진
     1. 기본 기능 구현 (First, Second Pass)
     2. (표 6-5) 프로그램 테스트 및 성공 (input.txt)
     3. (표 6-8) 프로그램 테스트 및 오류 발생. lable 글자 수 인식 필요
     4. output.bin에 원하는 값이 안 나옴. 이진 수를 문자열로서 txt 파일로 출력할 필요가 있음

    <2021-10-31> - 최우진
     1. lable 글자 수를 인식하기 위해 is_lable 함수와 insert_lable 함수를 수정
     2. 16진수를 2진수로 변환 하기 위한 ConvertHexToBin 함수를 추가
     3. 이진 수를 문자열 형태의 output.txt 파일로 출력하기 위한 output_bin 함수를 추가
     4. fwrite 함수를 output_bin 함수로 대체
     5. input, output 파일을 asm, bin이 아니라 txt 파일로 수정
     6. (표 6-8) 프로그램 테스트 및 성공 (input2.txt)
     7. interface 출력 형태에 대해 논의가 필요
    
    <2021-11-02> - 최우진
     1. input2.txt 파일 입력시 output 파일 값의 순서가 섞여 출력됨. 수정 필요
     2. hex 출력 파일 필요
     3. 인터럽트 어떻게 하지?
     4. output 파일 값의 순서가 섞여 출력 되는 문제 해결 (fclose 함수 위치를 while문 마지막 줄로 변경)
     5. 실행시 output_bin.txt, output_hex.txt 와 같이 2진, 16진 파일 둘다 출력 되도록 수정
     6. (표 6-13) 프로그램 테스트시 lable 없는 DEC, HEX에서 오류 발생
        ㄴ2nd pass의 if (IS_PSEUDO(opcode)) 안에 lable 없는 DEC, HEX를 체크하는 조건문을 추가하여 해결
     7. (표 6-13) 프로그램 테스트시 lable 있는 간접모드 명령어에서 'I'를 인식 못 하는 문제 발생
        ㄴ2nd pass의 MRI 조건문 안에 lable이 있는 간접모드 명령을 체크하는 조건문을 추가하여 해결

    <2021-11-03> - 최우진
     1. 4자리 보다 적거나 음수가 4자리 이상 혹은 이하로 출력 되는 문제를 서식지정자 수정으로 해결, 빈 자리는 0으로 채운다.
     2. token[3]에 바로 접근하면 오류 발생. (빈 token 접근 할 때)
        ㄴnum_token으로 토큰 개수 먼저 체크하고 num_token이 4일 때만 token[3]에 접근 가능하도록 수정하여 해결

    <2021-11-04> - 최우진
     1. input 어셈블러 프로그램에 lable이 있지만 뒤에 아무것도 없을 경우 프로그램이 정상 실행이 안 되는 문제 발생 (변수의 초기화가 안 되었기 때문)
        ㄴlable이 있지만 뒤에 아무것도 없을 경우 2nd pass에서 lable이 "CTR,"일 때만 임의로 token[1]="DEC", token[2]="0" 하고 나머지 lable일 경우는 token[1]="HEX", token[2]="0"을 하도록 수정하여 해결

    <2021-11-07> - 최우진
     1. lable만 있을때 lable이 뭐든간에 token[1]="HEX", token[2]="0"을 하도록 수정
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable: 4996)

#define BOOL 	int
#define TRUE	1
#define FALSE	0

#define MAX_LABLE_LEN	10
#define MAX_TABLE_SZ    100
#define MAX_LINE_LEN	100

#define IS_PSEUDO(x)    ((x==0x0001 || x==0x0002)?TRUE:FALSE) // AND 때문에 이와 같이 변경 
#define IS_RRI(Y)       ((Y>0x7000)?TRUE:FALSE) // RRI를 구분하기 위해 
// pseudo instructions
#define DEC_INST        0x0001
#define HEX_INST        0x0002

// machine instructions
#define AND_INST	0x0000
#define ADD_INST	0x1000
#define LDA_INST	0x2000
#define STA_INST	0x3000
#define BUN_INST	0x4000
#define BSA_INST	0x5000
#define ISZ_INST	0x6000
#define CLA_INST	0x7800
#define CLE_INST	0x7400
#define CMA_INST	0x7200
#define CME_INST	0x7100
#define CIR_INST	0x7080
#define CIL_INST	0x7040
#define INC_INST	0x7020
#define SPA_INST	0x7010
#define SNA_INST	0x7008
#define SZA_INST	0x7004
#define SZE_INST	0x7002
#define HLT_INST	0x7001
#define INP_INST	0xF800
#define OUT_INST	0xF400
#define SKI_INST	0xF200
#define SKO_INST	0xF100
#define ION_INST	0xF080
#define IOF_INST	0xF040
#define SET_INDIRECT(x) ( x | 0x8000) // indirect 모드일때 0x8000을 instruction에 더해주는 효과 

struct tagTABLE {
    char lable[MAX_LABLE_LEN];
    short location;
} TABLE[MAX_TABLE_SZ];  // 주소-기호 테이블 구조체

int num_lables = 0;  // lable 개수

char* delimiters = " \n"; // 구분자

short BINCODE[1000];

void insert_lable(char* lable, short loc)  // lable를 주소-기호 테이블에 넣는 함수
{
    int ctr = 0;
    for (int i = 0; i < 4; i++)  // lable의 글자 수를 세기 위한 반복문(최대 글자수는 3글자)
    {
        if (lable[i] == ',') { break; }  // ','를 만날때 까지 ctr(카운터) 변수를 1씩 증가시킴
        else { ctr++; }
    }
    strncpy(TABLE[num_lables].lable, lable, ctr); // ','를 제외하고 lable 복사
    TABLE[num_lables].location = loc;
    num_lables++;
}

short get_opcode(char* str)  // symbol에 해당하는 hex code를 가져오는 함수
{
    if (!strcmp(str, "ADD")) return ADD_INST;
    else if (!strcmp(str, "AND")) return AND_INST;
    else if (!strcmp(str, "LDA")) return LDA_INST;
    else if (!strcmp(str, "STA")) return STA_INST;
    else if (!strcmp(str, "BUN")) return BUN_INST;
    else if (!strcmp(str, "BSA")) return BSA_INST;
    else if (!strcmp(str, "ISZ")) return ISZ_INST;
    else if (!strcmp(str, "CLA")) return CLA_INST;
    else if (!strcmp(str, "CLE")) return CLE_INST;
    else if (!strcmp(str, "CMA")) return CMA_INST;
    else if (!strcmp(str, "CME")) return CME_INST;
    else if (!strcmp(str, "CIR")) return CIR_INST;
    else if (!strcmp(str, "CIL")) return CIL_INST;
    else if (!strcmp(str, "INC")) return INC_INST;
    else if (!strcmp(str, "SPA")) return SPA_INST;
    else if (!strcmp(str, "SNA")) return SNA_INST;
    else if (!strcmp(str, "SZA")) return SZA_INST;
    else if (!strcmp(str, "SZE")) return SZE_INST;
    else if (!strcmp(str, "HLT")) return HLT_INST;
    else if (!strcmp(str, "INP")) return INP_INST;
    else if (!strcmp(str, "OUT")) return OUT_INST;
    else if (!strcmp(str, "SKI")) return SKI_INST;
    else if (!strcmp(str, "SKO")) return SKO_INST;
    else if (!strcmp(str, "ION")) return ION_INST;
    else if (!strcmp(str, "IOF")) return IOF_INST;
    else if (!strcmp(str, "DEC")) return DEC_INST;
    else if (!strcmp(str, "HEX")) return HEX_INST;
}

short search_location(char* lable)
{
    int i;

    for (i = 0; i < num_lables; i++)
        if (!strcmp(TABLE[i].lable, lable)) return TABLE[i].location;

    return -1;
}

BOOL is_lable(char* string)  // 읽어들인 token이 lable인지 판단하는 함수
{
    for (int i = 0; i < 4; i++)  // lable의 최대 글자 수는 3글자
    {
        if (string[i] == ',') { return TRUE; }
    }
    return FALSE;
}

int get_tokens(char* line, char* token[])
{
    char* pstr;
    int counter = 0;

    if ((pstr = strtok(line, delimiters)) != NULL) {
        token[counter] = pstr;
        if (pstr[0] == '/' || pstr[0] == '\n') return counter; // if the comment line is encontered
            counter++;
        while ((pstr = strtok((char*)NULL, delimiters)) != NULL) {
            token[counter] = pstr;
            if (pstr[0] == '/' || pstr[0] == '\n') return counter; // if the comment line is encontered
                counter++;
        }
    }
    
    return counter;
}


int* ConvertHexToBin(char c)  // 16진수를 2진수를 변환하는 함수
{
    int n = 0;
    static int arr[4];
    n = (int)c - 48; // 숫자가 아닐경우 A를 10으로 만들어줌 
    if (n > 9) n -= 7;
    for (int i = 0; i < 4; i++) {
        arr[i] = (n % 2); n /= 2;
    }
    return arr;
}

void output_bin(FILE * fp2, unsigned short ins, int LC)  // 이진 형태의 output 파일을 출력하기 위한 함수
{
    char ins_string[50];  // unsigned short ins를 문자열로 저장하기 위한 배열
    char lc_string[50];  // 정수 LC를 문자열로 저장하기 위한 배열
    int* bin_value;
    int* lc_value;
    
    // LC를 이진수로 쓰기
    sprintf(lc_string, "%x", LC);
    for (int i = 0; i < strlen(lc_string); i++) {
        lc_value = ConvertHexToBin(lc_string[i]);
        for (int j = 3; j > -1; j--) {
            fprintf(fp2, "%d", lc_value[j]);
            //printf("%d", lc_value[j]);
        }
    }
    fputs(" ", fp2);
    //printf(" ");

    // ins를 이진수로 쓰기
    sprintf(ins_string, "%4x", ins);
    for (int i = 0; i < strlen(ins_string); i++) {
        bin_value = ConvertHexToBin(ins_string[i]);
        for (int j = 3; j >= 0; j--) {
            fprintf(fp2, "%d", bin_value[j]);  // sprintf, fputs를 대체
            //printf("%d", bin_value[j]);
        }
        //printf(" ");
        fputs(" ", fp2);
    }
    fputs("\n", fp2);
}


main(void)
{
    char line[MAX_LINE_LEN];
    char* token[10];
    int  num_token;
    FILE* fp, * fp2, * fp3;
    unsigned short instruction; // 자료형을 unsigned short으로 변경해줌 
    unsigned short loc;
    unsigned short opcode;
    int   LC, opcode_idx;
    int outh;                  // 16진수로 화면에 출력할 값을 저장하기 위해 사용할 임시 변수 

    fp = fopen("input12.txt", "r");  // 입력 파일을 input.txt로 설정
    if (fp == NULL) {
        printf("Error: Failed to open file");
        exit(0);
    }

    // Beginning of the 1st Pass 
    LC = 0;
    while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
        num_token = get_tokens(line, token);
        if (num_token == 0) continue;

        if (is_lable(token[0])) {
            insert_lable(token[0], LC);
        }
        else if (!strcmp(token[0], "ORG")) {
            LC = (int)strtol(token[1], NULL, 16);
            continue;
        }
        LC++;
    }

    // Beginning of the 2nd Pass
    rewind(fp);
    LC = 0;
    while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
        num_token = get_tokens(line, token);
        if (num_token == 0) continue;
        else if (num_token == 1 && is_lable(token[0])) {
            token[1] = "HEX";
            token[2] = "0";
        }
        /*else if (num_token == 1 && is_lable(token[0]) && (strcmp(token[0], "CTR,") > 0 || strcmp(token[0], "CTR,") < 0)) {
            token[1] = "HEX";
            token[2] = "0";
        }
        else if (num_token == 1 && is_lable(token[0]) && !strcmp(token[0], "CTR,")) {
            token[1] = "DEC";
            token[2] = "0";
        }*/

        instruction = 0;

        if (is_lable(token[0])) opcode_idx = 1;
        else if (!strcmp(token[0], "ORG")) {
            LC = (int)strtol(token[1], NULL, 16);
            continue;
        }
        else if (!strcmp(token[0], "END")) {
            break;
        }
        else opcode_idx = 0;

        opcode = get_opcode(token[opcode_idx]);
        fp2 = fopen("output12_bin.txt", "a"); // 2진 출력 파일을 output_bin.txt으로 설정
        fp3 = fopen("output12_hex.txt", "a"); // 16진 출력 파일을 output_hex.txt으로 설정

        if (IS_PSEUDO(opcode)) {
            if (opcode == 0x0001) // DEC이면 10진수를 16진수로 출력 
            {
                if (!is_lable(token[0])) {  // lable이 없는 DEC일 경우 token[1]에서 operand 가져옴
                    outh = (int)strtol(token[1], NULL, 10);
                }
                else {  // lable이 있는 DEC일 경우 token[2]에서 operand 가져옴
                    outh = (int)strtol(token[2], NULL, 10);
                }
                output_bin(fp2, outh, LC);  // 오픈한 txt 파일에 이진수 쓰기
                printf("[%4hx]  %04hx \n", LC, outh);
                fprintf(fp3, "%x %04hx\n", LC, outh);
            }
            else                  // HEX 이면 그대로 출력 
            {
                if (!is_lable(token[0])) {  // lable이 없는 HEX일 경우 token[1]에서 operand 가져옴
                    outh = (int)strtol(token[1], NULL, 16);
                }
                else {  // lable이 있는 HEX일 경우 token[2]에서 operand 가져옴
                    outh = (int)strtol(token[2], NULL, 16);
                }
                //outh = (int)strtol(token[2], NULL, 16);
                output_bin(fp2, outh, LC);  // 오픈한 txt 파일에 이진수 쓰기
                printf("[%4hx]  %04hx \n", LC, outh);
                fprintf(fp3, "%x %04hx\n", LC, outh);
            }

        }
        else if (IS_RRI(opcode)) // RRI 이면 opcode만 출력 
        {
            output_bin(fp2, opcode, LC);  // 오픈한 txt 파일에 이진수 쓰기
            printf("[%4hx]  %04hx  \n", LC, opcode);  // location과 opcode를 화면에 출력
            fprintf(fp3, "%x %04x\n", LC, opcode);
        }
        else   // 나머지 경우(Memory Reference Instruction)
        {
            instruction |= opcode;
            if (!is_lable(token[0]) && (num_token == 3)) {  // lable이 없고 간접모드 명령어일 경우
                if (!strcmp(token[2], "I"))  // 토큰에 값이 존재하지 않는데(lable만 있을 경우) 접근할 경우 오류 발생. 때문에 위 조건문으로 접근 통제
                    instruction = SET_INDIRECT(instruction);
            }
            else if (is_lable(token[0]) && (num_token == 4)) {  // lable이 있는 간접모드 명령어일 경우
                if(!strcmp(token[3], "I"))   // 바로 token[3]으로 접근하면 오류 나서 num_token으로 토큰 개수 먼저 체크
                    instruction = SET_INDIRECT(instruction);
            }
            // INDIRECT 모드 이면 0x8000과 OR 연산을 해주고 다시 instruction에 저장 
            loc = search_location(token[opcode_idx + 1]);
            if (loc == -1) {
                printf("Error: Invalid Lable!!! \a\a\n");
                exit(0);
            }

            instruction |= loc;  // instruction과 location을 더해서 instruction에 저장한다
            output_bin(fp2, instruction, LC);  // 오픈한 txt 파일에 이진수 쓰기
            printf("[%4hx]  %04hx  \n", LC, instruction);  // 화면에 location과 instruction 출력
            fprintf(fp3, "%x %04x\n", LC, instruction);
            //fclose(fp2); // txt 파일을 닫는다 // 이거 하나 때문에 파일 출력이 이상했다.. 이거 찾는데만 3시간..
            BINCODE[LC] = instruction;

        }
        LC++;
        fclose(fp2);  // txt 파일을 닫는다
        fclose(fp3);
    }
    system("pause");
}
