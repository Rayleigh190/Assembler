/*
    ////////////////////////////////////////////////////////
    Computer System Architecture Team Project

    Team Name : 5
    Assignment : Assembler
    Project period : 2021-10-06 ~ 2021-11-15
    ////////////////////////////////////////////////////////

    [Work history]

    <2021-10-24> - �ֿ���
     1. �⺻ ��� ���� (First, Second Pass)
     2. (ǥ 6-5) ���α׷� �׽�Ʈ �� ���� (input.txt)
     3. (ǥ 6-8) ���α׷� �׽�Ʈ �� ���� �߻�. lable ���� �� �ν� �ʿ�
     4. output.bin�� ���ϴ� ���� �� ����. ���� ���� ���ڿ��μ� txt ���Ϸ� ����� �ʿ䰡 ����

    <2021-10-31> - �ֿ���
     1. lable ���� ���� �ν��ϱ� ���� is_lable �Լ��� insert_lable �Լ��� ����
     2. 16������ 2������ ��ȯ �ϱ� ���� ConvertHexToBin �Լ��� �߰�
     3. ���� ���� ���ڿ� ������ output.txt ���Ϸ� ����ϱ� ���� output_bin �Լ��� �߰�
     4. fwrite �Լ��� output_bin �Լ��� ��ü
     5. input, output ������ asm, bin�� �ƴ϶� txt ���Ϸ� ����
     6. (ǥ 6-8) ���α׷� �׽�Ʈ �� ���� (input2.txt)
     7. interface ��� ���¿� ���� ���ǰ� �ʿ�
    
    <2021-11-02> - �ֿ���
     1. input2.txt ���� �Է½� output ���� ���� ������ ���� ��µ�. ���� �ʿ�
     2. hex ��� ���� �ʿ�
     3. ���ͷ�Ʈ ��� ����?
     4. output ���� ���� ������ ���� ��� �Ǵ� ���� �ذ� (fclose �Լ� ��ġ�� while�� ������ �ٷ� ����)
     5. ����� output_bin.txt, output_hex.txt �� ���� 2��, 16�� ���� �Ѵ� ��� �ǵ��� ����
     6. (ǥ 6-13) ���α׷� �׽�Ʈ�� lable ���� DEC, HEX���� ���� �߻�
        ��2nd pass�� if (IS_PSEUDO(opcode)) �ȿ� lable ���� DEC, HEX�� üũ�ϴ� ���ǹ��� �߰��Ͽ� �ذ�
     7. (ǥ 6-13) ���α׷� �׽�Ʈ�� lable �ִ� ������� ��ɾ�� 'I'�� �ν� �� �ϴ� ���� �߻�
        ��2nd pass�� MRI ���ǹ� �ȿ� lable�� �ִ� ������� ����� üũ�ϴ� ���ǹ��� �߰��Ͽ� �ذ�

    <2021-11-03> - �ֿ���
     1. 4�ڸ� ���� ���ų� ������ 4�ڸ� �̻� Ȥ�� ���Ϸ� ��� �Ǵ� ������ ���������� �������� �ذ�, �� �ڸ��� 0���� ä���.
     2. token[3]�� �ٷ� �����ϸ� ���� �߻�. (�� token ���� �� ��)
        ��num_token���� ��ū ���� ���� üũ�ϰ� num_token�� 4�� ���� token[3]�� ���� �����ϵ��� �����Ͽ� �ذ�

    <2021-11-04> - �ֿ���
     1. input ����� ���α׷��� lable�� ������ �ڿ� �ƹ��͵� ���� ��� ���α׷��� ���� ������ �� �Ǵ� ���� �߻� (������ �ʱ�ȭ�� �� �Ǿ��� ����)
        ��lable�� ������ �ڿ� �ƹ��͵� ���� ��� 2nd pass���� lable�� "CTR,"�� ���� ���Ƿ� token[1]="DEC", token[2]="0" �ϰ� ������ lable�� ���� token[1]="HEX", token[2]="0"�� �ϵ��� �����Ͽ� �ذ�

    <2021-11-07> - �ֿ���
     1. lable�� ������ lable�� ���簣�� token[1]="HEX", token[2]="0"�� �ϵ��� ����
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

#define IS_PSEUDO(x)    ((x==0x0001 || x==0x0002)?TRUE:FALSE) // AND ������ �̿� ���� ���� 
#define IS_RRI(Y)       ((Y>0x7000)?TRUE:FALSE) // RRI�� �����ϱ� ���� 
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
#define SET_INDIRECT(x) ( x | 0x8000) // indirect ����϶� 0x8000�� instruction�� �����ִ� ȿ�� 

struct tagTABLE {
    char lable[MAX_LABLE_LEN];
    short location;
} TABLE[MAX_TABLE_SZ];  // �ּ�-��ȣ ���̺� ����ü

int num_lables = 0;  // lable ����

char* delimiters = " \n"; // ������

short BINCODE[1000];

void insert_lable(char* lable, short loc)  // lable�� �ּ�-��ȣ ���̺� �ִ� �Լ�
{
    int ctr = 0;
    for (int i = 0; i < 4; i++)  // lable�� ���� ���� ���� ���� �ݺ���(�ִ� ���ڼ��� 3����)
    {
        if (lable[i] == ',') { break; }  // ','�� ������ ���� ctr(ī����) ������ 1�� ������Ŵ
        else { ctr++; }
    }
    strncpy(TABLE[num_lables].lable, lable, ctr); // ','�� �����ϰ� lable ����
    TABLE[num_lables].location = loc;
    num_lables++;
}

short get_opcode(char* str)  // symbol�� �ش��ϴ� hex code�� �������� �Լ�
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

BOOL is_lable(char* string)  // �о���� token�� lable���� �Ǵ��ϴ� �Լ�
{
    for (int i = 0; i < 4; i++)  // lable�� �ִ� ���� ���� 3����
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


int* ConvertHexToBin(char c)  // 16������ 2������ ��ȯ�ϴ� �Լ�
{
    int n = 0;
    static int arr[4];
    n = (int)c - 48; // ���ڰ� �ƴҰ�� A�� 10���� ������� 
    if (n > 9) n -= 7;
    for (int i = 0; i < 4; i++) {
        arr[i] = (n % 2); n /= 2;
    }
    return arr;
}

void output_bin(FILE * fp2, unsigned short ins, int LC)  // ���� ������ output ������ ����ϱ� ���� �Լ�
{
    char ins_string[50];  // unsigned short ins�� ���ڿ��� �����ϱ� ���� �迭
    char lc_string[50];  // ���� LC�� ���ڿ��� �����ϱ� ���� �迭
    int* bin_value;
    int* lc_value;
    
    // LC�� �������� ����
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

    // ins�� �������� ����
    sprintf(ins_string, "%4x", ins);
    for (int i = 0; i < strlen(ins_string); i++) {
        bin_value = ConvertHexToBin(ins_string[i]);
        for (int j = 3; j >= 0; j--) {
            fprintf(fp2, "%d", bin_value[j]);  // sprintf, fputs�� ��ü
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
    unsigned short instruction; // �ڷ����� unsigned short���� �������� 
    unsigned short loc;
    unsigned short opcode;
    int   LC, opcode_idx;
    int outh;                  // 16������ ȭ�鿡 ����� ���� �����ϱ� ���� ����� �ӽ� ���� 

    fp = fopen("input12.txt", "r");  // �Է� ������ input.txt�� ����
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
        fp2 = fopen("output12_bin.txt", "a"); // 2�� ��� ������ output_bin.txt���� ����
        fp3 = fopen("output12_hex.txt", "a"); // 16�� ��� ������ output_hex.txt���� ����

        if (IS_PSEUDO(opcode)) {
            if (opcode == 0x0001) // DEC�̸� 10������ 16������ ��� 
            {
                if (!is_lable(token[0])) {  // lable�� ���� DEC�� ��� token[1]���� operand ������
                    outh = (int)strtol(token[1], NULL, 10);
                }
                else {  // lable�� �ִ� DEC�� ��� token[2]���� operand ������
                    outh = (int)strtol(token[2], NULL, 10);
                }
                output_bin(fp2, outh, LC);  // ������ txt ���Ͽ� ������ ����
                printf("[%4hx]  %04hx \n", LC, outh);
                fprintf(fp3, "%x %04hx\n", LC, outh);
            }
            else                  // HEX �̸� �״�� ��� 
            {
                if (!is_lable(token[0])) {  // lable�� ���� HEX�� ��� token[1]���� operand ������
                    outh = (int)strtol(token[1], NULL, 16);
                }
                else {  // lable�� �ִ� HEX�� ��� token[2]���� operand ������
                    outh = (int)strtol(token[2], NULL, 16);
                }
                //outh = (int)strtol(token[2], NULL, 16);
                output_bin(fp2, outh, LC);  // ������ txt ���Ͽ� ������ ����
                printf("[%4hx]  %04hx \n", LC, outh);
                fprintf(fp3, "%x %04hx\n", LC, outh);
            }

        }
        else if (IS_RRI(opcode)) // RRI �̸� opcode�� ��� 
        {
            output_bin(fp2, opcode, LC);  // ������ txt ���Ͽ� ������ ����
            printf("[%4hx]  %04hx  \n", LC, opcode);  // location�� opcode�� ȭ�鿡 ���
            fprintf(fp3, "%x %04x\n", LC, opcode);
        }
        else   // ������ ���(Memory Reference Instruction)
        {
            instruction |= opcode;
            if (!is_lable(token[0]) && (num_token == 3)) {  // lable�� ���� ������� ��ɾ��� ���
                if (!strcmp(token[2], "I"))  // ��ū�� ���� �������� �ʴµ�(lable�� ���� ���) ������ ��� ���� �߻�. ������ �� ���ǹ����� ���� ����
                    instruction = SET_INDIRECT(instruction);
            }
            else if (is_lable(token[0]) && (num_token == 4)) {  // lable�� �ִ� ������� ��ɾ��� ���
                if(!strcmp(token[3], "I"))   // �ٷ� token[3]���� �����ϸ� ���� ���� num_token���� ��ū ���� ���� üũ
                    instruction = SET_INDIRECT(instruction);
            }
            // INDIRECT ��� �̸� 0x8000�� OR ������ ���ְ� �ٽ� instruction�� ���� 
            loc = search_location(token[opcode_idx + 1]);
            if (loc == -1) {
                printf("Error: Invalid Lable!!! \a\a\n");
                exit(0);
            }

            instruction |= loc;  // instruction�� location�� ���ؼ� instruction�� �����Ѵ�
            output_bin(fp2, instruction, LC);  // ������ txt ���Ͽ� ������ ����
            printf("[%4hx]  %04hx  \n", LC, instruction);  // ȭ�鿡 location�� instruction ���
            fprintf(fp3, "%x %04x\n", LC, instruction);
            //fclose(fp2); // txt ������ �ݴ´� // �̰� �ϳ� ������ ���� ����� �̻��ߴ�.. �̰� ã�µ��� 3�ð�..
            BINCODE[LC] = instruction;

        }
        LC++;
        fclose(fp2);  // txt ������ �ݴ´�
        fclose(fp3);
    }
    system("pause");
}
