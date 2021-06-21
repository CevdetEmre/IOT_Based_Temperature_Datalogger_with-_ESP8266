//CEVDET EMRE YILMAZ
//160403003
//Temparature_Data_Logger
//Submission date 10.06.2021

//------------------------------------------------------------------------------
//NOTE:    This project aims to transfer the temperature values measured
//         in the circuit established in the proteus simulation environment
//         to the Wi-Fi module ESP8266 via uart and store them on the server.
//------------------------------------------------------------------------------

//--------------------------------------------------------------(For LCD Screen)
//Here, pin assignment has been done.
sbit LCD_RS at RB0_bit;                                                         //LCD_RS pin is assigned to B0
sbit LCD_EN at RB1_bit;                                                         //LCD_EN pin is assigned to B1
sbit LCD_D4 at RB2_bit;                                                         //LCD_D4 pin is assigned to B2
sbit LCD_D5 at RB3_bit;                                                         //LCD_D5 pin is assigned to B3
sbit LCD_D6 at RB4_bit;                                                         //LCD_D6 pin is assigned to B4
sbit LCD_D7 at RB5_bit;                                                         //LCD_D7 pin is assigned to B5

//---------------------------------------------------------------(Pin Direction)
//Here, the paths to be used by the necessary pins for the LCD screen are assigned.
sbit LCD_RS_Direction at TRISB0_bit;                                            //LCD_RS register has selected B0 line
sbit LCD_EN_Direction at TRISB1_bit;                                            //LCD_EN register has selected B1 line
sbit LCD_D4_Direction at TRISB2_bit;                                            //LCD_D4 register has selected B2 line
sbit LCD_D5_Direction at TRISB3_bit;                                            //LCD_D5 register has selected B3 line
sbit LCD_D6_Direction at TRISB4_bit;                                            //LCD_D6 register has selected B4 line
sbit LCD_D7_Direction at TRISB5_bit;                                            //LCD_D7 register has selected B5 line

//---------------------------------------------------------------------(Defines)
//Variables to be used in functions are defined globally
unsigned char value[50],pos;                                                    //"Value" and "pos" variables are defined as unsigned char
char x,i ;                                                                      //"x" and "i" variables defined as char
unsigned char sec;                                                              //sec variable defined as unsigned char

//--------------------------------------------------------------(UART Interrupt)
//NOTE:   UART (Universal Asynchronous Receiver Transmitter) is a
//        communication protocol that provides communication between computer
//        and microcontrollers or microcontrollers and peripherals.

//NOTE2:  The reason for using uart interrupt is that the data is
//        transferred to the ESP8266 and stored on the server.

//Here, uart interrupt is used

//RCIF: EUSART Receive Interrupt Flag bit
//      1 = The EUSART receive buffer, RCREG, is full (cleared when RCREG is read)
//      0 = The EUSART receive buffer is empty
//PIR1.RCIF==1 indicate that buffer receiver is full (only one byte).
//RCREG(Receive Register) : Stores all the values received by the value.
void interrupt (){                                                              //Interrupt function is created
     if(PIR1.RCIF==1 && pos<28){                                                //It enters the interrupt when PIR1.RCIF==1 and pos<28
          value[pos]=RCREG;                                                     //Each value of value read is written to RCREG.
          pos++;                                                                //Pos variable increases by 1
          PIR1.RCIF=0;                                                          //The EUSART receive buffer is empty
     }
}

//------------------------------------------------------------(Cleaning process)
//    The purpose of this function is to delete 30
//    characters (ie space ) of the 50-character value array.
void clearVAL(){                                                                //clearVal function created
     char i;                                                                    //"i" variable defined as char
     for(i=0;i<30;i++)                                                          //For loop
         value[i]=' ';                                                          //It deletes 29 value values (that is, it assigns space), which corresponds to the value of decimal:32.
}
//---------------------------------------------------------------(Main function)
void main(){                                                                    //It is the main function of the project
//---------------------------------------------------------------------(Defines)
//Variables to be used in functions are defined local
     char flag;                                                                 //"flag" variable defined as char (local)
     unsigned int adc_val;                                                      //"adc_val" variable defined as unsigned integer (local)
     
//-----------------------------------------------------------------(Assignments)
//Analog, digital, input and output assignments
     ANSELB=0X00;                                                               //PORTB adjusted as analog
     ANSELC=0X00;                                                               //PORTC adjusted as analog
     ANSELA=0XFF;                                                               //PORTA adjusted as digital
     TRISB=0x00;                                                                //PORTB is output
     delay_ms(2000);                                                            //2s delay
     
//--------------------------------------------------------------(Interrupt bits)
//Here interrupt bits are set
     INTCON.GIE=1;                                                              //General input register is enable
     INTCON.RCIE=1;                                                             //Usart receýve interrupt enable bit
     INTCON.PEIE=1;                                                             //Peripheral interrupt enable bit
     
//-----------------------------------------------------------(Other adjustments)
     UART1_Init(9600);                                                          //Initialize uart module at 9600 bps
     adc_init();                                                                //Starting adc(analog digital converter)
     lcd_init();                                                                //Starting lcd
     lcd_out(1,2, "IOT DATALOGGER");                                            //"Iot datalogger" is printed on the lcd screen
     delay_ms(2000);                                                            //2s delay
     
//---------------------------------------------------(First control(AT Command))
//Here the control of the uart connection takes place

//AT Command: It is used to understand whether the commands we
//            send with the serial port are delivered without any problems.
//            AT is written to the program and if there is no problem,
//            OK is sent to the modem in response.

//NOTE:       In the while loop, if the answer from the wi-fi
//            module is ok, "ok" is written on the LCD, and if there
//            is no response from the wi-fi module, "error" is written on the LCD.

     clearVAL();                                                                //Call clearVAL function
     lcd_out(2,2,"AT");                                                         //"at" is printed on the lcd screen
     UART1_WRITE_TEXT("AT\r\n");                                                //At command sent to esp8266 via uart.
     delay_ms(3000);                                                            //3s delay
     while(1){
         x=strstr(&value,"OK");                                                 //Find "ok" in value
         if(x<0){
             Lcd_Cmd(_LCD_CLEAR);                                               //Clear display
             lcd_out(2,1,"ERROR1");                                             //Writing "error1" in lcd
             break;                                                             //Break
         }
         else{
              Lcd_Cmd(_LCD_CLEAR);                                              //Clear display
              lcd_out(2,1, "OK1 ");                                             //Writing "error1" in lcd
              break;                                                            //Break
         }
     }
     delay_ms(2000);                                                            //2s delay
     
//------------------------------------------------(Second control(ATE0 Command))
//Here echo is turned off.

//ATE Command :The ATE command can be used to turn command echo
//             on (ATE1) or off (ATE0). If no parameter is supplied (i.e. ATE),
//             it is the same as ATE0.

//NOTE:       In the while loop, if the answer from the wi-fi
//            module is ok, "ok" is written on the LCD, and if there
//            is no response from the wi-fi module, "error" is written on the LCD.

     clearVAL();                                                                //Call clearVAL function
     Lcd_Cmd(_LCD_CLEAR);                                                       //Clear display
     lcd_out(2,1,"ATE0");                                                       //"ate0" is printed on the lcd screen
     UART1_WRITE_TEXT("ATE0\r\n");                                              //Ate0 command sent to esp8266 via uart.
     delay_ms(3000);                                                            //3s delay
     Lcd_Cmd(_LCD_CLEAR);                                                       //Clear display
     while(1){
         x=strstr(&value,"OK");                                                 //Find "ok" in value
         if(x<0){
             Lcd_Cmd(_LCD_CLEAR);                                               //Clear display
             lcd_out(2,1,"ERROR2");                                             //Writing "error2" in lcd
             break;                                                             //Break
         }
         else{
              Lcd_Cmd(_LCD_CLEAR);                                              //Clear display
              lcd_out(2,1, "OK2");                                              //Writing "ok2" in lcd
              break;                                                            //Break
         }
     }
     delay_ms(2000);                                                            //2s delay
     Lcd_Cmd(_LCD_CLEAR);                                                       //Clear display
     
//--------------------------------------------(Third control(AT+CWMODE Command))
//Here it is desired to put the module in STA and Access Point (AP) mode.


//AT+CWMODE Command :To put the module in STA and Access Point (AP) mode,
//                   we need to issue the AT+CWMODE=3 command. If we want to connect to the
//                   networks in the environment, we need to run it in STA mode.

//NOTE:       In the while loop, if the answer from the wi-fi
//            module is ok, "ok" is written on the LCD, and if there
//            is no response from the wi-fi module, "error" is written on the LCD.

     clearVAL();                                                                //Call clearVAL function
     lcd_out(2,1,"AT+CWMODE=3");                                                //"AT+CWMODE=3" is printed on the lcd screen
     UART1_WRITE_TEXT("AT+CWMODE=3\r\n");                                       //AT+CWMODE=3 command sent to esp8266 via uart.
     delay_ms(3000);                                                            //3s delay
     while(1){
         x=strstr(&value,"OK");                                                 //Find "ok" in value
         if(x<0){
             Lcd_Cmd(_LCD_CLEAR);                                               //Clear display
             lcd_out(2,1,"ERROR3");                                             //Writing "error3" in lcd
             break;                                                             //Break
         }
         else{
              Lcd_Cmd(_LCD_CLEAR);                                              //Clear display
              lcd_out(2,1, "OK3");                                              //Writing "ok3" in lcd
              break;                                                            //Break
         }
     }
     delay_ms(2000);                                                            //2s delay
     Lcd_Cmd(_LCD_CLEAR);                                                       //clear display
     
//------------------------------------------(Fourth control (AT+CIPMUX Command))
// Here is the code written for ESP8266 to work in multiple mode


//AT+CIPMUX=1 Command :Sent  "AT+CWMUX=1" command to the  ESP module to enable
//                     multiple connections by calling the function to send CMD()

//NOTE:       In the while loop, if the answer from the wi-fi
//            module is ok, "ok" is written on the LCD, and if there
//            is no response from the wi-fi module, "error" is written on the LCD.

     clearVAL();                                                                //Call clearVAL function
     lcd_out(2,1,"AT+CIPMUX=1");                                                //"AT+CIPMUX=1" is printed on the lcd screen
     UART1_WRITE_TEXT("AT+CIPMUX=1\r\n") ;                                      //AT+CIPMUX=1 command sent to esp8266 via uart.
     delay_ms(3000);                                                            //3s delay
     while(1){
         x=strstr(&value,"OK");                                                 //Find "ok" in value
         if(x<0){
             Lcd_Cmd(_LCD_CLEAR);                                               //Clear display
             lcd_out(2,1,"ERROR4");                                             //Writing "error4" in lcd
             break;
         }
         else{
             Lcd_Cmd(_LCD_CLEAR);                                               //Clear display
             lcd_out(2,1, "OK4 ");                                              //Writing "ok4" in lcd
             break;
         }
     }
     delay_ms(2000);                                                            //2s delay
     Lcd_Cmd(_LCD_CLEAR);                                                       //Clear display
     
//--------------------------------------------(Fifth control (AT+CWJAP Command))
// Here is the code for the ESP8266 to connect to the internet

// AT+CWJAP=\"Nokia 8\",\"12345678\" Command :It is the command required for 
//                                           the ESP8266 to connect to the internet.
//                                           We can connect the esp8266 to the
//                                           internet by typing the wi-fi name
//                                           and password into the command.

//NOTE:       In the while loop, if the answer from the wi-fi
//            module is ok, "ok" is written on the LCD, and if there
//            is no response from the wi-fi module, "error" is written on the LCD.

     clearVAL();                                                                //Call clearVAL function
     lcd_out(2,1,"CONNECTING...");                                              //"CONNECTING..." is printed on the lcd screen
     
     //-------------------------------------------------------------------------
     //\0 must be put to compare 2 data. If it is not included, it can never be compared.
     value[0]='\0';
     
     //-------------------------------------------------------------------------
     strcat(value,"AT+CWJAP=\"Nokia 8\",\"12345678\"");                         //wi-fi connection
     UART1_WRITE_TEXT(&value) ;                                                 //sending value variable to ESP8266
     
     //-------------------------------------------------------------------------
     UART1_WRITE(0X0D);                                                         //to open the way to uart
     UART1_WRITE(0X0A);                                                         //to transmit data
     
     //-------------------------------------------------------------------------
     delay_ms(5000);                                                            //5s delay
     while(1){
         x=strstr(&value,"OK");                                                 //Find "ok" in value
         if(x<0){
             Lcd_Cmd(_LCD_CLEAR);                                               //Clear display
             lcd_out(2,1,"ERROR5");                                             //Writing "error5" in lcd
             break;
         }
         else{
             Lcd_Cmd(_LCD_CLEAR);                                               //Clear display
             lcd_out(2,1, "OK5 ");                                              //Writing "ok5" in lcd
             break;
         }
     }
     delay_ms(2000);                                                            //2s delay
     sec=0;                                                                     //sec value reset
     
//------------------------------------------------------------------------------
// Here, the IP address of ThingSpeak server is sent to ESP8266 using adc 
// conversion and AT+CIPSTART=4,\"TCP\",\"184.106.153.149\",80 command.

//I assigned 0.488 times the value that adc reads to adc_val.

//AT+CIPSTART=4,\"TCP\",\"184.106.153.149\",80 Command :ThingSpeak's IP address
//                                                     has been sent to the ESP8266.

//NOTE:       In the while loop, if the answer from the wi-fi
//            module is ok, "ok" is written on the LCD, and if there
//            is no response from the wi-fi module, "error" is written on the LCD.
     while(1){
         char text[8];                                                          //define 8-element char array
         adc_val=adc_read(0)*0.488;                                             //adcval=adcread*0.488
         Lcd_Cmd(_LCD_CLEAR);                                                   //Clear display
         inttostr(adc_val, text);                                               //converts adcvalue value to string and saves the text variable
         lcd_out(1,2,text);                                                     //Writing "text" in lcd
         delay_ms(2000);                                                        //2s delay
         lcd_out(2,1,"UPLOADING...");                                           //Writing "UPLOADING..." in lcd
         clearVAL();                                                            //Call clearVAL function
         
//------------------------------------------------------------------------------
     //\0 must be put to compare 2 data. If it is not included, it can never be compared
         value[0]='\0';
         
//------------------------------------------------------------------------------
         strcat(value,"AT+CIPSTART=4,\"TCP\",\"184.106.153.149\",80");          //ThingSpeak IP adresses
         UART1_WRITE_TEXT(&value) ;                                             //sending value variable to ESP8266
         
     //-------------------------------------------------------------------------
         UART1_WRITE(0X0D);                                                     //to open the way to uart
         UART1_WRITE(0X0A);                                                     //to transmit data

     //-------------------------------------------------------------------------
         sec=0;                                                                 //sec value reset
         delay_ms(2000);                                                        //2s delay
         while(1){
             x=strstr(&value,"OK");                                             //Find "ok" in value
             if(x<0){
                 lcd_out(2,1,"ERROR6");                                         //Writing "error6" in lcd
                 flag=0;                                                        //reset flag
                 break;
             }
             else{
                 lcd_out(2,1,"OK6 ");                                           //Writing "ok6" in lcd
                 flag=1;                                                        //flag is set
                 break;
             }
         }
         delay_ms(4000);                                                        //4s delay
         
//------------------------------------------------------------------------------
//Here I am checking the ESP8266 for the 2nd time. If the response of the
//command sent is ">", it continues, if not, it responds as "error7"

// AT+CIPSEND=4,46 Command :The AT+CHIPSEND instruction is used to send a value 
//                          to the ESP826. Each value has an ascii code.

//NOTE:       In the while loop, if the answer from the wi-fi
//            module is ok, "ok" is written on the LCD, and if there
//            is no response from the wi-fi module, "error" is written on the LCD.
         if(flag==1){
             clearVAL();                                                        //Call clearVAL function
//------------------------------------------------------------------------------
//\0 must be put to compare 2 data. If it is not included, it can never be compared
             value[0]='\0';

//------------------------------------------------------------------------------
             strcat(value,"AT+CIPSEND=4,46");                                   //Controlling 2
             UART1_WRITE_TEXT(&value) ;                                         //sending value variable to ESP8266
//------------------------------------------------------------------------------
             UART1_WRITE(0X0D);                                                 //to open the way to uart
             UART1_WRITE(0X0A);                                                 //to transmit data

//------------------------------------------------------------------------------
             delay_ms(3000);                                                    //3s delay
             while(1){
                 x=strstr(&value,">");                                          //checking the ">" mark
                 if(x<0){
                     lcd_out(2,1,"ERROR7");                                     //Writing "error7" in lcd
                     flag=0;                                                    //flag is reset
                     break;
                 }
                 else{
                     lcd_out(2,1,"OK7 ");                                       //Writing "ok7" in lcd
                     flag=1;                                                    //flag is set
                     break;
                 }
             }
             delay_ms(4000);                                                    //4s delay
//------------------------------------------------------------------------------
 //Here, sending the data to the server is completed. The UART WRITE function
 // was called more than once because bit by bit sending was made.

 //GET /update?key=0CKMY5IHRTQOWKQX&field1= Command :This command is the write-key
 //                                                  required for me to write the data 
 //                                                 allocated to my channel on the server.


//NOTE:       In the while loop, if the answer from the wi-fi
//            module is ok, "ok" is written on the LCD, and if there
//            is no response from the wi-fi module, "error" is written on the LCD.
             if(flag==1){
                 clearVAL();                                                    //Call clearVAL function
//------------------------------------------------------------------------------
//\0 must be put to compare 2 data. If it is not included, it can never be compared
                 value[0]='\0';

//------------------------------------------------------------------------------
                 lcd_out(2,1,"GET DATA");                                       //Writing "GET DATA" in lcd
                 strcat(value,"GET /update?key=0CKMY5IHRTQOWKQX&field1=");      //ThingSpeak write-key
                 UART1_WRITE_TEXT(&value) ;                                     //sending value variable to ESP8266
//-------------------------------------------------------------
//Since the data cannot go all at once, the following procedure
//is applied. The transfer takes place bit by bit.
                 
                 UART1_WRITE((adc_val/1000)+48);                                //calculations for bit-bit
                 UART1_WRITE(((adc_val%1000)/100)+48);                          //calculations for bit-bit
                 UART1_WRITE(((adc_val%100)/10)+48);                            //calculations for bit-bit
                 UART1_WRITE((adc_val%10)+48);                                  //calculations for bit-bit
//------------------------------------------------------------------------------
                 UART1_WRITE(0X0D);                                             //to open the way to uart
                 UART1_WRITE(0X0A);                                             //to transmit data

//------------------------------------------------------------------------------
//CIPCLOSE Command :Required command for ESP8266 to shut down

                 delay_ms(4000);                                                //4s delay
                 lcd_out(2,1,"CIPCLOSE");                                       //Writing "CIPCLOSE" in lcd
                 clearVAL();                                                    //Call clearVAL function
//------------------------------------------------------------------------------
//\0 must be put to compare 2 data. If it is not included, it can never be compared
                 value[0]='\0';

//------------------------------------------------------------------------------
                 delay_ms(3000);                                                //3s delay
                 strcat(value,"AT+CIPCLOSE");                                   //ESP8266 close
                 UART1_WRITE_TEXT(&value) ;                                     //sending value variable to ESP8266
//------------------------------------------------------------------------------
                 UART1_WRITE(0X0D);                                             //to open the way to uart
                 UART1_WRITE(0X0A);                                             //to transmit data

//------------------------------------------------------------------------------
                 delay_ms(1000);                                                //1s delay
             }
         }
         delay_ms(5000);                                                        //5s delay
     }
}