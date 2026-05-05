#include <reg52.h>

// ======= LCD1: LCD HIEN TAI (4-BIT MODE) =======
sbit RS = P2 ^ 6;
sbit EN = P2 ^ 7;
sbit D4 = P3 ^ 4;
sbit D5 = P3 ^ 5;
sbit D6 = P3 ^ 6;
sbit D7 = P3 ^ 7;

// ======= LCD2: HIEN THI CHO TRONG (4-BIT MODE) =======
sbit LCD2_RS = P2 ^ 4;
sbit LCD2_EN = P2 ^ 5;
sbit LCD2_D4 = P3 ^ 0;
sbit LCD2_D5 = P3 ^ 1;
sbit LCD2_D6 = P3 ^ 2;
sbit LCD2_D7 = P3 ^ 3;

// ======= BUTTON =======
sbit BTN_IN = P1 ^ 6;
sbit BTN_OUT = P1 ^ 7;

// ======= SERVO =======
sbit SERVO_PIN_IN = P1 ^ 4;
sbit SERVO_PIN_OUT = P1 ^ 5;

#define CHANNEL_IN  0
#define CHANNEL_OUT 1

#define SERVO_CLOSE_PULSE 800
#define SERVO_OPEN_PULSE  2200
#define SERVO_PERIOD_US   20000
#define SERVO_TICK_US     100
#define SERVO_HOLD_MS     10000

#define DEBOUNCE_MS       30
#define TIMER0_RELOAD     (65536 - SERVO_TICK_US)
#define LCD_1             0
#define LCD_2             1


// ======= BIEN TOAN CUC =======
unsigned char temp = 30;
volatile unsigned int system_ms = 0;
volatile unsigned int pwm_counter_us = 0;
volatile unsigned int servo_pulse_in = SERVO_CLOSE_PULSE;
volatile unsigned int servo_pulse_out = SERVO_CLOSE_PULSE;
volatile unsigned int in_timer = 0;
volatile unsigned int out_timer = 0;
bit in_active = 0;
bit out_active = 0;

// ======= HAM TRE =======
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++)
            ;
}

// ======= LCD =======
void Lcd_SetRS(unsigned char lcd, unsigned char value)
{
    if (lcd == LCD_1)
        RS = value;
    else
        LCD2_RS = value;
}

void Lcd_Send4Bit(unsigned char lcd, unsigned char cX)
{
    if (lcd == LCD_1)
    {
        D7 = (cX >> 3) & 1;
        D6 = (cX >> 2) & 1;
        D5 = (cX >> 1) & 1;
        D4 = cX & 1;
    }
    else
    {
        LCD2_D7 = (cX >> 3) & 1;
        LCD2_D6 = (cX >> 2) & 1;
        LCD2_D5 = (cX >> 1) & 1;
        LCD2_D4 = cX & 1;
    }
}

void Lcd_Enable(unsigned char lcd)
{
    if (lcd == LCD_1)
    {
        EN = 1;
        delay_ms(1);
        EN = 0;
    }
    else
    {
        LCD2_EN = 1;
        delay_ms(1);
        LCD2_EN = 0;
    }
    delay_ms(1);
}

void Lcd_Write(unsigned char lcd, unsigned char rs, unsigned char value)
{
    Lcd_SetRS(lcd, rs);
    Lcd_Send4Bit(lcd, value >> 4);
    Lcd_Enable(lcd);
    Lcd_Send4Bit(lcd, value);
    Lcd_Enable(lcd);
    delay_ms(2);
}

void Lcd_Command(unsigned char lcd, unsigned char cmd)
{
    Lcd_Write(lcd, 0, cmd);
}

void Lcd_Char(unsigned char lcd, unsigned char chr)
{
    Lcd_Write(lcd, 1, chr);
}

void Lcd_Init(unsigned char lcd)
{
    unsigned char i;

    delay_ms(50);
    Lcd_SetRS(lcd, 0);
    if (lcd == LCD_1)
        EN = 0;
    else
        LCD2_EN = 0;

    for (i = 0; i < 3; i++)
    {
        Lcd_Send4Bit(lcd, 0x03);
        Lcd_Enable(lcd);
        delay_ms(5);
    }

    Lcd_Send4Bit(lcd, 0x02);
    Lcd_Enable(lcd);
    delay_ms(5);

    Lcd_Command(lcd, 0x28);
    Lcd_Command(lcd, 0x0C);
    Lcd_Command(lcd, 0x06);
    Lcd_Command(lcd, 0x01);
    delay_ms(10);
}

void Lcd_Goto(unsigned char lcd, unsigned char row, unsigned char col)
{
    if (row == 0)
        Lcd_Command(lcd, 0x80 + col);
    else
        Lcd_Command(lcd, 0xC0 + col);
}

void Lcd_String(unsigned char lcd, char *str)
{
    while (*str)
        Lcd_Char(lcd, *str++);
}

void Lcd_Number(unsigned char lcd, unsigned char num)
{
    unsigned char tens = 0;

    while (num >= 10)
    {
        num -= 10;
        tens++;
    }

    Lcd_Char(lcd, tens + '0');
    Lcd_Char(lcd, num + '0');
}

void Update_LCD2_Slot()
{
    static unsigned char last_temp = 255;

    if (last_temp == temp)
        return;

    last_temp = temp;

    Lcd_Goto(LCD_2, 0, 0);
    Lcd_String(LCD_2, "CHO CON TRONG   ");

    Lcd_Goto(LCD_2, 1, 0);
    Lcd_Number(LCD_2, temp);
    Lcd_String(LCD_2, "              ");
}

// ======= MANG 50 BIEN SO XE NGAU NHIEN =======
unsigned char code license_plates[50][15] = {
    "92_D1_859.45",
    "66_H3_854.23",
    "95_X2_704.64",
    "53_A2_323.39",
    "83_Z1_674.35",
    "97_X7_325.67",
    "89_K1_877.30",
    "96_S6_384.29",
    "64_M2_194.58",
    "57_N6_718.43",
    "53_T9_227.58",
    "56_X5_949.90",
    "91_N4_821.18",
    "53_H5_181.39",
    "57_P5_564.91",
    "74_F6_463.36",
    "94_K2_723.91",
    "61_X4_267.69",
    "75_K9_324.97",
    "71_B4_941.14",
    "71_P5_167.37",
    "88_M4_771.73",
    "76_T3_371.27",
    "66_X9_369.84",
    "78_Y7_470.38",
    "59_V8_193.16",
    "58_E3_911.97",
    "78_Z2_494.58",
    "90_T9_357.80",
    "51_D9_868.44",
    "93_M2_400.65",
    "61_T1_839.43",
    "83_F9_208.90",
    "70_V4_256.57",
    "61_X9_100.86",
    "71_U1_214.56",
    "70_H1_346.82",
    "56_C8_935.18",
    "85_E3_775.70",
    "86_F5_640.87",
    "78_G9_873.98",
    "63_L7_787.93",
    "74_T9_562.25",
    "66_H2_446.12",
    "89_X4_702.38",
    "51_C1_334.18",
    "53_M2_626.40",
    "68_U4_652.26",
    "98_Y8_348.70",
    "77_G2_199.94"};

unsigned char current_index = 0;

void Update_LCD()
{
    Lcd_Command(LCD_1, 0x01);

    Lcd_Goto(LCD_1, 0, 0);
    Lcd_String(LCD_1, "BIEN SO: ");

    Lcd_Goto(LCD_1, 1, 0);
    Lcd_String(LCD_1, license_plates[current_index]);
    
    current_index++;
    if (current_index >= 50)
        current_index = 0;
}

// ======= SERVO =======
void Servo_SetPulse(unsigned char channel, unsigned int pulse)
{
    unsigned char ea_state;

    ea_state = EA;
    EA = 0;

    if (channel == CHANNEL_IN)
        servo_pulse_in = pulse;
    else
        servo_pulse_out = pulse;

    EA = ea_state;
}

void Servo_Open(unsigned char channel)
{
    unsigned char ea_state;

    Servo_SetPulse(channel, SERVO_OPEN_PULSE);

    ea_state = EA;
    EA = 0;
    if (channel == CHANNEL_IN)
    {
        in_timer = SERVO_HOLD_MS;
        in_active = 1;
    }
    else
    {
        out_timer = SERVO_HOLD_MS;
        out_active = 1;
    }
    EA = ea_state;
}

void Servo_Close(unsigned char channel)
{
    unsigned char ea_state;

    Servo_SetPulse(channel, SERVO_CLOSE_PULSE);

    ea_state = EA;
    EA = 0;
    if (channel == CHANNEL_IN)
        in_active = 0;
    else
        out_active = 0;
    EA = ea_state;
}

void Servo_Init(void)
{
    SERVO_PIN_IN = 0;
    SERVO_PIN_OUT = 0;

    TMOD = (TMOD & 0xF0) | 0x01;
    TH0 = TIMER0_RELOAD >> 8;
    TL0 = TIMER0_RELOAD & 0xFF;

    ET0 = 1;
    EA = 1;
    TR0 = 1;
}

void Timer0_ISR() interrupt 1
{
    static unsigned char ms_divider = 0;

    TH0 = TIMER0_RELOAD >> 8;
    TL0 = TIMER0_RELOAD & 0xFF;

    pwm_counter_us += SERVO_TICK_US;
    if (pwm_counter_us >= SERVO_PERIOD_US)
    {
        pwm_counter_us = 0;
        SERVO_PIN_IN = 1;
        SERVO_PIN_OUT = 1;
    }

    if (pwm_counter_us >= servo_pulse_in)
        SERVO_PIN_IN = 0;
    if (pwm_counter_us >= servo_pulse_out)
        SERVO_PIN_OUT = 0;

    ms_divider++;
    if (ms_divider >= 10)
    {
        ms_divider = 0;
        system_ms++;

        if (in_active && in_timer > 0)
            in_timer--;
        if (out_active && out_timer > 0)
            out_timer--;
    }
}

void Servo_Control()
{
    unsigned char close_in = 0;
    unsigned char close_out = 0;
    unsigned char ea_state;

    ea_state = EA;
    EA = 0;
    if (in_active && in_timer == 0)
        close_in = 1;
    if (out_active && out_timer == 0)
        close_out = 1;
    EA = ea_state;

    if (close_in)
        Servo_Close(CHANNEL_IN);

    if (close_out)
        Servo_Close(CHANNEL_OUT);
}

// ======= BUTTON =======
void Handle_Button_Event(unsigned char channel)
{
    unsigned char changed = 0;

    if (channel == CHANNEL_IN)
    {
        if (temp > 0)
        {
            temp--;
            changed = 1;
        }
    }
    else
    {
        if (temp < 99)
        {
            temp++;
            changed = 1;
        }
    }

    if (changed)
        Update_LCD2_Slot();

    Update_LCD();
    Servo_Open(channel);
}

void Handle_Button_Channel(unsigned char channel, unsigned char current_state)
{
    static unsigned char last_read[2] = {1, 1};
    static unsigned char stable_state[2] = {1, 1};
    static unsigned int last_change_ms[2] = {0, 0};
    unsigned int now;
    unsigned char ea_state;

    ea_state = EA;
    EA = 0;
    now = system_ms;
    EA = ea_state;

    if (current_state != last_read[channel])
    {
        last_read[channel] = current_state;
        last_change_ms[channel] = now;
    }

    if ((unsigned int)(now - last_change_ms[channel]) >= DEBOUNCE_MS)
    {
        if (current_state != stable_state[channel])
        {
            stable_state[channel] = current_state;
            if (stable_state[channel] == 0)
                Handle_Button_Event(channel);
        }
    }
}

void Handle_Button()
{
    Handle_Button_Channel(CHANNEL_IN, BTN_IN);
    Handle_Button_Channel(CHANNEL_OUT, BTN_OUT);
}

// ======= MAIN =======
void main()
{
    BTN_IN = 1;
    BTN_OUT = 1;
    P1 = 0xFF;
    P2 = 0x00;
    P3 = 0x00;

    Servo_Init();
    Servo_Close(CHANNEL_IN);
    Servo_Close(CHANNEL_OUT);

    delay_ms(200);
    Lcd_Init(LCD_1);
    Lcd_Init(LCD_2);

    Lcd_Goto(LCD_1, 0, 0);
    Lcd_String(LCD_1, "HE THONG NHUNG");
    Lcd_Goto(LCD_1, 1, 0);
    Lcd_String(LCD_1, "SMART PARKING");

    Update_LCD2_Slot();

    while (1)
    {
        Handle_Button();
        Servo_Control();
    }
}
