/**************JoyGuru***************
 *                                  *
 *     Author: Sajeeb Kumar Ray     *
 * Contact: sajeebbro2002@gmail.com *
 *                                  *
 ************************************/
//char arraCC[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
char arraCA[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
short int i = 0, k = 0, hold1 = 1, hold2 = 1, eject1 = 0;
unsigned short delay_var = 0, set = 1, cur_eject = 0, cur_hold2 = 0, hold2_ok = 0;
int total = 0, one = 0, two = 0, three = 0, four = 0, temp;

void EEPROM_Init();
void call_display(int number);
void total_display(int n);
void display(short int v1, short int v2, short int v3, short int v4);
void total_reset();
void set_click();
void checking();
void eject1_check();
void hold2_check();

void main() {
   TRISB = 0x00;
   TRISC.f0 = TRISC.f1 = TRISC.f2 = TRISC.f3 = 0xff;
   TRISC.f6 = TRISC.f7 = 0x00;
   TRISD.f0 = TRISD.f1 = TRISD.f2 = TRISD.f3 = 0xff;
   TRISD.f4 = TRISD.f5 = TRISD.f6 = TRISD.f7 = 0x00;
   TRISE = 0x00;
   portb = 0x00;
   portc.f6 = portc.f7 = 0x00;
   portd.f4 = portd.f5 = portd.f6 = portd.f7 = 0x00;
   portd = 0x00;
   porte = 0x00;

   EEPROM_Init();

   while(1)
   {
      porte.f1 = 0xff; // run led on
      total_reset();
      set_click();
      checking();
      if(hold2 == 0)
      {
          if(hold1 == 1 && hold2_ok == 1)
          {
                   hold2_ok = 0;
                   EEPROM_Write(0x07,hold2_ok);
                   portc.f6 = 0xff; //eject 2nd bag
                   porte.f2 = 0xff; // led on
                   for(i = 1; i <= 50; i++)
                   {
                       eject1_check();
                       call_display(total);  // this function call need 20 ms
                   }
                   portc.f6 = 0x00;
                   porte.f2 = 0x00;
                   total++; // 2nd system produce a bag
                   total_display(total); // this function inclue EEPROM to store total
          }
          else if(hold1 == 0 && hold2_ok == 1)
          {
              // delay. devide delay var by 10 when applying
              hold2_ok = 0;
              EEPROM_Write(0x07,hold2_ok);
              for(k = 1; k <= delay_var; k++)
              {
                  for(i = 1; i <= 5; i++)
                     call_display(total);  // this function call need 20 ms
                  eject1_check();
                  hold2_check();
              }

                   portc.f6 = 0xff; //eject 2nd bag
                   porte.f2 = 0xff; // led on
                   for(i = 1; i <= 50; i++)
                   {
                       eject1_check();
                       hold2_check();
                       call_display(total);  // this function call need 20 ms
                   }
                   portc.f6 = 0x00;
                   porte.f2 = 0x00;
                   total++; // 2nd system produce a bag
                   total_display(total); // this function inclue EEPROM to store total

          }

      }
      call_display(total);
   }
}
void checking()
{
    hold1 = portd.f0;
    hold2_check();
    eject1_check();
}
void hold2_check()
{
    hold2 = portd.f2;
    if(hold2 == 1)
    {
        if(cur_hold2 == 0)  // cur_hold2 = 0 means, current hold of 2nd system is not select
        {
              hold2_ok = 1;
              EEPROM_Write(0x07,hold2_ok);
              cur_hold2 = 1;
              EEPROM_Write(0x06,cur_hold2);
        }
    }
    if(hold2 == 0)
    {
        if(cur_hold2 == 1)
        {
             cur_hold2 = 0; // set for next detection
             EEPROM_Write(0x06,cur_hold2);
        }
    }
}
void eject1_check()
{
    eject1 = portd.f1;
    if(eject1 == 1)
    {
        if(cur_eject == 0)  // cur_eject = 0 means, current eject of first system not sel
        {
              total++;
              total_display(total); // this function inclue EEPROM to store total
              cur_eject = 1;
              EEPROM_Write(0x05,cur_eject);
        }
    }
    if(eject1 == 0)
    {
        if(cur_eject == 1)
        {
             cur_eject = 0; // set for next detection
             EEPROM_Write(0x05,cur_eject);
        }
    }
}
void total_reset()
{
    if(portc.f3 == 0xff)  // reset click initialize
     {
        delay_ms(1500);
        if(portc.f3 == 0xff)  // click stability check
        {
            total = 0;
            for(i = 3; i >= 0; i--)    // delay for preventing set
            {
                 for(k = 0; k < 25; k++)
                     call_display(i);
            }
            total_display(total);  // this function call need 20 ms
        }
     }
}
void set_click()
{
     while(set == 0)
     {
         portd.f6 = 0xff;   // active power for third digit
         portb = 0x7F; // provide data for third digit
         delay_ms(10);
         portd.f6 = 0x00;   // Deactive power for third digit
         porte.f0 = 0xff; //  set led on
         if(portc.f0 == 0xff)  // click initialize
         {
            delay_ms(80);
            if(portc.f0 == 0xff)  // click stability check
            {
                if(delay_var < 100)
                    delay_var++;        // digit increment
            }
         }
         if(portc.f1 == 0xff)  // click initialize
         {
            delay_ms(80);
            if(portc.f1 == 0xff)  // click stability check
            {
                if(delay_var > 0)
                    delay_var--;        // digit decrement
            }
         }
         if(portc.f2 == 0xff)  // set click initialize
         {
            delay_ms(1000);
            if(portc.f2 == 0xff)  // click stability check
            {
                set = 1;
                delay_var = delay_var + 10;
                EEPROM_Write(0x22,delay_var);
                for(i = 3; i >= 0; i--)    // delay for preventing reset
                {
                     for(k = 0; k < 25; k++)
                         call_display(i);
                }
                porte.f0 = 0x00;
            }
         }
         call_display(delay_var);
     }
     if(portc.f2 == 0xff && set == 1)  // reset click initialize
     {
        delay_ms(1500);
        if(portc.f2 == 0xff)  // click stability check
        {
            set = 0;
            for(i = 3; i >= 0; i--)    // delay for preventing set
            {
                 for(k = 0; k < 25; k++)
                     call_display(i);
            }
            if(delay_var >= 10)
            delay_var = delay_var - 10;
        }
     }
}
void EEPROM_Init()
{
    if(EEprom_Read(0x04) == 0xff) //initialization of hours in EEPROM
        EEPROM_Write(0x04,four);
    else
        four = EEPROM_read(0x04); // read previos values
    delay_ms(20);
    if(EEprom_Read(0x03) == 0xff) //initialization of hours in EEPROM
        EEPROM_Write(0x03,three);
    else
        three = EEPROM_read(0x03); // read previos values
    delay_ms(20);
    if(EEprom_Read(0x02) == 0xff) //initialization of hours in EEPROM
        EEPROM_Write(0x02,two);
    else
        two = EEPROM_read(0x02); // read previos values
    delay_ms(20);
    if(EEprom_Read(0x01) == 0xff) //initialization of hours in EEPROM
        EEPROM_Write(0x01,one);
    else
        one = EEPROM_read(0x01); // read previos values
    delay_ms(20);
    // retrieving total
    total = four + (10*three) + (100*two) + (1000*one);

    if(EEprom_Read(0x22) == 0xff) //initialization of minute in EEPROM
    {
        set = 0;
        EEPROM_Write(0x22,delay_var);
    }
    else
    {
        delay_var = EEPROM_read(0x22);
        if(delay_var >= 10)
            delay_var = delay_var - 10;
    }
    delay_ms(20);
    if(EEprom_Read(0x05) == 0xff) //initialization in EEPROM
        EEPROM_Write(0x05,cur_eject);
    else
        cur_eject = EEPROM_read(0x05); // read previos values
    delay_ms(20);
    if(EEprom_Read(0x06) == 0xff) //initialization in EEPROM
        EEPROM_Write(0x06,cur_hold2);
    else
        cur_hold2 = EEPROM_read(0x06); // read previos values
    delay_ms(20);
    if(EEprom_Read(0x07) == 0xff) //initialization in EEPROM
        EEPROM_Write(0x07,hold2_ok);
    else
        hold2_ok = EEPROM_read(0x07); // read previos values
    delay_ms(20);
}
void total_display(int n)
{
     temp = n;
     four = temp%10;
     EEPROM_Write(0x04,four);
     temp /= 10;
     three = temp%10;
     EEPROM_Write(0x03,three);
     temp /= 10;
     two = temp%10;
     EEPROM_Write(0x02,two);
     temp /= 10;
     one = temp;
     EEPROM_Write(0x01,one);
     display(one, two, three, four);
}
void call_display(int number)
{
     temp = number;
     four = temp%10;
     temp /= 10;
     three = temp%10;
     temp /= 10;
     two = temp%10;
     temp /= 10;
     one = temp;
     display(one, two, three, four);
}
void display(short int v1,short int v2,short int v3,short int v4)
{
     portd.f4 = 0xff;   // active power for digit left
     portb = arraCA[v1]; // provide data for left digit
     delay_ms(5);
     portd.f4 = 0x00;   // Deactive power for digit left

     portd.f5 = 0xff;     // active power for second digit
     portb = arraCA[v2];    // provide data for second digit
     delay_ms(5);
     portd.f5 = 0x00;      // deactive power for second digit

     portd.f6 = 0xff;   // active power for third digit
     portb = arraCA[v3]; // provide data for third digit
     delay_ms(5);
     portd.f6 = 0x00;   // Deactive power for third digit

     portd.f7 = 0xff;   // active power for digit right
     portb = arraCA[v4]; // provide data for right digit
     delay_ms(5);
     portd.f7 = 0x00;   // Deactive power for digit right
}