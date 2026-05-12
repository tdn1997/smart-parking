#include <REGX52.H>

// ================= FAN =================
#define FAN P2_7

// ================= FIRE/SMOKE =================
#define FIRE_SENSOR P2_6
#define LED_RED     P3_5
#define BUZZER      P3_6
#define MOTOR       P3_7

// ================= ADC =================
#define ADC_DATA P1
#define ADC_CS   P3_0
#define ADC_RD   P3_1
#define ADC_WR   P3_2
#define ADC_INTR P3_3

// ================= LED 7 SEG =================
#define LED_DIGIT1 P2_0
#define LED_DIGIT2 P2_1

unsigned char led7[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};

unsigned char nhietDo = 0;

// ================= DELAY =================
void delay_ms(int ms)
{
    int i,j;
    for(i=0;i<ms;i++)
        for(j=0;j<120;j++);
}

// ================= ADC =================
unsigned char ADC_Read()
{
    unsigned char adc_value;

    ADC_DATA = 0xFF;
    ADC_CS = 0;

    ADC_WR = 0;
    delay_ms(1);
    ADC_WR = 1;

    while(ADC_INTR == 1);

    ADC_RD = 0;
    adc_value = ADC_DATA;
    ADC_RD = 1;

    ADC_CS = 1;

    return adc_value;
}

void Update_NhietDo()
{
    unsigned int temp;

    temp = (unsigned int)ADC_Read() * 500 / 255;

    if(temp > 99)
        temp = 99;

    nhietDo = (unsigned char)temp;
}

// ================= DISPLAY 7 SEG =================
void display_digit(unsigned char digit, unsigned char num)
{
    LED_DIGIT1 = 0;
    LED_DIGIT2 = 0;

    P0 = led7[num];

    if(digit == 1)
        LED_DIGIT1 = 1;
    else
        LED_DIGIT2 = 1;

    delay_ms(5);

    LED_DIGIT1 = 0;
    LED_DIGIT2 = 0;
}

void display7seg(unsigned char num)
{
    unsigned char chuc, donvi;

    chuc = num/10;
    donvi = num%10;

    display_digit(1, chuc);
    display_digit(2, donvi);
}

// ================= FIRE/SMOKE =================
void Handle_FireSmoke()
{
    if(FIRE_SENSOR == 1)
    {
        LED_RED = 1;
        BUZZER = 1;
        MOTOR = 1;
    }
    else
    {
        LED_RED = 0;
        BUZZER = 0;
        MOTOR = 0;
    }
}

// ================= MAIN =================
void main()
{
    while(1)
    {
        Update_NhietDo();
        Handle_FireSmoke();

        if(nhietDo >= 35)
        {
            FAN = 1;
        }
        else
        {
            FAN = 0;
        }

        // ===== LED 7 do?n =====
        display7seg(nhietDo);

    }
}
