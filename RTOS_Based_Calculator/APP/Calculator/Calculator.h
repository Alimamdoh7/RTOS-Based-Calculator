/*
 * Calculator.h
 *
 * Created: 1/23/2024 11:07:50 AM
 *  Author: Ali Mamdoh
 */ 


#ifndef CALCULATOR_H_
#define CALCULATOR_H_

#include "PORT_Core.h"
#include "LCD_Core.h"
#include "KeyPad_Core.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define  Number_Of_Operands     15U
#define  Number_Of_Operations   10U

extern TaskHandle_t Init_Task_Handle;
extern TaskHandle_t Task_1_Handle;
extern TaskHandle_t Task_2_Handle;
extern QueueHandle_t  Queue1_Handle;

extern sint32 Numbers[Number_Of_Operands];
extern uint8 Operations[Number_Of_Operations];
extern uint8 Key_Value ;
extern sint32 result;
extern uint8 Division_By_Zero_Flag;
extern uint8 num_cntr;
extern uint8 op_cntr;
extern uint8 i;
extern uint8 j;

void Project_Init (void);
void Clear_Button (void);
void Operations_Buttons (void);
void Numbers_Buttons (void);
void App (void);
void Calculate (void);
void Multiplication (void);
void Division (void);
void MulDiv_Sequence(void);
void SumSub_Sequence(void);
void Division_By_Zero (void);

void Tasks_Init (void *ptr);
void Task1_KeyPad_GetValue (void *ptr);
void Task2_LCDDisplay (void *ptr);


#endif /* CALCULATOR_H_ */