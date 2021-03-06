/* 
 * File:   only_on_off.c
 * Author: Varun sahni
 *client: test
 * module: this is working code for 11 switches on and off only manul and application
 */

#include <stdio.h>
#include <stdlib.h>
// 'C' source line config statements
// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection (HS Oscillator, High-speed crystal/resonator connected between OSC1 and OSC2 pins)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable bit (VCAP pin function disabled)
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will not cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
// Since we have used 16 MHz crystal
#define _XTAL_FREQ 16000000  

// Pin MACROS
#define OUTPUT_RELAY1 PORTBbits.RB1
#define OUTPUT_RELAY2 PORTCbits.RC1
#define OUTPUT_RELAY3 PORTAbits.RA0
#define OUTPUT_RELAY4 PORTFbits.RF1
#define OUTPUT_RELAY5 PORTAbits.RA3
#define OUTPUT_RELAY6 PORTAbits.RA1
#define OUTPUT_RELAY7 PORTAbits.RA2
#define OUTPUT_RELAY8 PORTBbits.RB3
#define OUTPUT_RELAY9 PORTAbits.RA5
#define OUTPUT_RELAY10 PORTCbits.RC0
#define OUTPUT_RELAY11 PORTFbits.RF0


#define OUTPUT_RELAY_DIR_1 TRISBbits.TRISB1
#define OUTPUT_RELAY_DIR_2 TRISCbits.TRISC1
#define OUTPUT_RELAY_DIR_3 TRISAbits.TRISA0
#define OUTPUT_RELAY_DIR_4 TRISFbits.TRISF1
#define OUTPUT_RELAY_DIR_5 TRISAbits.TRISA3
#define OUTPUT_RELAY_DIR_6 TRISAbits.TRISA1
#define OUTPUT_RELAY_DIR_7 TRISAbits.TRISA2
#define OUTPUT_RELAY_DIR_8 TRISBbits.TRISB3
#define OUTPUT_RELAY_DIR_9 TRISAbits.TRISA5
#define OUTPUT_RELAY_DIR_10 TRISCbits.TRISC0
#define OUTPUT_RELAY_DIR_11 TRISFbits.TRISF0


#define INPUTSWITCH1 RF2
#define INPUTSWITCH2 RF3
#define INPUTSWITCH3 RF4
#define INPUTSWITCH4 RF5
#define INPUTSWITCH5 RF6
#define INPUTSWITCH6 RD7
#define INPUTSWITCH7 RD6
#define INPUTSWITCH8 RD5
#define INPUTSWITCH9 RD4
#define INPUTSWITCH10 RD3
#define INPUTSWITCH11 RD2


#define INPUT_SWITCH_DIR_1 TRISFbits.TRISF2
#define INPUT_SWITCH_DIR_2 TRISFbits.TRISF3
#define INPUT_SWITCH_DIR_3 TRISFbits.TRISF4
#define INPUT_SWITCH_DIR_4 TRISFbits.TRISF5
#define INPUT_SWITCH_DIR_5 TRISFbits.TRISF6
#define INPUT_SWITCH_DIR_6 TRISDbits.TRISD7
#define INPUT_SWITCH_DIR_7 TRISDbits.TRISD6
#define INPUT_SWITCH_DIR_8 TRISDbits.TRISD5
#define INPUT_SWITCH_DIR_9 TRISDbits.TRISD4
#define INPUT_SWITCH_DIR_10 TRISDbits.TRISD3
#define INPUT_SWITCH_DIR_11 TRISDbits.TRISD2


// direction of PWM OUTPUT to MOC3021

/*
 * Extra Periferals Direction and PORT
 */
//#define ZCD_CCP9_DIR TRISEbits.TRISE3
// USART Directions
#define USART_1_TRANSMIT_OUTPUT_DIR TRISCbits.TRISC6
#define USART_1_RECIEVE_INPUT_DIR TRISCbits.TRISC7

#define RECIEVED_DATA_LENGTH (16+2)
#define TOTAL_NUMBER_OF_SWITCH (12+2)

#define TRUE 1
#define FALSE 0



// Conditional compilation
//#define DEBUG
//#define RELEASE
#define SWITCH_7_RELAY
#define SWITCH_8_RELAY
#define SWITCH_9_RELAY
#define SWITCH_10_RELAY
#define SWITCH_11_RELAY

// ALL error Definitions
/* 
 * #define WRONG_DATA_RECIEVED_ERROR_CODE ERRX
 * #define RECIVING_OVERRUN_ERROR EROV
 * #define RECEIVING_DATA_LOST_IN_MAIN ERLS
 */
/* DATA USED IN MANUAL  STARTS HERE*/



#define ON 1
#define OFF 0
#define CHAR_OFF '0'
#define CHAR_ON '1'
        
/* DATA USED IN MANUAL END HERE*/

unsigned char ErrorNames[5]="####";

int mainReceivedDataPosition=0, mainDataReceived=FALSE;
unsigned char mainReceivedDataBuffer[RECIEVED_DATA_LENGTH]="#"; 
unsigned char tempReceivedDataBuffer[RECIEVED_DATA_LENGTH-8]="#";
unsigned char parentalLockBuffer[TOTAL_NUMBER_OF_SWITCH]="000000000000";
unsigned char copy_parentalLockBuffer[TOTAL_NUMBER_OF_SWITCH]="000000000000";
unsigned char currentStateBuffer[(TOTAL_NUMBER_OF_SWITCH*4)+2]="#";

unsigned int M1;unsigned int M2;unsigned int M3;unsigned int M4;
unsigned int M5;unsigned int M6;unsigned int M7;unsigned int M8;
unsigned int M9;unsigned int M10;unsigned int M11;unsigned int M12;
int start_PWM_Generation_in_ISR_FLAG=FALSE;
char levelofDimmer_MSB='0',levelofDimmer_LSB='0';

void errorsISR(char* errNum);
void errorsMain(char* errNum);
void sendAcknowledgment(char* currentStateBuffer);

void clearAllPorts();
void pinINIT_extra();
void GPIO_pin_Initialize();
void peripheralsEnable();
void AllInterruptEnable();
void EUSART_Initialize();

void TMR3_Initialize();
void TMR1_Initialize();
void CCP9_Initialize();
void allPeripheralInit();

void copyReceivedDataBuffer();

void applianceControl(char switchMSB, char switchLSB, char switchSTATE, char dimmerSpeedMSB, char dimmerSpeedLSB, char parentalControl, char finalFrameState);

interrupt void isr(){
 
    // ************************************* UART INTERRUPT *********************************************** //
    if(RC1IF){        
        if(RC1STAbits.OERR){    // If over run error, then reset the receiver
            RC1STAbits.CREN = 0; // countinuous Recieve Disable
            RC1STAbits.CREN = 1; // countinuous Recieve Enable
            
            ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='O';      ErrorNames[3]='V';
            errorsISR(ErrorNames); 
        } 
        
        mainReceivedDataBuffer[mainReceivedDataPosition]=RC1REG;
        #ifdef DEBUG
        TX1REG=mainReceivedDataBuffer[mainReceivedDataPosition];
        #endif
        if(mainReceivedDataBuffer[0]=='%'){
            mainReceivedDataPosition++;
            if(mainReceivedDataPosition>15){
                mainDataReceived=TRUE;
                mainReceivedDataPosition=0;                
                RC1IF=0;                
            }
        }
        else{
            RC1STAbits.CREN = 0; // countinuous Recieve Disable
            RC1STAbits.CREN = 1; // countinuous Recieve Enable
            mainReceivedDataPosition=0; // Reinitiate buffer counter
            
            ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='R';      ErrorNames[3]='X';
            errorsISR(ErrorNames);            
        }
    }// End of RC1IF 
}




/*
 * Alfaone Main code starts here
 * For 4 switches 1 Dimmer
 */
int main() {
    __delay_ms(2000);
        M1=ON;    M2=ON;    M3=ON;    M4=ON;    M5=ON; M6=ON; M7=ON;M8=ON;
        M9=ON; M10=ON; M11=ON;M12=ON;
//        OUTPUT_RELAY1 = OFF;  OUTPUT_RELAY2 = OFF;    OUTPUT_RELAY3 = OFF;  OUTPUT_RELAY4 = OFF;
//        OUTPUT_RELAY5 = OFF;  OUTPUT_RELAY6 = OFF;    OUTPUT_RELAY7 = OFF;  OUTPUT_RELAY8 = OFF;
            GPIO_pin_Initialize();
            allPeripheralInit();
    
    while(1){       
        if(mainDataReceived==TRUE){
            mainDataReceived=FALSE;
            if(mainReceivedDataBuffer[0]=='%' && mainReceivedDataBuffer[1]=='%' && mainReceivedDataBuffer[14]=='@' && mainReceivedDataBuffer[15]=='@'){
                copyReceivedDataBuffer();
                
                applianceControl(tempReceivedDataBuffer[0],
                        tempReceivedDataBuffer[1],
                        tempReceivedDataBuffer[2],
                        tempReceivedDataBuffer[3],
                        tempReceivedDataBuffer[4],
                        tempReceivedDataBuffer[5],
                        tempReceivedDataBuffer[6]);
                                
            }   // End of all buffer data check
            else
            {
                ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='L';      ErrorNames[3]='S';
                errorsMain(ErrorNames);
                RC1STAbits.SPEN=0;  // Serial port disabled 
                RC1STAbits.CREN = 0; // countinuous Recieve Disable                
                for(int dataBufferCounter = 0; dataBufferCounter< 15; dataBufferCounter++)
                {
                    mainReceivedDataBuffer[dataBufferCounter] = '#'; // clean received data buffer
                }
                RC1STAbits.CREN = 1; // countinuous Recieve Enable
                RC1STAbits.SPEN=1;  // Serial port enabled (configures RXx/DTx and TXx/CKx pins as serial port pins)
            }
        } // End of mainDataReceived condition

        /******************** MANUAL RESPONE STARTS HERE************ */
        
        //check switch one status
        //off condition
       int man = 1;
        if(copy_parentalLockBuffer[1] == CHAR_OFF  && INPUTSWITCH1 == OFF && M1 == OFF)
        {
            if(man == 1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            OUTPUT_RELAY1=OFF;
            }
            man=0;
            M1=1;
            
        }
        //on condition
        if(copy_parentalLockBuffer[1] == CHAR_OFF && INPUTSWITCH1 == ON && M1 == ON)
        {
            //TX1REG='C';
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            OUTPUT_RELAY1=ON;
            }
            man=0;
            M1=0;
        }
        
       // //check switch second status 
        //off condition
        if(copy_parentalLockBuffer[2] == CHAR_OFF && INPUTSWITCH2 == OFF && M2 == OFF)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '2';__delay_ms(1);
            OUTPUT_RELAY2=OFF;
            }
            man=0;
            M2=1;
        }
        //on condtion
        if(copy_parentalLockBuffer[2] == CHAR_OFF && INPUTSWITCH2 == ON && M2 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '2';__delay_ms(1);
            OUTPUT_RELAY2=ON;
            }
            man=0;
            M2=0;
        }
        
        
       // //check switch third status 
        //off condition
        if(copy_parentalLockBuffer[3] == CHAR_OFF && INPUTSWITCH3 == OFF && M3 == OFF)
        {
            if(man == 1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '3';__delay_ms(1);
            OUTPUT_RELAY3=OFF;
            }
            man=0;
            M3=1;
          
        }
        //on condtion
        if(copy_parentalLockBuffer[3] == CHAR_OFF && INPUTSWITCH3 == ON && M3 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '3';__delay_ms(1);
            OUTPUT_RELAY3=ON;
            }
            man=0;
            M3=0;
            
        }
        
     
       // //check switch fourth status 
        //off condition
        if(copy_parentalLockBuffer[4] == CHAR_OFF && INPUTSWITCH4 == OFF && M4 == OFF)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '4';__delay_ms(1);
            OUTPUT_RELAY4=OFF;
            }
            man=0;
            M4=1;
            
        }
        //on condtion
        if(copy_parentalLockBuffer[4] == CHAR_OFF && INPUTSWITCH4 == ON && M4 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '4';__delay_ms(1);
            OUTPUT_RELAY4=ON;
            }
            man=0;
            M4=0;
           
        }
               //off condition
        if(copy_parentalLockBuffer[5] == CHAR_OFF && INPUTSWITCH5 == OFF && M5 == OFF)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '5';__delay_ms(1);
            OUTPUT_RELAY5=OFF;
            }
            man=0;
            M5=1;
            
        }
        //on condtion
        if(copy_parentalLockBuffer[5] == CHAR_OFF && INPUTSWITCH5 == ON && M5 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '5';__delay_ms(1);
            OUTPUT_RELAY5=ON;
            }
            man=0;
            M5=0;
           
        }
       
               //off condition
        if(copy_parentalLockBuffer[6] == CHAR_OFF && INPUTSWITCH6 == OFF && M6 == OFF)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '6';__delay_ms(1);
            OUTPUT_RELAY6=OFF;
            }
            man=0;
            M6=1;
            
        }
        //on condtion
        if(copy_parentalLockBuffer[6] == CHAR_OFF && INPUTSWITCH6 == ON && M6 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '6';__delay_ms(1);
            OUTPUT_RELAY6=ON;
            }
            man=0;
            M6=0;
           
        }
       
#ifdef SWITCH_7_RELAY   
               //off condition
        if(copy_parentalLockBuffer[7] == CHAR_OFF && INPUTSWITCH7 == OFF && M7 == OFF)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '7';__delay_ms(1);
            OUTPUT_RELAY7=OFF;
            }
            man=0;
            M7=1;
            
        }
        //on condtion
        if(copy_parentalLockBuffer[7] == CHAR_OFF && INPUTSWITCH7 == ON && M7 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '7';__delay_ms(1);
            OUTPUT_RELAY7=ON;
            }
            man=0;
            M7=0;
           
        }
#endif  
       
#ifdef SWITCH_8_RELAY   
       //off condition
        if(copy_parentalLockBuffer[8] == CHAR_OFF && INPUTSWITCH8 == OFF && M8 == OFF)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '8';__delay_ms(1);
            OUTPUT_RELAY8=OFF;
            }
            man=0;
            M8=1;
            
        }
        //on condtion
        if(copy_parentalLockBuffer[8] == CHAR_OFF && INPUTSWITCH8 == ON && M8 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '8';__delay_ms(1);
            OUTPUT_RELAY8=ON;
            }
            man=0;
            M8=0;
           
        }
#endif   
       #ifdef SWITCH_9_RELAY   
       //off condition
        if(copy_parentalLockBuffer[9] == CHAR_OFF && INPUTSWITCH9 == OFF && M9 == OFF)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '9';__delay_ms(1);
            OUTPUT_RELAY9=OFF;
            }
            man=0;
            M9=1;
            
        }
        //on condtion
        if(copy_parentalLockBuffer[9] == CHAR_OFF && INPUTSWITCH9 == ON && M9 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '9';__delay_ms(1);
            OUTPUT_RELAY9=ON;
            }
            man=0;
            M9=0;
           
        }
#endif 
       #ifdef SWITCH_10_RELAY   
       //off condition
        if(copy_parentalLockBuffer[10] == CHAR_OFF && INPUTSWITCH10 == OFF && M10 == OFF)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            OUTPUT_RELAY10=OFF;
            }
            man=0;
            M10=1;
            
        }
        //on condtion
        if(copy_parentalLockBuffer[10] == CHAR_OFF && INPUTSWITCH10 == ON && M10 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            OUTPUT_RELAY10=ON;
            }
            man=0;
            M10=0;
           
        }
#endif 
              #ifdef SWITCH_11_RELAY   
       //off condition
        if(copy_parentalLockBuffer[11] == CHAR_OFF && INPUTSWITCH11 == OFF && M11 == OFF)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            OUTPUT_RELAY11=OFF;
            }
            man=0;
            M11=1;
            
        }
        //on condtion
        if(copy_parentalLockBuffer[11] == CHAR_OFF && INPUTSWITCH11 == ON && M11 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            OUTPUT_RELAY11=ON;
            }
            man=0;
            M11=0;
           
        }
#endif 
    }    
}

void applianceControl(char charSwitchMSB, char charSwitchLSB, char charSwitchSTATE, char chDimmerSpeedMSB, char chDimmerSpeedLSB,
        char charParentalControl, char charFinalFrameState){
    
    //define used variables and initilize it with zero
    int integerSwitchNumber = 0;
    int integerSwitchState = 0;
    int integerSpeed = 0;
    int currentStateBufferPositions=0;
    // Get switch Number in Integer format 
    //define all used character data types and initlize it with "#"
    char switchNumberStringBuffer[2]="#";
    char dimmerSpeedStringBuffer[2]="#";
    
    switchNumberStringBuffer[0]=charSwitchMSB;
    switchNumberStringBuffer[1]=charSwitchLSB;    
    integerSwitchNumber = atoi(switchNumberStringBuffer);//convert string into integer
    
    // Get switch State in Integer Format
    
    integerSwitchState = charSwitchSTATE-'0';
    
    // Get speed of Fan or level of dimmer    
    dimmerSpeedStringBuffer[0]=chDimmerSpeedMSB;
    dimmerSpeedStringBuffer[1]=chDimmerSpeedLSB;    
    integerSpeed = atoi(dimmerSpeedStringBuffer);
    
    // save Parental lock state of each switch into parental lock buffer
//    int integerParentalControl=charParentalControl-'0';
    parentalLockBuffer[integerSwitchNumber] = charParentalControl;
    copy_parentalLockBuffer[integerSwitchNumber]=parentalLockBuffer[integerSwitchNumber];
    // ACKNOWLEDGMENT data Format :->> (Gateway+SwitchState+SwitchMSB+SwitchLSB)
    
    currentStateBufferPositions = ((1+4*(integerSwitchNumber))-5);
    currentStateBuffer[currentStateBufferPositions++] = 'G';
    currentStateBuffer[currentStateBufferPositions++] = charSwitchSTATE;
    currentStateBuffer[currentStateBufferPositions++] = charSwitchMSB;
    currentStateBuffer[currentStateBufferPositions] = charSwitchLSB;    
    
    currentStateBufferPositions-=3;     // since we have come forward by 3 address in current state buffer
    if(charFinalFrameState=='1')    // until 
    {
        sendAcknowledgment(currentStateBuffer+currentStateBufferPositions);    
    }
    
    switch(integerSwitchNumber){
        case 1:
        {
            OUTPUT_RELAY1 = integerSwitchState;
        }break;
            
        case 2:
            {
            OUTPUT_RELAY2 = integerSwitchState;

            } break;
        case 3:
        {
            OUTPUT_RELAY3 = integerSwitchState;

        } break;
          
        case 4:
        {
            OUTPUT_RELAY4 = integerSwitchState;
        }break;
        case 5:
        {
            OUTPUT_RELAY5 = integerSwitchState;
        }break;  
        case 6:
        {
            OUTPUT_RELAY6 = integerSwitchState;
        }break;
#ifdef SWITCH_7_RELAY
        case 7:
        {
            OUTPUT_RELAY7 = integerSwitchState;
        }break; 
#endif     
#ifdef SWITCH_8_RELAY        
        case 8:
        {
            OUTPUT_RELAY8 = integerSwitchState;
        }break; 
#endif      
        #ifdef SWITCH_9_RELAY        
        case 9:
        {
            OUTPUT_RELAY9 = integerSwitchState;
        }break; 
#endif  
        #ifdef SWITCH_10_RELAY        
        case 10:
        {
            OUTPUT_RELAY10 = integerSwitchState;
        }break; 
#endif  
        #ifdef SWITCH_11_RELAY        
        case 11:
        {
            OUTPUT_RELAY11 = integerSwitchState;
        }break; 
#endif 
            default:
            break;
        }
    
}


/*
 * All input output pin initialization
 */
void GPIO_pin_Initialize(){
    clearAllPorts();
    pinINIT_extra();
    INPUT_SWITCH_DIR_1 = 1;
    INPUT_SWITCH_DIR_2 = 1;
    INPUT_SWITCH_DIR_3 = 1;
    INPUT_SWITCH_DIR_4 = 1;
    INPUT_SWITCH_DIR_5 = 1;
    INPUT_SWITCH_DIR_6 = 1;
    INPUT_SWITCH_DIR_7 = 1;
    INPUT_SWITCH_DIR_8 = 1;
    INPUT_SWITCH_DIR_9=1;
    INPUT_SWITCH_DIR_10=1;
    INPUT_SWITCH_DIR_11=1;
   
    
    OUTPUT_RELAY_DIR_1 = 0;
    OUTPUT_RELAY_DIR_2 = 0;
    OUTPUT_RELAY_DIR_3 = 0;
    OUTPUT_RELAY_DIR_4 = 0;
    OUTPUT_RELAY_DIR_5 = 0;
    OUTPUT_RELAY_DIR_6 = 0;
    OUTPUT_RELAY_DIR_7 = 0;
    OUTPUT_RELAY_DIR_8 = 0;
    OUTPUT_RELAY_DIR_9=0;
    OUTPUT_RELAY_DIR_10=0;
    OUTPUT_RELAY_DIR_11=0;
   
    
    // peripherals directions
    
    // USART DIRECTIONS
    USART_1_TRANSMIT_OUTPUT_DIR = 0;
    USART_1_RECIEVE_INPUT_DIR = 1;
    
    clearAllPorts();
}

/*
 * ALL Peripheral Initialization
 */
void allPeripheralInit(){
    EUSART_Initialize();

}

/*
 * USART Control Registers initialization
 */
void EUSART_Initialize(){
    PIE1bits.RC1IE = 0;
    PIE1bits.TX1IE = 0;

    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE enabled; ABDEN disabled;
    BAUD1CON = 0x0A;

    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled;
    RC1STA = 0x90;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave;
    TX1STA = 0x24;

    // Baud Rate = 9600; SP1BRGL 12;
    //SPBRGL = 0x0C;
    //SPBRGL = 0x19;                  // SP1BRGL is 25 (hex value=0x19) for 9600 baud on 16 MHz crystal frequency
    SP1BRGL = 0xA0;                  // SYNC =0 ; BRGH = 1 ; BRG16=1;
    // Baud Rate = 9600; SP1BRGH 1;
    SP1BRGH = 0x01;

    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;

    // enable receive interrupt
    PIE1bits.RC1IE = 1;                    // handled into INTERRUPT_Initialize()

    // Transmit Enabled
    TX1STAbits.TXEN = 1;

    // Serial Port Enabled
    RC1STAbits.SPEN = 1;
}


void peripheralsEnable(){
    // Transmit Enabled
    TX1STAbits.TXEN = 1;

    // Serial Port Enabled
    RC1STAbits.SPEN = 1;
}
void AllInterruptEnable(){
    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;
    
    // enable receive interrupt
    PIE1bits.RC1IE = 1;                    // handled into INTERRUPT_Initialize()

}

void errorsISR(char* errNum){
    int Tx_count=0;
  	while(Tx_count!=4)
 	{ 
        while (!TX1STAbits.TRMT);
 		TX1REG = *errNum;
 		*errNum++;
        Tx_count++;
 	}
}
void errorsMain(char* errNum){
   int Tx_count=0;
  	while(Tx_count!=4)
 	{ 
        while (!TX1STAbits.TRMT);
 		TX1REG = *errNum;
 		*errNum++;
        Tx_count++;
 	}
}
void sendAcknowledgment(char* currentStateBuffer){
  int Tx_count=0;
  	while(Tx_count!=4)
 	{ 
        while (!TX1STAbits.TRMT);
//        TX1REG='S';
 		TX1REG = *currentStateBuffer;
 		*currentStateBuffer++;
        Tx_count++;
 	}
}

void copyReceivedDataBuffer(){
    int dataBufferCounter=2;
    for(dataBufferCounter=2;dataBufferCounter<9;dataBufferCounter++){
        tempReceivedDataBuffer[dataBufferCounter-2]=mainReceivedDataBuffer[dataBufferCounter]; // copy data buffer from main
        mainReceivedDataBuffer[dataBufferCounter]='#';  // clean data buffer
    }
}
/*
 * AANALOG and PULL up REGISTERS related initialization
 */
void pinINIT_extra(){
    ANSELG=0x00;    WPUG = 0;
    
    ANSELF=0x00;
    
    ANSELE=0x00;    WPUE=0x00;
    
    ANSELD=0x00;    WPUD=0x00;
    
    ANSELB=0x00;    WPUB=0x00;
    
    ANSELA=0x00;     
} 

/*
 * always clear all the ports before initialization
 */
void clearAllPorts(){
    OUTPUT_RELAY1=0;
    OUTPUT_RELAY2=0;
    OUTPUT_RELAY3=0;
    OUTPUT_RELAY4=0;
    OUTPUT_RELAY5=0;
    OUTPUT_RELAY6=0;
    OUTPUT_RELAY7=0;
    OUTPUT_RELAY8=0;
    OUTPUT_RELAY9=0;
    OUTPUT_RELAY10=0;
    OUTPUT_RELAY11=0;
}
