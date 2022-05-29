#include "lcd/lcd.h"
#include <string.h>
#include "utils.h"
#include "img.h"
#define TIME_INTERVAL(T) (SystemCoreClock / 4000.0 * (T))

typedef struct DINO_struct
{
    int status; // 0 for erect, 1 for jump and 2 for prone
    int id;     // id for obstacle, 0 for cactus1, 1 for cactus2 and 2 for pter.
    int score;
    int left_right; // 0 for left, 1 for right.
    int obs_left_pos, width;
    int jump_time;
    int is_jumping;
    double coeff;
    int root;
    struct CACTUS
    {
        unsigned char obs[480];
        int obs_size;
    } cactus;
    struct PTER
    {
        unsigned char obs_down[800];
        unsigned char obs_up[800];
        int up_down; // 0 for down and 1 for up.
        int obs_size;
    } pter;
} DINO;

typedef struct GROUND_struct
{
    int point1[2];
    int point2[2];
    int point3[2];
    int point4[2];
    int point5[2];
    int point6[2];
    int point7[2];
    int point8[2];
    int point9[2];
    int point10[2];
} GROUND;

/*
int point1[2]={1,72};
    int point2[2]={16,74};
    int point3[2]={33,72};
    int point4[2]={48,78};
    int point5[2]={64,76};
    int point6[2]={80,74};
    int point7[2]={94,77};
    int point8[2]={113,78};
    int point9[2]={130,79};
    int point10[2]={146,76};
*/

void Inp_init(void)
{
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
}

void Adc_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);
    RCU_CFG0 |= (0b10 << 14) | (1 << 28);
    rcu_periph_clock_enable(RCU_ADC0);
    ADC_CTL1(ADC0) |= ADC_CTL1_ADCON;
}

void IO_init(void)
{
    Inp_init(); // inport init
    Adc_init(); // A/D init
    Lcd_Init(); // LCD init
}

void dynamic_show_text(u16 x, u16 y, const char text[], u16 color)
{
    int len = strlen(text);
    for (int i = 0; i < len; ++i)
    {
        LCD_ShowChar(x + i * 8, y, text[i], 1, color);
        delay_1ms(50);
    }
}

int calc_quad(DINO *dino, double coeff, int root)
{
    return coeff * dino->jump_time * (root - dino->jump_time);
}

int calc1_quad(DINO *dino, double coeff, int root)
{
    return coeff * (dino->jump_time + 1) * (root - dino->jump_time - 1);
}

int Cactus_collide(DINO *dino)
{
    if (!dino->is_jumping)
    {
        if (dino->status == 0 && dino->obs_left_pos <= 49 && dino->obs_left_pos + dino->width - 1 >= 35)
            return 0;
        if (dino->status == 2 && dino->obs_left_pos <= 56 && dino->obs_left_pos + dino->width - 1 >= 35)
            return 0;
    }
    return 1;
}

int Pter_collide(DINO *dino)
{
    if (dino->status == 0 && dino->obs_left_pos <= 42 && dino->obs_left_pos + dino->width - 1 >= 38)
        return 0;
    if (dino->status == 1)
    {
        // int up_pos = 50 - calc_quad(dino, dino->coeff, dino->root);
        if (dino->obs_left_pos <= 49 && dino->obs_left_pos + dino->width - 1 >= 30)
            return 0;
    }
    return 1;
}

int Collision(DINO *dino)
{
    int collide = 1;
    if (dino->id == 0 || dino->id == 1)
        collide = Cactus_collide(dino);
    else if (dino->id == 2)
        collide = Pter_collide(dino);
    return collide;
}

void play(int numerator, int denomi, int root, int grd, int speed)
{
    DINO dino;
    dino.id = 3;
    dino.status = 0;
    dino.score = 0;
    dino.left_right = 0;
    dino.is_jumping = 0;
    dino.obs_left_pos = 0;
    dino.coeff = (double)numerator / denomi;
    dino.root = root;
    int64_t t1, t2, t3;
    t1 = get_timer_value(); // Score
    t2 = get_timer_value(); // Trex
    t3 = get_timer_value(); // Obstacle
    int pter_count = 0;
    int filled = 0;
    // handle unstoppable
    int flag_un = 0;
    // handle ground
    GROUND ground;
    ground.point1[0] = 1;
    ground.point1[1] = 74;
    ground.point2[0] = 3;
    ground.point2[1] = 74;
    ground.point3[0] = 33;
    ground.point3[1] = 76;
    ground.point4[0] = 48;
    ground.point4[1] = 78;
    ground.point5[0] = 50;
    ground.point5[1] = 76;
    ground.point6[0] = 55;
    ground.point6[1] = 75;
    ground.point7[0] = 94;
    ground.point7[1] = 77;
    ground.point8[0] = 113;
    ground.point8[1] = 78;
    ground.point9[0] = 130;
    ground.point9[1] = 79;
    ground.point10[0] = 146;
    ground.point10[1] = 76;
    int ground_count = 1;
    while (1)
    {
        // draw the ground
        if (ground_count == 1 || ground_count % grd == 0)
        {
            LCD_Fill(0, 74, 159, 79, BLACK);
            LCD_DrawPoint(ground.point1[0], ground.point1[1], WHITE);
            LCD_DrawPoint(ground.point2[0], ground.point1[1], WHITE);
            LCD_DrawPoint(ground.point3[0], ground.point1[1], WHITE);
            LCD_DrawPoint(ground.point4[0], ground.point1[1], WHITE);
            LCD_DrawPoint(ground.point5[0], ground.point1[1], WHITE);
            LCD_DrawPoint(ground.point6[0], ground.point1[1], WHITE);
            LCD_DrawPoint(ground.point7[0], ground.point1[1], WHITE);
            LCD_DrawPoint(ground.point8[0], ground.point1[1], WHITE);
            LCD_DrawPoint(ground.point9[0], ground.point1[1], WHITE);
            LCD_DrawPoint(ground.point10[0], ground.point1[1], WHITE);
            ground.point1[0] = ground.point1[0] <= 0 ? 159 : ground.point1[0] - 1;
            ground.point2[0] = ground.point2[0] <= 0 ? 159 : ground.point2[0] - 1;
            ground.point3[0] = ground.point3[0] <= 0 ? 159 : ground.point3[0] - 1;
            ground.point4[0] = ground.point4[0] <= 0 ? 159 : ground.point4[0] - 1;
            ground.point5[0] = ground.point5[0] <= 0 ? 159 : ground.point5[0] - 1;
            ground.point6[0] = ground.point6[0] <= 0 ? 159 : ground.point6[0] - 1;
            ground.point7[0] = ground.point7[0] <= 0 ? 159 : ground.point7[0] - 1;
            ground.point8[0] = ground.point8[0] <= 0 ? 159 : ground.point8[0] - 1;
            ground.point9[0] = ground.point9[0] <= 0 ? 159 : ground.point9[0] - 1;
            ground.point10[0] = ground.point10[0] <= 0 ? 159 : ground.point10[0] - 1;
        }
        ground_count++;
        // handle time
        if (get_timer_value() - t1 > TIME_INTERVAL(100))
        {
            t1 = get_timer_value();
            LCD_ShowNum(90, 10, dino.score, 7, 0xE0B7);
            dino.score++;
            if (dino.score % 100 == 0)
            {
                flag_un = dino.score;
            }
            if (flag_un != 0)
            { // enter unstoppable
                LCD_ShowString(0, 0, (u8 *)("Unstoppable"), RED);
            }
            if (dino.score == flag_un + 50)
            {
                flag_un = 0;
                LCD_Fill(0, 0, 88, 20, BLACK);
            }
        }

        if (Get_Button(1)) // Prone
        {
            if (!filled)
            {
                LCD_Fill(30, 50 - calc_quad(&dino, dino.coeff, dino.root), 49, 69 - calc_quad(&dino, dino.coeff, dino.root), BLACK);
                filled = 1;
            }
            dino.status = 2;
            dino.is_jumping = 0;
            dino.jump_time = 0;
        }
        else if (Get_Button(0)) // Jump
        {
            if (dino.is_jumping == 0)
                dino.jump_time = 0;
            dino.status = 1;
            dino.is_jumping = 1;
            filled = 0;
        }
        else if (dino.is_jumping == 1)
            dino.status = 1;
        else
            dino.status = 0;

        if (get_timer_value() - t2 > TIME_INTERVAL(30))
        {
            t2 = get_timer_value();
            if (dino.status == 0)
            {
                LCD_Fill(50, 50, 56, 69, BLACK);
                if (dino.left_right == 0)
                {
                    LCD_ShowPicture(30, 50, 49, 69, trex1, 800);
                    dino.left_right = 1;
                }
                else
                {
                    LCD_ShowPicture(30, 50, 49, 69, trex2, 800);
                    dino.left_right = 0;
                }
            }
            if (dino.status == 1)
            {
                int time = dino.jump_time;
                if (time >= 0 && time < dino.root / 2)
                {
                    LCD_ShowPicture(30, 50 - calc_quad(&dino, dino.coeff, dino.root), 49, 69 - calc_quad(&dino, dino.coeff, dino.root), trex3, 800);
                    LCD_Fill(30, 69 - calc1_quad(&dino, dino.coeff, dino.root), 49, 69 - calc_quad(&dino, dino.coeff, dino.root), BLACK);
                }
                else if (time >= dino.root / 2 && time < dino.root)
                {
                    LCD_ShowPicture(30, 50 - calc_quad(&dino, dino.coeff, dino.root), 49, 69 - calc_quad(&dino, dino.coeff, dino.root), trex3, 800);
                    LCD_Fill(30, 50 - calc_quad(&dino, dino.coeff, dino.root), 49, 50 - calc1_quad(&dino, dino.coeff, dino.root), BLACK);
                }
                dino.jump_time++;
                dino.jump_time %= dino.root;
                if (dino.jump_time == 0)
                    dino.is_jumping = 0;
            }
            if (dino.status == 2)
            {
                if (dino.left_right == 0)
                {
                    LCD_ShowPicture(30, 50, 56, 69, trex4, 1080);
                    dino.left_right = 1;
                }
                else
                {
                    LCD_ShowPicture(30, 50, 56, 69, trex5, 1080);
                    dino.left_right = 0;
                }
            }
        }
        if (get_timer_value() - t3 > TIME_INTERVAL(speed))
        {
            t3 = get_timer_value();
            if (dino.obs_left_pos <= 0)
            {
                LCD_Fill(0, 30, 29, 69, BLACK);
                dino.id = rand() % 3;
                if (dino.id == 0)
                {
                    dino.obs_left_pos = 148;
                    dino.width = 12;
                    memcpy(dino.cactus.obs, cactus1, sizeof(cactus1));
                    dino.cactus.obs_size = 480;
                }
                if (dino.id == 1)
                {
                    dino.obs_left_pos = 148;
                    dino.width = 12;
                    memcpy(dino.cactus.obs, cactus2, sizeof(cactus2));
                    dino.cactus.obs_size = 480;
                }
                if (dino.id == 2)
                {
                    dino.obs_left_pos = 140;
                    dino.width = 20;
                    memcpy(dino.pter.obs_down, pter1, sizeof(pter1));
                    memcpy(dino.pter.obs_up, pter2, sizeof(pter2));
                    dino.pter.up_down = 0;
                    dino.pter.obs_size = 800;
                    pter_count = 0;
                }
            }
            dino.obs_left_pos -= 2;
            if (dino.id == 0 || dino.id == 1)
            {
                LCD_Fill(dino.obs_left_pos + dino.width, 50, dino.obs_left_pos + dino.width + 1, 69, BLACK);
                LCD_ShowPicture(dino.obs_left_pos, 50, dino.obs_left_pos + dino.width - 1, 69, dino.cactus.obs, dino.cactus.obs_size);
            }
            else
            {
                pter_count++;
                int judge = (pter_count >> 2) & 0x1;
                LCD_Fill(dino.obs_left_pos + dino.width, 30, dino.obs_left_pos + dino.width + 1, 49, BLACK);
                if (dino.pter.up_down == 0 && judge == 0)
                {
                    LCD_ShowPicture(dino.obs_left_pos, 35, dino.obs_left_pos + dino.width - 1, 54, dino.pter.obs_up, dino.pter.obs_size);
                    dino.pter.up_down = 1;
                }
                else if (dino.pter.up_down == 1 && judge == 1)
                {
                    LCD_ShowPicture(dino.obs_left_pos, 35, dino.obs_left_pos + dino.width - 1, 54, dino.pter.obs_down, dino.pter.obs_size);
                    dino.pter.up_down = 0;
                }
            }
        }
        int res = 1;
        if (flag_un != 0)
            res = 1;
        else
            res = Collision(&dino);
        if (res == 0)
        {
            Game_Over();
            while (!Get_Button(1))
            {
                delay_1ms(50);
                if (Get_Button(1))
                    start_game();
            }
            return;
        }
    }
}

void display_help()
{
    Show_help_text();
    while (!Get_Button(0))
    {
        delay_1ms(50);
        if (Get_Button(0))
            break;
    }
}

void setting()
{
    LCD_Clear(BLACK);
    dynamic_show_text(35, 10, "Chrome Dino", 0xE0B7);
    dynamic_show_text(27, 32, "Press Button0", 0x265E);
    dynamic_show_text(47, 52, "To Start", 0x265E);
    while (!Get_Button(0))
    {
        delay_1ms(50);
        if (Get_Button(0))
            break;
    }
    LCD_Clear(BLACK);
    dynamic_show_text(50, 20, "Settings", 0xE0B7);
    dynamic_show_text(40, 45, "Play", 0x265E);
    dynamic_show_text(90, 45, "Help", 0x265E);
    LCD_DrawRectangle(38, 43, 72, 65, 0xE0B7);
    while (!Get_Button(1) || !Get_Button(0))
    {
        delay_1ms(50);
        if (Get_Button(0))
        {
            break;
        }
        if (Get_Button(1))
        {
            Switch();
            display_help();
            break;
        }
    }
    LCD_Clear(BLACK);
}

int display_mode()
{
    int flag = 0;
    dynamic_show_text(68, 20, "Mode", 0xE0B7);
    dynamic_show_text(40, 45, "Easy", 0x265E);
    dynamic_show_text(90, 45, "Hard", 0x265E);
    LCD_DrawRectangle(38, 43, 72, 65, 0xE0B7);
    while (!Get_Button(0) || !Get_Button(1))
    {
        delay_1ms(50);
        if (Get_Button(0))
        {
            delay_1ms(300);
            break;
        }
        else if (Get_Button(1))
        {
            LCD_Fill(38, 43, 72, 65, BLACK);
            LCD_ShowString(40, 45, (u8 *)"Easy", 0x265E);
            LCD_DrawRectangle(88, 43, 122, 65, 0xE0B7);
            delay_1ms(300);
            flag = 1;
            break;
        }
    }
    return flag;
}

// void start_game(){
//     LCD_Clear(BLACK);
//     int mode=display_mode();
//     LCD_Clear(BLACK);
//     delay_1ms(100);
//     LCD_DrawLine(0, 70, 159, 70, WHITE);
//     if(mode==0){ //easy
//         play(2.0 / 15, 30,200,20);
//     }
//     else{   //hard
//         play(8.0 / 15, 15,100,10);
//     }
// }

int main()
{
    IO_init(); // init OLED
    setting();
    start_game();
    return 0;
}
