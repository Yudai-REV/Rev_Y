#include "D_Main.h"
#include "D_I2C.h"
#include "D_SIO.h"
#include "D_EIO.h"
#define M_S 80                     //モーター基準値
#define Line_Dif 150               //ラインセンサー判断値
#define k 0.2                      //方位 比例制御 定数
#define STOP gPwm[0] = 0x00 | 0x80;  gPwm[1] = 0x00 | 0x80;  gPwm[2] = 0x00 | 0x80;  pwm_out(); wait_ms(500);
#define Calc_MPU Dir = gAD[ CN10 ]; Dir = Dir / 1023 * 360; Dir = 180 + Dir; if( Dir >= 360 ){ Dir = Dir - 360; }

//--------------------------------------------------------------------------------
// Program Name : rev.010.C
//--------------------------------------------------------------------------------
void user_main(void){
//---------------------------------ﾛｰｶﾙ変数定義-----------------------------------
    int M1;
    int M2;
    int M3;
    int M_PLUS;
    int B1;
    int B2;
    int B3;
    int B4;
    int B5;
    int B6;
    int LR;
    int LL;
    int LB;
    int Dir;
    int Count;
    int Data;
    int Time = 0;
    int Action;
    int GreenFR = gAD[ CN7 ] + Line_Dif;
    int GreenFL = gAD[ CN8 ] + Line_Dif;
    int GreenFB = gAD[ CN9 ] + Line_Dif;
//--------------------------------------------------------------------------------
    
    while(gAD[ CN1 ] >= 1015){
      if( gAD[ CN10 ] >= 700 || gAD[ CN10 ] <= 300 ){
        set_Led( 1 , LED_ON );
        set_Led( 2 , LED_ON );
        set_Led( 3 , LED_ON );
        set_Led( 4 , LED_ON );
      }else{
        set_Led( 1 , LED_OFF );
        set_Led( 2 , LED_OFF );
        set_Led( 3 , LED_OFF );
        set_Led( 4 , LED_OFF );      
      }
    }

    set_Led( 1 , LED_OFF );
    set_Led( 2 , LED_OFF );
    set_Led( 3 , LED_OFF );
    set_Led( 4 , LED_OFF );
       
       
    while(TRUE)
    {
    
        Calc_MPU
        M_PLUS = ( Dir - 180 ) * k;
    
        //Line_Check
        
        Time = 500;
        
        LR = gAD[ CN7 ];
        LL = gAD[ CN8 ];
        LB = gAD[ CN9 ];
        
        if( LR > GreenFR ){
          if( LB > GreenFB ){
            STOP
            M1 = -M_S/4;  M2 = -M_S/4*3;  M3 = M_S;
          }else{
            if( Action == 1 ){
              STOP
              M1 = -M_S;  M2 = 00;  M3 = M_S;
            }else if( Action == 4 ){
              STOP
              M1 = 00;  M2 = -M_S;  M3 = M_S;
            }else{
              STOP
              M1 = -M_S/2;  M2 = -M_S/2;  M3 = M_S;
            }
          }
        }else if( LL > GreenFL ){
          if( LB > GreenFB ){
            STOP
            M1 = M_S/4*3;  M2 = M_S/4;  M3 = -M_S;
          }else{
            if( Action == 1 ){
              STOP
              M1 = 00;  M2 = M_S;  M3 = -M_S;
            }else if( Action == 4 ){
              STOP
              M1 = M_S;  M2 = 00;  M3 = -M_S;
            }else{
              STOP
              M1 = M_S/2;  M2 = M_S/2;  M3 = -M_S;
            }
          }          
        }else if( LB > GreenFB ){
          if      ( Action == 1 ){
            STOP
            M1 = -M_S;  M2 = M_S;  M3 = 00;
          }else if( Action == 2 ){
            STOP
            M1 = 00;  M2 = -M_S;  M3 = M_S;
          }else if( Action == 3 ){
            STOP
            M1 = M_S;  M2 = 00;  M3 = -M_S;
          }else if( Action == 4 ){
            STOP
            M1 = M_S;  M2 = -M_S;  M3 = 00;
          }

        }else{         

          Time = 10;
          
          if( Dir > 110 && Dir < 240 ){
            B1 = gAD[ CN1 ];    //Ball_Check
            B2 = gAD[ CN2 ];
            B3 = gAD[ CN3 ];
            B4 = gAD[ CN4 ];
            B5 = gAD[ CN5 ];
            B6 = gAD[ CN6 ];
            Count = 0;
            Data = 0;
            
            if( B1 > 300 ){ Count = Count + 1; Data = Data + 6; }
            if( B2 > 350 ){ Count = Count + 1; Data = Data + 8; }
            if( B3 > 350 ){ Count = Count + 1; Data = Data + 4; }
            if( B4 > 350 ){ Count = Count + 1; Data = Data + 10; }
            if( B5 > 350 ){ Count = Count + 1; Data = Data + 2; }
            
            if( Count != 0 ){ 
              Data = Data / Count;
              
              if       ( Data ==  2 ){ M1 = -M_S;  M2 = 00;  M3 = M_S; Action = 3;
              }else if ( Data ==  3 ){ M1 = -M_S/4*3;  M2 = -M_S/4;  M3 = M_S; Action = 3;
              }else if ( Data ==  4 ){ M1 = -M_S/2;  M2 = -M_S/2;  M3 = M_S; Action = 3;
              }else if ( Data ==  5 ){ M1 = 00;  M2 = -M_S;  M3 = M_S; Action = 1;
              }else if ( Data ==  6 ){ M1 = M_S;  M2 = -M_S;  M3 = 00; Action = 1;
              }else if ( Data ==  7 ){ M1 = M_S;  M2 = 00;  M3 = -M_S; Action = 1;
              }else if ( Data ==  8 ){ M1 = M_S/2;  M2 = M_S/2;  M3 = -M_S; Action = 2;
              }else if ( Data ==  9 ){ M1 = M_S/4;  M2 = M_S/4*3;  M3 = -M_S; Action = 2;
              }else if ( Data == 10 ){ M1 = 00;  M2 = M_S;  M3 = -M_S; Action = 2;
              }
              
            }else{
              if( B6 > 400 ){  //Ball_B
                if( B6 > B5 > B4 ){
                  M1 = 00;  M2 = M_S;  M3 = -M_S;
                }else if ( B6 > B4 > B5 ){
                  M1 = -M_S;  M2 = 00;  M3 = M_S;
                }
              }else{
                M1 = 00;  M2 = 00;  M3 = 00;
              }
            }
           
          }else{
            Time  = 10;
            if( Dir > 180 ){
              M1 = 10;  M2 = 10;  M3 = 10;
            }else{
              M1 = -10;  M2 = -10;  M3 = -10;
            }
          }//Direction
 
 
        }//Line
        
        
             
          if( M1 == 00 && M2 == 00 && M3 == 00 ){
          }else{
            M1 = M1 + M_PLUS;  M2 = M2 + M_PLUS;  M3 = M3 + M_PLUS;        
          }
          
          if( M1 >= 100 ){M1 = 100; } if( M1 <= -100 ){M1 = -100; }
          if( M2 >= 100 ){M2 = 100; } if( M2 <= -100 ){M2 = -100; }
          if( M3 >= 100 ){M3 = 100; } if( M3 <= -100 ){M3 = -100; }
              
          gPwm[0] = M1 < 0 ? (M1 * -1) | 0x80 : M1 == 00 ? 0x80 : M1;
          gPwm[1] = M2 < 0 ? (M2 * -1) | 0x80 : M2 == 00 ? 0x80 : M2;
          gPwm[2] = M3 < 0 ? (M3 * -1) | 0x80 : M3 == 00 ? 0x80 : M3;
          pwm_out();
          wait_ms( Time );
        
    }//while
}