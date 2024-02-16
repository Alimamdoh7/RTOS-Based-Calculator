/*
 * Calculator.c
 *
 * Created: 1/23/2024 11:07:38 AM
 *  Author: Ali Mamdoh
 */ 

#include "Calculator.h"

TaskHandle_t   Init_Task_Handle = NULL;
TaskHandle_t   Task_1_Handle = NULL;
TaskHandle_t   Task_2_Handle = NULL;
QueueHandle_t  Queue1_Handle = NULL;

sint64 Numbers[Number_Of_Operands]={0};
uint8 Operations[Number_Of_Operations]={0};
uint8 Key_Value =0;
sint64 result=0;
uint8 Division_By_Zero_Flag=0;
uint8 num_cntr=0 , op_cntr=0;
uint8 i=0 , j=0;

void Project_Init (void)
{
	PORT_Init();
	LCD_Init();
	KeyPad_Init();
	LCD_GoTo(0,0);
	LCD_WriteString("Calculator");
	_delay_ms(2000);
	LCD_Clear();
	LCD_GoTo(0,0);
	LCD_WriteString("Made By:");
	LCD_GoTo(1,0);
	LCD_WriteString("Ali Mamdoh");
	_delay_ms(2000);
	LCD_Clear();
}

void Clear_Button (void)
{
	 Division_By_Zero_Flag=0;
	 op_cntr=0;
	 num_cntr=0;
	 result=0;
	 for (i=0; i<Number_Of_Operations ; i++)
	 {
		 Operations[i]=0;
	 }
	 for (j=0 ; j<Number_Of_Operands ; j++)
	 {
		 Numbers[j]=0;
	 }
}

void Operations_Buttons (void)
{
	Operations[op_cntr] = Key_Value;
	num_cntr++;
	op_cntr++;
}

void Numbers_Buttons (void)
{
	Numbers[num_cntr] *=10;
	Numbers[num_cntr] += (Key_Value -'0');
}


void Calculate (void)
{
	MulDiv_Sequence();
	SumSub_Sequence();
	
	if (Division_By_Zero_Flag == 1)
	{
		Division_By_Zero();
	}
}

void Division_By_Zero (void)
{
	op_cntr=0;
	num_cntr=0;
	result=0;
	for (i=0; i<Number_Of_Operations ; i++)
	{
		Operations[i]=0;
	}
	for (j=0 ; j<Number_Of_Operands ; j++)
	{
		Numbers[j]=0;
	}

}

void MulDiv_Sequence (void)
{
	for (i=0 ; i < op_cntr ; i++)
	{
		if (Operations[i] == 'x' && Operations[i+1]== '/')
		{
			while (Operations[i] == 'x' && Operations[i+1] == '/')
			{
				Multiplication();
				Division();
			}
		}
		else if (Operations[i] == '/' && Operations[i+1] == 'x')
		{
			while (Operations [i] == '/' && Operations[i+1] == 'x')
			{
				Division();
			    Multiplication();
			}
		}
		else 
		{
			if (Operations[i] == 'x')
			{
				while (Operations [i] == 'x')
				{
					Multiplication();
				}
			}
			else if (Operations[i] == '/')
			{
				while (Operations [i] == '/')
				{
					Division();
				}
			}
		}
	}
	result = Numbers[0];	
}

void Multiplication (void)
{
	Numbers[i] *=  Numbers[i+1] ;
	for (j=i; j < op_cntr ; j++)
	{
		Operations[j] = Operations[j+1];
		Numbers[j+1] = Numbers [j+2];
	}
}

void Division (void)
{
	if (Numbers[i+1] == 0)
	{
		Division_By_Zero_Flag=1;
	}
	else if (Numbers[i+1] != 0)
	{
		Numbers[i] /=  Numbers[i+1] ;
	}
	for (j = i; j<op_cntr ; j++)
	{
		Operations[j] = Operations[j+1];
		Numbers[j+1] = Numbers[j+2];
	}
}

void SumSub_Sequence (void)
{
	for(i = 0 ; i < op_cntr; i++)
	{
		if(Operations[i] == '+')
		result += Numbers[i+1];

		else if (Operations[i] == '-')
		result -= Numbers[i+1];
	}
}

void Tasks_Init (void *ptr)
{
	while (1)
	{
		PORT_Init();
		LCD_Init();
		KeyPad_Init();
		LCD_GoTo(0,0);
		LCD_WriteString("Calculator");
		_delay_ms(2000);
		LCD_Clear();
		LCD_GoTo(0,0);
		LCD_WriteString("Made By:");
		LCD_GoTo(1,0);
		LCD_WriteString("Ali Mamdoh");
		_delay_ms(2000);
		LCD_Clear();
		Queue1_Handle = xQueueCreate(35 , sizeof( uint8 ));
		vTaskSuspend( Init_Task_Handle );
	}
}

void Task1_KeyPad_GetValue (void *ptr)
{
	while (1)
	{
		Key_Value= KeyPad_GetValue();
		if (Key_Value != 0)
		{
			xQueueSend(	Queue1_Handle,(const void *) &Key_Value,(TickType_t) portMAX_DELAY );
			
			if (Key_Value == 'x' || Key_Value == '/' || Key_Value == '+' || Key_Value == '-')
			{
				Operations_Buttons();
			}
			else if (Key_Value >= '0' && Key_Value <= '9')
			{
				Numbers_Buttons();
			}
			else if (Key_Value == '=')
			{
				Calculate();
				Calculate();
			}
			else if (Key_Value == 'c')
			{
				Clear_Button();
			}
		}
		vTaskDelay( (const TickType_t) 100);
	}
}


void Task2_LCDDisplay (void *ptr)
{
	uint8 Value = 0;
	while (1)
	{
		xQueueReceive( Queue1_Handle, (void *) &Value, (TickType_t) portMAX_DELAY );
		
		if (Value == 'x' || Value == '/' || Value == '+' || Value == '-'  || (Value >='0' && Value <= '9' ))
		{
			LCD_WriteChar(Value);
		}
		else if (Value == '=' && Division_By_Zero_Flag == 0)
		{
			LCD_WriteChar(Value);
			LCD_GoTo(1,0);
			LCD_WriteInteger(result);
		}
		else if (Value == '=' && Division_By_Zero_Flag == 1)
		{
			LCD_WriteChar(Value);
			LCD_GoTo(1,0);
			LCD_WriteString("Math Error");
			_delay_ms(2000);
			LCD_Clear();
			Division_By_Zero_Flag=0;
		}
		else if (Value == 'c')
		{
			LCD_Clear();
		}
		vTaskDelay( (const TickType_t) 50);
	}
}

void App (void)
{
	xTaskCreate(    Tasks_Init ,
	"Init Task" ,
	(configSTACK_DEPTH_TYPE)  100,
	NULL,
	3,
	&Init_Task_Handle
	);
	
	xTaskCreate(    Task1_KeyPad_GetValue,
	"LedTGL1",
	(configSTACK_DEPTH_TYPE) 100,
	NULL,
	2,
	&Task_1_Handle
	);
	
	xTaskCreate(    Task2_LCDDisplay,
	"ButtonCheck",
	(configSTACK_DEPTH_TYPE) 100,
	NULL,
	1,
	&Task_2_Handle
	);
	
	vTaskStartScheduler();

}