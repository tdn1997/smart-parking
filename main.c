#include <reg52.h>

// ======= DINH NGHIA CHAN LCD (4-BIT MODE) =======
sbit RS = P2 ^ 0; // Register Select
sbit EN = P2 ^ 1; // Enable
sbit D4 = P0 ^ 4; // Data bit 4
sbit D5 = P0 ^ 5; // Data bit 5
sbit D6 = P0 ^ 6; // Data bit 6
sbit D7 = P0 ^ 7; // Data bit 7

// ======= DINH NGHIA CHAN NUT NHAN =======
sbit BTN_UP = P1 ^ 6;   // Nut tang
sbit BTN_DOWN = P1 ^ 7; // Nut giam

sbit MOTOR_1 = P1 ^ 2; // vao
sbit MOTOR_2 = P1 ^ 3; // ra

// ======= DINH NGHIA CHAN 7-SEGMENT =======
#define SEG_DATA P3 // Du lieu 7-segment (A-G)
sbit SEG1 = P2 ^ 6; // COM1 - Hang chuc
sbit SEG2 = P2 ^ 7; // COM2 - Hang don vi

// ======= MA HIEN THI 7-SEGMENT =======
unsigned char code seg_code[] = {
    0xC0, // 0
    0xF9, // 1
    0xA4, // 2
    0xB0, // 3
    0x99, // 4
    0x92, // 5
    0x82, // 6
    0xF8, // 7
    0x80, // 8
    0x90  // 9
};

// ======= BIEN TOAN CUC =======
unsigned char temp = 30;

// ======= HAM TRE =======
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++)
            ;
}

// ======= HAM GUI 4-BIT CHO LCD =======
void LCD_Send4Bit(unsigned char cX)
{
    D7 = (cX >> 3) & 1;
    D6 = (cX >> 2) & 1;
    D5 = (cX >> 1) & 1;
    D4 = cX & 1;
}

// ======= HAM GUI XUNG ENABLE =======
void LCD_Enable()
{
    EN = 1;
    delay_ms(1);
    EN = 0;
    delay_ms(1);
}

// ======= HAM GUI LENH CHO LCD =======
void LCD_Command(unsigned char cmd)
{
    RS = 0;
    LCD_Send4Bit(cmd >> 4);
    LCD_Enable();
    LCD_Send4Bit(cmd);
    LCD_Enable();
    delay_ms(2);
}

// ======= HAM GUI DU LIEU CHO LCD =======
void LCD_Char(unsigned char chr)
{
    RS = 1;
    LCD_Send4Bit(chr >> 4);
    LCD_Enable();
    LCD_Send4Bit(chr);
    LCD_Enable();
    delay_ms(2);
}

// ======= HAM KHOI TAO LCD 4-BIT =======
void LCD_Init()
{
    delay_ms(50); // >40ms sau khi cấp nguồn
    RS = 0;
    EN = 0;

    // Khởi tạo mạnh cho 4-bit mode
    LCD_Send4Bit(0x03);
    LCD_Enable();
    delay_ms(5);

    LCD_Send4Bit(0x03);
    LCD_Enable();
    delay_ms(5);

    LCD_Send4Bit(0x03);
    LCD_Enable();
    delay_ms(5);

    LCD_Send4Bit(0x02); // Chuyển sang 4-bit
    LCD_Enable();
    delay_ms(5);

    // Các lệnh cấu hình
    LCD_Command(0x28); // 4-bit, 2 dòng, font 5x8
    LCD_Command(0x0C); // Display ON, Cursor OFF, Blink OFF
    LCD_Command(0x06); // Tăng con trỏ sang phải
    LCD_Command(0x01); // Clear màn hình
    delay_ms(10);
}

// ======= HAM DIA CHI CON TRO VT TRI =======
void LCD_Goto(unsigned char row, unsigned char col)
{
    unsigned char address;
    if (row == 0)
        address = 0x80 + col; // Clear
    else
        address = 0xC0 + col;
    LCD_Command(address);
}

// ======= HAM HIEN THI CHUOI =======
void LCD_String(char *str)
{
    while (*str)
    {
        LCD_Char(*str++);
    }
}

// ======= HAM HIEN THI SO =======
void LCD_Number(unsigned char num)
{
    LCD_Char((num / 10) + '0');
    LCD_Char((num % 10) + '0');
}

// ======= HAM HIEN THI 7-SEGMENT =======
void Display_7Seg()
{
    unsigned char chuc, donvi;
    chuc = temp / 10;
    donvi = temp % 10;

    // ANODE chung: SEG1 de bat, SEG2 de tat
    // Hien thi hang chuc
    SEG1 = 0;
    SEG2 = 0;
    P3 = seg_code[chuc];
    SEG1 = 1;
    delay_ms(2);

    // Hien thi hang don vi
    SEG1 = 0;
    SEG2 = 0;
    P3 = seg_code[donvi];
    SEG2 = 1;
    delay_ms(2);
}

// ======= HAM CAP NHAT LCD =======
void Update_LCD()
{
    // Tat ca bit cao de tranh xung dot
    SEG1 = 1;
    SEG2 = 1;
    P3 = 0xFF;
    delay_ms(5);

    // Dong 1: NHIET DO XX
    LCD_Goto(0, 3);
    LCD_String("BIEN SO: ");
}

void moCuaChoXeVao()
{

    MOTOR_1 = 1;   
    MOTOR_2 = 0;
}

void moCuaChoXeRa()
{

    MOTOR_2 = 1;
    MOTOR_1 = 0;
}

// ======= HAM XU LY NUT NHAN =======
void Process_Button()
{
    static unsigned char btn_pressed = 0;
    unsigned char i;

    // Nut tang (nhan = 0)
    if (BTN_UP == 0 && !btn_pressed)
    {
        delay_ms(30);
        if (BTN_UP == 0)
        {
            btn_pressed = 1;
            if (temp < 99)
            {
                temp++;
            }
            Update_LCD();

            // Hien thi 7-seg on dinh sau khi cap nhat
            for (i = 0; i < 100; i++)
            {
                Display_7Seg();
            }

            moCuaChoXeVao();
        }
    }

    // Nut giam (nhan = 0)
    if (BTN_DOWN == 0 && !btn_pressed)
    {
        delay_ms(30);
        if (BTN_DOWN == 0)
        {
            btn_pressed = 1;
            if (temp > 0)
            {
                temp--;
            }
            Update_LCD();

            // Hien thi 7-seg on dinh sau khi cap nhat
            for (i = 0; i < 100; i++)
            {
                Display_7Seg();
            }

            moCuaChoXeRa();
        }
    }

    // Reset trang thai
    if (BTN_UP == 1 && BTN_DOWN == 1)
    {
        btn_pressed = 0;
    }
}

// ======= HAM MAIN =======
void main()
{
    unsigned char i;

    // Cau hinh port
    P3 = 0xFF; // Set cao de on dinh
    P1 = 0xFF; // Port 1 input (pull-up)
    P2 = 0x00; // Port 2 output
    P3 = 0x00; // Port 3 output

    // Tat 7-segment ban dau
    SEG1 = 1;
    SEG2 = 1;

    MOTOR_1 = 0;
    MOTOR_2 = 0;
    // Khoi tao LCD
    delay_ms(200);
    LCD_Init();
    delay_ms(100);

    Update_LCD();

    // Hien thi 7-seg on dinh sau khi khoi tao
    for (i = 0; i < 100; i++)
    {
        Display_7Seg();
    }

    // Vong lap chinh
    while (1)
    {
        // Hien thi 7-segment lien tuc
        Display_7Seg();

        // Kiem tra nut nhan
        Process_Button();
    }
}