#include <reg51.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_STRING_LENGTH 20

unsigned char rough_str[MAX_STRING_LENGTH];
float result;
unsigned int entries,check;

typedef enum {
    STATE_INIT,
    STATE_WAIT_INPUT,
    STATE_PROCESS_INPUT,
    STATE_DISPLAY_NAME,
    STATE_DISPLAY_ROLLNO,
    STATE_EXIT
} State;

void delay(unsigned int time) {
    unsigned int i;
    for (i = 0; i < time; i++) {
        TH0 = 0xF8;
        TL0 = 0xCC;
        TR0 = 1;
        while (TF0 == 0);
        TR0 = 0;
        TF0 = 0;
    }
}
void uart_intit(){
	
	 TMOD = 0x20;
    TH1 = 0xFA;
    SCON = 0x50;
    TR1 = 1;
	 delay(5);
}



void transmitted_data(const char *str) {
    while (*str) {
        SBUF = *str;
        while (TI == 0);
        TI = 0;
        str++;
    }
}

void trans_data(unsigned char ch) {
    SBUF = ch;
    while (TI == 0);
    TI = 0;
}

unsigned char received_data() {
    while (RI == 0);
    RI = 0;
    return SBUF;
}

void rec_data(char *str, int length) {
    int i;
    for (i = 0; i < length - 1; i++) {
        str[i] = received_data();
        if (str[i] == '\r' || str[i] == '\n') {
            break;
        }
    }
    str[i] = '\0';
}

void cal_fun() {
    float number_1 , number_2 ;
    unsigned char choice;
	  unsigned int flag =1;
   
	if(entries == 0 ){

    transmitted_data("\n \t \t CALCULATOR \n");
		transmitted_data("\n \t Oprerations to be Performed .... \n");
	//TODO:
	delay(10);
	}

    transmitted_data("\n \t A: ADDITION \n \t D DIVISION \n \t M: MULTIPLICATION \n \t S : SUBTRACTION ");
	  transmitted_data("\n Enter Your Choice : ");
  	choice = received_data();
    received_data();
    trans_data(choice);
	 transmitted_data("\n Number -1 : ");
   if(entries == 0){ 
//    transmitted_data("\n Number -1 : ");
    rec_data(rough_str, MAX_STRING_LENGTH);
    received_data();
    number_1 = atof(rough_str);
		
		}else{
	      number_1 = result;
			strcat(rough_str, "Result");
}


    strcat(rough_str, "\n Number - 2: ");
	  transmitted_data(rough_str);
    memset(rough_str, '\0', MAX_STRING_LENGTH);

    rec_data(rough_str, MAX_STRING_LENGTH);
    received_data();
	  transmitted_data(rough_str);
    number_2 = atof(rough_str);
    memset(rough_str, '\0', MAX_STRING_LENGTH);
  
    switch (choice) {
        case 'A':
            result = number_1 + number_2;
            break;
        case 'D':
            if (number_2 == 0) {
                transmitted_data("\n infinity");
							  flag = 0;
            } else {
                result  = number_1 / number_2;
            }
            break;
        case 'M':
            result  = number_1 * number_2;
            break;
        case 'S':
           result = number_1 - number_2;
            break;
        default:
					transmitted_data("\n U ENTER WRONG CHOICE .... \n");
				   flag =0;
            break;
    }

    // Simplified float to string conversion
		if(flag == 1){
			sprintf(rough_str, "\n Result : %.2f \n", result );
     transmitted_data(rough_str);
		 memset(rough_str,'\0',sizeof(rough_str));
			
		}
		flag = 1;
}

int fsm_state() {
    State curr_state = STATE_INIT;
    while (1) {
        switch (curr_state) {
            case STATE_INIT:
                curr_state = STATE_WAIT_INPUT;
						    
                break;
            case STATE_WAIT_INPUT:
							 entries = 0;

                cal_fun();
						    transmitted_data("\n If You Still Want to Continue (Y|N) ......  : ");
						   check = received_data();
						    received_data();
						    trans_data(check);
						
						   if(check == 'Y'){
								 entries = 1;
								 curr_state = STATE_PROCESS_INPUT;
							 }
							 else{
								   transmitted_data("\n If You Still Want to Stay or EXIT(S|E) ......  : ");
								    check = received_data();
						    received_data();
						    trans_data(check);
								 if(check == 'S'){
									 curr_state = STATE_WAIT_INPUT;
								 }
								 else{
								 transmitted_data("\n OK Program is End  .... " );
                   curr_state = STATE_EXIT;	
								 }									 
							 }
                break;
            case STATE_PROCESS_INPUT:
							 transmitted_data(" \n Next operation do you want to perform :" );
						       cal_fun();
							     transmitted_data("\n If You Still Want to Continue (Y|N) ......  : ");
						    check = received_data();
						    received_data();
						    trans_data(check);
						
						   if(check == 'Y'){
								 entries = 1;
								 curr_state = STATE_PROCESS_INPUT;
							 }
							 else{
								   transmitted_data("\n If You Still Want to Stay or EXIT(S|E) ......  : ");
								    check = received_data();
						    received_data();
						    trans_data(check);
								 if(check == 'S'){
									 curr_state = STATE_WAIT_INPUT;
								 }
								 else{
								 transmitted_data("\n OK Program is End  .... " );
                   curr_state = STATE_EXIT;	
								 }									 
							 }
						           
						           //curr_state = STATE_PROCESS_INPUT;
						

            case STATE_EXIT:
							         curr_state = STATE_EXIT;
							         
							          //break;
                    while (1);
 
                break;
            default:
							 curr_state = STATE_EXIT;
							         
							          //break;
                    while (1);
                break;
        }
    }
}

void main() {
	uart_intit();
    delay(5);
    fsm_state();
}
