/*
 * @Author: Guoquan Wei 1940359148@qq.com 
 * @Date: 2017-10-27 19:15:47 
 * @Last Modified by: Guoquan Wei
 * @Last Modified time: 2017-10-27 19:18:21
 */

#include <reg52.h>
#include <stdio.h>
#include <intrins.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#define MAXVAL 5
int sp = 0;
int k = 0;
double val[MAXVAL];

#define MAXOP 5
#define NUMBER '0'
#define NAME 'n'

#define uchar unsigned char
#define uint unsigned int
#define LCD_data P0	//数据口
sbit LCD_RS = P3 ^ 5;  //寄存器选择输入
sbit LCD_RW = P3 ^ 6;  //液晶读/写控制
sbit LCD_EN = P3 ^ 4;  //液晶使能控制
sbit LCD_PSB = P3 ^ 7; //串/并方式控制
sbit wela = P2 ^ 6;
sbit dula = P2 ^ 7;

// code相当于常量，不能改变，但其可以节省自动变量的空间
uchar code dis1[3][16] = {
	{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'},
	{'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4', '5', '6'},
	{'7', '8', '9', '0', '+', ' ', '*', '/', '-', '%', '=', '.', ' ', ' ', ' ', ' '}};
uchar dis2[16] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

uchar row = 1, column = 0;

#define delayNOP(); {_nop_();_nop_();_nop_();_nop_();};

void delay(int ms)
{
    while(ms--)
	{
      uchar i;
	  for(i=0;i<250;i++)  
	   {
	    _nop_();			   
		_nop_();
		_nop_();
		_nop_();
	   }
	}
}

/*******************************************************************/
/*                                                                 */
/*检查LCD忙状态                                                    */
/*lcd_busy为1时，忙，等待。lcd-busy为0时,闲，可写指令与数据。      */
/*                                                                 */
/*******************************************************************/
bit lcd_busy()
{                          
   bit result;
   LCD_RS = 0;
   LCD_RW = 1;
   LCD_EN = 1;
   delayNOP();
   result = (bit)(P0&0x80);
   LCD_EN = 0;
   return(result); 
}

/*******************************************************************/
/*                                                                 */
/*写指令数据到LCD                                                  */
/*RS=L，RW=L，E=高脉冲，D0-D7=指令码。                             */
/*                                                                 */
/*******************************************************************/
void lcd_wcmd(uchar cmd)
{                          
   while(lcd_busy());
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_EN = 0;
    _nop_();
    _nop_(); 
    P0 = cmd;
    delayNOP();
    LCD_EN = 1;
    delayNOP();
    LCD_EN = 0;  
}

/*******************************************************************/
/*                                                                 */
/*写显示数据到LCD                                                  */
/*RS=H，RW=L，E=高脉冲，D0-D7=数据。                               */
/*                                                                 */
/*******************************************************************/
void lcd_wdat(uchar dat)
{                          
   while(lcd_busy());
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_EN = 0;
    P0 = dat;
    delayNOP();
    LCD_EN = 1;
    delayNOP();
    LCD_EN = 0; 
}

void initphoto()
{
	uchar i, j;
	lcd_wcmd(0x34); //写数据时,关闭图形显示
	for (i = 0; i < 32; i++)
	{
		lcd_wcmd(0x80 + i);		 //先写入水平坐标值
		lcd_wcmd(0x80);			 //写入垂直坐标值
		for (j = 0; j < 16; j++) //再写入两个8位元的数据
			lcd_wdat(0x00);
		delay(2);
	}

	for (i = 0; i < 32; i++)
	{
		lcd_wcmd(0x80 + i);		 //先写入水平坐标值
		lcd_wcmd(0x88);			 //写入垂直坐标值
		for (j = 0; j < 16; j++) //再写入两个8位元的数据
			lcd_wdat(0x00);
		delay(2);
	}
	lcd_wcmd(0x36); //写完数据,开图形显示
}

void lcd_init()
{
	LCD_PSB = 1; //并口方式
	initphoto();
	lcd_wcmd(0x31); //扩展指令操作
	delay(5);
	lcd_wcmd(0x01);
	delay(5);
	LCD_RS = 0;
	LCD_RW = 0;
	lcd_wcmd(0x0c);
	delay(5);

	/*lcd_wcmd(0x0f); //显示开，关光标
	delay(5);
	lcd_wcmd(0x01);		   //清除LCD的显示内容
	delay(5);*/
}

/*********************************************************/
/*                                                       */
/* 设定显示位置                                          */
/*                                                       */
/*********************************************************/
void lcd_pos(uchar X, uchar Y)
{
	uchar pos;
	if (X == 0)
	{
		X = 0x80;
	}
	else if (X == 1)
	{
		X = 0x90;
	}
	else if (X == 2)
	{
		X = 0x88;
	}
	else if (X == 3)
	{
		X = 0x98;
	}
	pos = X + Y;
	lcd_wcmd(pos); //显示地址
}

void printfcharater(char *p)
{
	int i = 0, j = 0;
	while (*p != '\0')
	{
		dis2[i++] = *p++;
	}
	for (j = i; j < 16; j++)
	{
		dis2[j] = ' ';
	}
	lcd_wcmd(0x30);
	delay(2);
	lcd_pos(0, 0); //设置显示位置为第四行的第1个字符
	/*for (k=0 ; k<16 ; k++ )
	{
		lcd_wdat(' ');
	}*/
	for (j = 0; j < 16; j++)
	{
		lcd_wdat(dis2[j]);
		delay(2);
	}
	lcd_wcmd(0x34);
}

void printcursor()
// 打印光标函数
{
	lcd_wcmd(0x34);
	delay(2);
	if (row == 0 || row == 1)
	{
		lcd_wcmd(0x80 + 14 + 16 * row);
		delay(2);
		lcd_wcmd(0x80 + column / 2);
		delay(2);
		if (column % 2 == 0)
		{
			lcd_wdat(0x7f);
			delay(2);
			lcd_wdat(0x00);
		}
		else
		{
			lcd_wdat(0x00);
			delay(2);
			lcd_wdat(0x7f);
		}
	}
	else if (row == 2 || row == 3)
	{
		lcd_wcmd(0x80 + 14 + 16 * (row - 2));
		delay(2);
		lcd_wcmd(0x88 + column / 2);
		delay(2);
		if (column % 2 == 0)
		{
			lcd_wdat(0x7f);
			delay(2);
			lcd_wdat(0x00);
		}
		else
		{
			lcd_wdat(0x00);
			delay(2);
			lcd_wdat(0x7f);
			delay(2);
		}
	}
	lcd_wcmd(0x36);
}

void printnumber(double num)
{
	char buffer[5];
	sprintf(buffer, "%4.2f", num);
	// %m.n在浮点数中，m也表示宽度；n表示小数的位数,#include <stdio.h>
	printfcharater(buffer);
}

void cleanpastcursor(int i, int j)
// 光标移动后，将上一个光标显示清除掉
{
	lcd_wcmd(0x34);
	delay(2);
	if (i == 0 || i == 1)
	{
		lcd_wcmd(0x80 + 14 + 16 * i);
		delay(2);
		lcd_wcmd(0x80 + j / 2);
		delay(2);
		lcd_wdat(0x00);
		delay(2);
		lcd_wdat(0x00);
	}
	else if (i == 2 || i == 3)
	{
		lcd_wcmd(0x80 + 14 + 16 * (i - 2));
		delay(2);
		lcd_wcmd(0x88 + j / 2);
		delay(2);
		lcd_wdat(0x00);
		delay(2);
		lcd_wdat(0x00);
		delay(2);
	}
	lcd_wcmd(0x36);
}

void push(double f)
{
	if (sp < MAXVAL)
	{
		val[sp++] = f;
	}
	else
	{
		printfcharater("error");
	}
}

double pop(void)
{
	if (sp > 0)
	{
		return val[--sp];
	}
	else
	{
		printfcharater("error");
		return 0.0;
	}
}

int getchar1()
{
	int i, j, u = 0;
	// i,j用来存储光标变化前的位置
	bit flag1 = 0, flag2 = 0;
	// 用flag非常重要，否则由于while语句一直循环，造成显示的不稳定，有乱码
	uchar temp;

	while (1)
	{
		i = row;
		j = column;
		P3 = 0xfe;
		temp = P3;
		temp = temp & 0xf0;
		if (temp != 0xf0)
		{
			delay(10);
			temp = P3;
			temp = temp & 0xf0;
			if (temp != 0xf0)
			{
				temp = P3;
				switch (temp)
				{
				case 0xde:
					// key = 1;
					if (row == 0)
					{
						row = 3;
						flag1 = 1;
					}
					else
					{
						row--;
						flag1 = 1;
					}
					break;
				}
				while (temp != 0xf0)
				{
					temp = P3;
					temp = temp & 0xf0;
				}
			}
		}

		P3 = 0xfd;
		temp = P3;
		temp = temp & 0xf0;
		if (temp != 0xf0)
		{
			delay(10);
			temp = P3;
			temp = temp & 0xf0;
			if (temp != 0xf0)
			{
				temp = P3;
				switch (temp)
				{
				case 0xed:
					// key = 4;
					if (column == 0)
					{
						column = 15;
						flag1 = 1;
					}
					else
					{
						column--;
						flag1 = 1;
					}
					break;
				case 0xdd:
					// key = 5;
					flag2 = 1;
					break;
				case 0xbd:
					// key = 6;
					if (column == 15)
					{
						column = 0;
						flag1 = 1;
					}
					else
					{
						column++;
						flag1 = 1;
					}
					break;
				}
				while (temp != 0xf0)
				{
					temp = P3;
					temp = temp & 0xf0;
				}
			}
		}

		P3 = 0xfb;
		temp = P3;
		temp = temp & 0xf0;
		if (temp != 0xf0)
		{
			delay(10);
			temp = P3;
			temp = temp & 0xf0;
			if (temp != 0xf0)
			{
				temp = P3;
				switch (temp)
				{
				case 0xdb:
					// key = 9;
					if (row == 3)
					{
						row = 0;
						flag1 = 1;
					}
					else
					{
						row++;
						flag1 = 1;
					}
					break;
				}
				while (temp != 0xf0)
				{
					temp = P3;
					temp = temp & 0xf0;
				}
			}
		}
		if (flag1 == 1)
		{
			flag1 = 0;
			cleanpastcursor(i, j);
			printcursor();
		}
		if (flag2 == 1)
		{
			flag2 = 0;
			dis2[k++] = dis1[row - 1][column];
			// 此处k应该为全局变量才行，因为每次return后，k值不会保存，又会变成0
			lcd_wcmd(0x30);
			delay(1);
			// 写入数据之前要开基本指令，输入完数据之后开扩展指令。因为之前是扩展指令进行了绘图模式
			lcd_pos(0, 0);
			for (u = 0; u < 16; u++)
			{
				lcd_wdat(dis2[u]);
				delay(1);
			}
			lcd_wcmd(0x36);
			// 上面两个delay(1)很特殊，如果delay2或者5的话，会出现显示不稳定的问题
			return dis1[row - 1][column];
		}
	}
}

#define BUFSIZE 5

uchar buf[BUFSIZE];
uchar bufp = 0;

int getch(void)
{
	return ((bufp > 0) ? buf[--bufp] : getchar1());
}

void ungetch(int c)
{
	if (bufp >= BUFSIZE)
	{
		printfcharater("error");
	}
	else
	{
		buf[bufp++] = c;
	}
}

// getop function:获取下一个运算号或值操作数
int getop(char s[])
{
	int i, c;

	while ((s[0] = c = getch()) == ' ')
	{
		;
	}
	s[1] = '\0';
	i = 0;
	if (!isdigit(c) && c != '.' && c != '-' && !islower(c))
	{
		return c;
	}
	if (islower(c))
	{
		while (islower(s[++i] = c = getch()))
		{
			;
		}
		s[i] = '\0';
		if (1)
		{
			ungetch(c);
		}
		if (strlen(s) > 1)
		{
			return NAME;
		}
		else
		{
			return s[--i];
		}
	}
	else if (c == '-')
	{
		if (isdigit(c = getch()) || c == '.')
		{
			s[++i] = c;
			while (isdigit(c = getch()) || c == '.')
			{
				s[++i] = c;
			}
			if (1)
			{
				ungetch(c);
				return NUMBER;
			}
		}
		else if (1)
		{
			ungetch(c);
		}
		return '-';
	}
	else if (isdigit(c))
	{

		while (isdigit(s[++i] = c = getch()))
		{
			;
		}

		if (c == '.')
		{
			while (isdigit(s[++i] = c = getch()) || c == '@')
			{
				;
			}
		}
		s[i] = '\0';
		if (1)
		{
			ungetch(c);
		}
	}
	else
	{

		while (isdigit(s[++i] = c = getch()))
		{
			;
		}

		s[i] = '\0';
		if (1)
		{
			ungetch(c);
		}
	}
	return NUMBER;
}

void mathfnc(char s[])
{
	double op2;
	if (strcmp(s, "sin") == 0)
	{
		push(sin(pop()));
	}
	else if (strcmp(s, "cos") == 0)
	{
		push(cos(pop()));
	}
	else if (strcmp(s, "pow") == 0)
	{
		op2 = pop();
		push(pow(pop(), op2));
	}
	else if (strcmp(s, "exp") == 0)
	{
		push(pow(pop(), op2));
	}
	else
	{
		printfcharater("error");
	}
}

main()
{
	uchar k, i;
	uchar type;
	double op2, v;
	uchar s[MAXOP];

	delay(10); //延时
	wela = 0;
	dula = 0;
	lcd_init(); //初始化LCD

	lcd_pos(0, 0); //设置显示位置为第四行的第1个字符
	for (k = 0; k < 16; k++)
	{
		lcd_wdat(dis2[k]);
	}

	lcd_pos(1, 0); //设置显示位置为第四行的第1个字符
	for (i = 1, k = 0; k < 16; k++)
	{
		lcd_wdat(dis1[i - 1][k]);
	}

	lcd_pos(2, 0); //设置显示位置为第四行的第1个字符
	k = 0;
	for (i = 2, k = 0; k < 16; k++)
	{
		lcd_wdat(dis1[i - 1][k]);
	}

	lcd_pos(3, 0); //设置显示位置为第四行的第1个字符
	for (i = 3, k = 0; k < 16; k++)
	{
		lcd_wdat(dis1[i - 1][k]);
	}

	printcursor();
	/*lcd_wcmd(0x34);
	lcd_wcmd(0x80+30);
	lcd_wcmd(0x88+1);
	lcd_wdat(0x00);
	lcd_wdat(0x7f);
	lcd_wcmd(0x36);*/
	while (1)
	{
		type = getop(s);
		switch (type)
		{
		case NAME:
			mathfnc(s);
			break;
		case NUMBER:
			push(atof(s));
			break;
		case '+':
			push(pop() + pop());
			break;
		case '*':
			push(pop() * pop());
			break;
		case '-':
			op2 = pop();
			push(pop() - op2);
			break;
		case '/':
			op2 = pop();
			if (op2 != 0.0)
			{
				push(pop() / op2);
			}
			else
			{
				printfcharater("error");
			}
			break;
		case '%':
			op2 = pop();
			if (op2 != 0.0)
			{
				push(fmod(pop(), op2));
			}
			else
			{
				printfcharater("error");
			}
			break;
		case '=':
			v = pop();
			printnumber(v);
			break;
		default:
			printfcharater("error");
			break;
		}
	}
}