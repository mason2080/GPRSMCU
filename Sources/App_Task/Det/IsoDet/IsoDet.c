#include "IsoDet.h"
#include "IsoIf.h"
#include "Iso_Macro.h"
#include "BatterySt.h"
#include "SchIf.h"
#include "ErrorAlarm.h"
#include "Ecu.h"
#include "IsoDetPrjConf.h"


#define FLAG_ISO_NORMAL   1
#define FLAG_ISO_ERROR    0
#define ERR_ISO_LEVEL2_CNT 5
#define ERR_ISO_LEVEL3_CNT 4

#define SWITCH_MOS_CNT 5

#define POS_SIDE_ERR  1
#define NEG_SIDE_ERR  2
#define MIDD_SIDE_ERR 3

uWord gain_insr = 0;
uWord gain_insr1 = 0;

typedef struct
{
    uByte cnt;
    uByte delay_timecnt;
} sIsoDet_TmCntType;

sIsoDet_TmCntType IsoDet_TmCnt;

uByte gby_ErrcntIso500 = 0;
uByte gby_ErrcntIso100 = 0;
uWord gwd_Iso_R = 0;
uByte gby_iso_location = 0;
uWord gwd_gain_rslt = 0;
uWord gwd_rslt1 = 0;

uByte gby_ChangeModeDelayFlag; // change mode delay
uByte gby_ChangeModeBak;
uWord gwd_ChangeModeWaitCnt;
uByte gby_InitFlag = 0;
uByte gby_SwitchMosCnt; 
//uByte gby_IsoErrSide;

uByte gby_IsoFlagTst[2] = { 0, 0};

const uByte gby_ErrLevelNum = 2;

static void delay_ms(uByte ms)
{
    uWord i;
    for (i=0; i<100; i++)
    {
        asm nop;
    }
}

uWord IsoDet_CaclIsoR(uWord scale)
{
    uWord sumraz;
    uWord insr;

    sumraz = 1690;
    insr = sumraz / scale;

    return insr;
}

typedef struct
{
    //const uByte  * p_level; // point to level array
    const uWord  * p_sumv; // point to sumv array
    const uWord  * p_table; // ponit to table
} sIsoDet_CurveType;


sIsoDet_CurveType IsoDet_Curve =
{
    //IsoDet_CnstErrLevel,
    IsoDet_CnstSumvArray,
    &IsoDet_CnstThrehTable[0][0]
};


static uWord IsoDet_GetCurveThreh(uWord Sumv,uByte Level,sIsoDet_CurveType * p_curve, uByte Sumv_Num, uByte Level_Num)
{
    uByte i,j,tmp_sumv_num;
    uWord s_sumv;
    uLWord sllwtmp[2],sllwtmp_calc[2];
    uLWord sllw_k,sllw_cur;
    uWord wtmp;

#define ISO_CUR_GAIN 100;

    sllw_cur = 0; // initialate current

    // prevent soc error value
    s_sumv = Sumv;

    // prevent length overflow max length
    
    (Sumv_Num > IsoDet_PrjConf.SumVNum) ? (tmp_sumv_num = IsoDet_PrjConf.SumVNum) : (tmp_sumv_num = Sumv_Num);
    (Level > IsoDet_PrjConf.ErrLevelNum) ? (i = IsoDet_PrjConf.ErrLevelNum) : (i = Level);

    if(i >= 1)
    {
        i -= 1;
    }
    // on the table point
    for(j = 0; j < tmp_sumv_num; j++)
    {
        if(0 == j)
        {
            if(s_sumv < *(p_curve->p_sumv + j))
            {
                sllw_cur = *(p_curve->p_table + tmp_sumv_num * i + j);
                break;
            }
        }
        if((tmp_sumv_num - 1) == j)
        {
            if(s_sumv > *(p_curve->p_sumv + j))
            {
                sllw_cur = *(p_curve->p_table + tmp_sumv_num * i + j);
                break;
            }
        }
        if(s_sumv == *(p_curve->p_sumv + j))
        {
            // on the table point
            sllw_cur = *(p_curve->p_table + tmp_sumv_num * i + j);

            break;
        }
        else
        {
            if(s_sumv < *(p_curve->p_sumv + j + 1))
            {
                // soc between the table point

                sllwtmp[0] = *(p_curve->p_table + tmp_sumv_num * i + j);
                sllwtmp[0] *= ISO_CUR_GAIN;
                sllwtmp[1] = *(p_curve->p_table + tmp_sumv_num * i + j + 1);
                sllwtmp[1] *= ISO_CUR_GAIN;

                // calculate derivative
                sllw_k = (sllwtmp[0] - sllwtmp[1])
                         /(*(p_curve->p_sumv + j + 1) - *(p_curve->p_sumv + j));
                sllw_cur = sllw_k * (*(p_curve->p_sumv + j + 1) - s_sumv) + sllwtmp[1];
                sllw_cur /= ISO_CUR_GAIN;

                break;
            }
        }
    }

    wtmp = (uWord)(sllw_cur);

    return wtmp;
}

static uWord IsoDet_GetIsoThreh(uWord Sumv,uByte Level)
{
    return IsoDet_GetCurveThreh(Sumv,Level,&IsoDet_Curve,IsoDet_PrjConf.SumVNum,IsoDet_PrjConf.ErrLevelNum);
}

/* ISO_POS for ptb1, ISO_NEG for ptb0 */

GeneralReturnType IsoDet_Judge(eIso_IdNumType Iso_Id,uByte* Iso_FlagPtr,uWord Sumv)
{
    GeneralReturnType st;
    uWord v_pos = 0, v_neg = 0;
    st = (GeneralReturnType)ERR_OK;

#ifdef __DEBUG_ISO_FAIL
    *flag_iso = FLAG_ISO_ERROR;
    if (gby_ErrLevelNum > 1)
    {
        flag_iso++;
        *flag_iso = FLAG_ISO_ERROR;
    }

    return FLAG_ISO_OK;
#endif

#ifdef __DEBUG_ISO_OK
    *flag_iso = FLAG_ISO_NORMAL;
    if (gby_ErrLevelNum > 1)
    {
        flag_iso++;
        *flag_iso = FLAG_ISO_NORMAL;
    }

    return FLAG_ISO_OK;
#endif

    //read the value of v_pos and v_neg
    st = IsoIf_RdAdValue(Iso_Id,&v_pos,&v_neg);

    //check the value and calculate
    if (st != ERR_OK)
    {
        return st;
    }
    else
    {
        if (ISO_MODE_POS_ON_NEG_ON == gby_flag_iso_mode)      // pos and neg mos all ON
        {
            glwd_iso_rslt_pos[0] += v_pos;
            glwd_iso_rslt_neg[0] += v_neg;
        }

        gby_flg_iso_cnt++;

        if (gby_flg_iso_cnt >= RERULT_NUM)
        {
            gby_flg_iso_cnt = 0;

            /* clear flag firstly */
            *Iso_FlagPtr = FLAG_ISO_NORMAL;
            if (gby_ErrLevelNum > 1)
            {
                *(Iso_FlagPtr+1) = FLAG_ISO_NORMAL;
            }

            if (ISO_MODE_POS_ON_NEG_ON == gby_flag_iso_mode)
            {
                if (glwd_iso_rslt_pos[0] > glwd_iso_rslt_neg[0])
                {
                    if ( (glwd_iso_rslt_neg[0] == 0) )
                    {
                        /* if(  ( (SUMV_RATIO_100mV == (SensorCfg.ratio_cfg&SUMV_RATIO_MASK))
                                &&(Sumv > ISO_CHECK_SUMV_THRD_100mV) )
                             ||( ((SensorCfg.ratio_cfg&SUMV_RATIO_MASK) != SUMV_RATIO_100mV)
                                &&(Sumv > ISO_CHECK_SUMV_THRD_10mV) )  )  */
                        // default ratio is 0.01V.
                        //if (gsvolt.sumv > ISO_CHECK_SUMV_THRD)
                        if(Sumv > ISO_CHECK_SUMV_THRD_100mV)
                        {
                            gwd_rslt = 0x99;

                            *Iso_FlagPtr = FLAG_ISO_ERROR;

                            if (gby_ErrLevelNum > 1)
                            {
                                Iso_FlagPtr++;
                                *Iso_FlagPtr = FLAG_ISO_ERROR;
                            }
                        }
                    }
                    else
                    {
                        gwd_rslt = (uWord)(glwd_iso_rslt_pos[0] / glwd_iso_rslt_neg[0]);

                        if (gwd_rslt > 12)
                        {
                            *Iso_FlagPtr = FLAG_ISO_ERROR;

                            if (gby_ErrLevelNum > 1)
                            {
                                if (gwd_rslt >50)
                                {
                                    Iso_FlagPtr++;
                                    *Iso_FlagPtr = FLAG_ISO_ERROR;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if ( (glwd_iso_rslt_pos[0] == 0))
                    {
                        /*if(  ( ((SensorCfg.ratio_cfg & SUMV_RATIO_MASK) == SUMV_RATIO_100mV)
                               &&(Sumv > ISO_CHECK_SUMV_THRD_100mV) )
                            ||( ((SensorCfg.ratio_cfg & SUMV_RATIO_MASK) != SUMV_RATIO_100mV)
                               &&(Sumv > ISO_CHECK_SUMV_THRD_10mV) )  )  */
                        // default ratio is 0.01V.
                        //if (gsvolt.sumv > ISO_CHECK_SUMV_THRD)
                        if(Sumv > ISO_CHECK_SUMV_THRD_100mV)
                        {
                            gwd_rslt = 0x99;

                            *Iso_FlagPtr = FLAG_ISO_ERROR;

                            if (gby_ErrLevelNum > 1)
                            {
                                Iso_FlagPtr++;
                                *Iso_FlagPtr = FLAG_ISO_ERROR;
                            }
                        }
                    }
                    else
                    {
                        gwd_rslt = (uWord)(glwd_iso_rslt_neg[0] / glwd_iso_rslt_pos[0]);
                        if (gwd_rslt > 12)
                        {
                            *Iso_FlagPtr = FLAG_ISO_ERROR;

                            if (gby_ErrLevelNum > 1)
                            {
                                if (gwd_rslt > 45)
                                {
                                    Iso_FlagPtr++;
                                    *Iso_FlagPtr = FLAG_ISO_ERROR;
                                }
                            }
                        }
                    }
                }
            }

            glwd_iso_rslt_pos[0] = 0;
            glwd_iso_rslt_pos[1] = 0;

            glwd_iso_rslt_neg[0] = 0;
            glwd_iso_rslt_neg[1] = 0;

            st = FLAG_ISO_DONE_ONCE;
        }
        else
        {
            st = ERR_OK ;
        }
    }
    return st;
}

// sumv resolution must to  be 0.1V.
GeneralReturnType IsoDet_ExtMode1Judge(eIso_IdNumType Iso_Id,uByte* Iso_FlagPtr, uWord Sumv)
{
    GeneralReturnType st;
    uWord v_pos = 0, v_neg = 0;
    uByte lby_flag;

    uWord lwd_sumv = Sumv;
    uWord min_sumv = 0;
    uWord lwd_v_mode = 0;
    uByte mode;
    //uByte gain_insr = 0;
    st = (GeneralReturnType)ERR_OK;

    //read the value of v_pos and v_neg
    st = IsoIf_RdAdValue(Iso_Id,&v_pos,&v_neg);
    if(v_pos <= IsoDet_PrjConf.DetMinAdVal)
    {
        v_pos = 0;
    }

    if(v_neg <= IsoDet_PrjConf.DetMinAdVal)
    {
        v_neg = 0;
    }
    //check the AD read value and calculate


    if (st != ERR_OK)
    {
        return st;
    }
    else
    {
        gwd_ChangeModeWaitCnt++;
        if(  (gwd_ChangeModeWaitCnt >= IsoDet_PrjConf.DoubleEndDetFilterTime)
                ||(ISO_MODE_POS_ON_NEG_ON == gby_flag_iso_mode))
        {

            if (ISO_MODE_POS_ON_NEG_ON == gby_flag_iso_mode)      // pos and neg mos all ON
            {
                glwd_iso_rslt_pos[0] += v_pos;
                glwd_iso_rslt_neg[0] += v_neg;
            }
            else if (ISO_MODE_POS_ON_NEG_OFF == gby_flag_iso_mode)// pos is ON, while neg is OFF
            {
                glwd_iso_rslt_pos[1] += v_pos;
                glwd_iso_rslt_neg[1] += v_neg;
            }
            else if (ISO_MODE_POS_OFF_NEG_ON == gby_flag_iso_mode)// pos is OFF, while neg is ON
            {
                glwd_iso_rslt_pos[2] += v_pos;
                glwd_iso_rslt_neg[2] += v_neg;
            }
            gby_flg_iso_cnt++;
            gwd_ChangeModeWaitCnt = IsoDet_PrjConf.DoubleEndDetFilterTime;

            min_sumv = lwd_sumv;
            glwd_sumv_acc += lwd_sumv;

        }



        if (gby_flg_iso_cnt >= RERULT_NUM)
        {
            gby_flg_iso_cnt = 0;

            if (ISO_MODE_POS_ON_NEG_ON == gby_flag_iso_mode)
            {
                /* clear flag firstly */
                *Iso_FlagPtr = FLAG_ISO_NORMAL;
                if (gby_ErrLevelNum > 1)
                {
                    *(Iso_FlagPtr+1) = FLAG_ISO_NORMAL;
                }

                lby_flag = FLAG_ISO_NORMAL;

                // indicate change mode
                if(0 == gby_ChangeModeDelayFlag)
                {

                    if (glwd_iso_rslt_pos[0] > glwd_iso_rslt_neg[0])
                    {
                        if (0 == glwd_iso_rslt_neg[0])
                        {
                            /*  if(((SUMV_RATIO_100mV == (SensorCfg.ratio_cfg & SUMV_RATIO_MASK))
                                  &&(lwd_sumv> ISO_CHECK_SUMV_THRD_100mV))
                               ||(((SensorCfg.ratio_cfg & SUMV_RATIO_MASK) != SUMV_RATIO_100mV)
                                  &&(lwd_sumv > ISO_CHECK_SUMV_THRD_10mV))) */
                            //if (lwd_sumv > ISO_CHECK_SUMV_THRD)
                            if(lwd_sumv > IsoDet_PrjConf.DetMinSumV)
                            {
                                gwd_rslt = 0x99;

                                *Iso_FlagPtr = FLAG_ISO_ERROR;
                                lby_flag = FLAG_ISO_ERROR;

                                if (gby_ErrLevelNum > 1)
                                {
                                    Iso_FlagPtr++;
                                    *Iso_FlagPtr = FLAG_ISO_ERROR;
                                }

                                gwd_Iso_R = 0;
                                gby_iso_location = NEG_SIDE_ERR;
                            }
                        }
                        else
                        {
                            gwd_rslt = (uWord)(glwd_iso_rslt_pos[0] / glwd_iso_rslt_neg[0]);
                            // gwd_gain_rslt = (Word)IsoDet_CalcPmtRGain(min_sumv,gwd_rslt);
                            //gwd_rslt1 = gwd_rslt + gwd_gain_rslt / 100;

                            // gwd_Iso_R = IsoDet_CaclIsoR(gwd_rslt1);
                            gain_insr = IsoDet_GetIsoThreh(min_sumv,1);//IsoDet_CalcPmtGain(min_sumv,1);
                            if(gwd_rslt > gain_insr)//if (gwd_rslt > 12)     //15
                            {
                                *Iso_FlagPtr = FLAG_ISO_ERROR;
                                lby_flag = FLAG_ISO_ERROR;

                                gby_iso_location = NEG_SIDE_ERR;

                                if (gby_ErrLevelNum > 1)
                                {
                                    gain_insr1 = IsoDet_GetIsoThreh(min_sumv,2);//IsoDet_CalcPmtGain(min_sumv,2);
                                    if(gwd_rslt > gain_insr1)//if (gwd_rslt > 50)      //90
                                    {
                                        Iso_FlagPtr++;
                                        *Iso_FlagPtr = FLAG_ISO_ERROR;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if ( 0 == glwd_iso_rslt_pos[0])
                        {
                            /*if(  ( (SUMV_RATIO_100mV == (SensorCfg.ratio_cfg & SUMV_RATIO_MASK))
                                   &&(lwd_sumv> ISO_CHECK_SUMV_THRD_100mV) )
                                ||( ((SensorCfg.ratio_cfg & SUMV_RATIO_MASK) != SUMV_RATIO_100mV)
                                   &&(lwd_sumv > ISO_CHECK_SUMV_THRD_10mV) )  )*/

                            //if (lwd_sumv  > ISO_CHECK_SUMV_THRD)
                            if(lwd_sumv > IsoDet_PrjConf.DetMinSumV)
                            {
                                gwd_rslt = 0x99;

                                *Iso_FlagPtr = FLAG_ISO_ERROR;
                                lby_flag = FLAG_ISO_ERROR;

                                if (gby_ErrLevelNum > 1)
                                {
                                    Iso_FlagPtr++;
                                    *Iso_FlagPtr = FLAG_ISO_ERROR;
                                }

                                gwd_Iso_R = 0;
                                gby_iso_location = POS_SIDE_ERR;
                            }
                        }
                        else
                        {
                            gwd_rslt = (uWord)(glwd_iso_rslt_neg[0] / glwd_iso_rslt_pos[0]);
                            //gwd_gain_rslt = (Word)IsoDet_CalcPmtRGain(min_sumv,gwd_rslt);
                            //gwd_rslt1 = gwd_rslt + gwd_gain_rslt / 100;

                            //gwd_Iso_R= IsoDet_CaclIsoR(gwd_rslt1);
                            gain_insr  = IsoDet_GetIsoThreh(min_sumv,1);//IsoDet_CalcPmtGain(min_sumv,1);
                            if(gwd_rslt > gain_insr)//if (gwd_rslt > 12)     //15
                            {
                                *Iso_FlagPtr = FLAG_ISO_ERROR;
                                lby_flag = FLAG_ISO_ERROR;

                                gby_iso_location = POS_SIDE_ERR;

                                if (gby_ErrLevelNum > 1)
                                {
                                    gain_insr1 = IsoDet_GetIsoThreh(min_sumv,2);//IsoDet_CalcPmtGain(min_sumv,2);
                                    if(gwd_rslt > gain_insr1)//if (gwd_rslt > 50)      //90
                                    {
                                        Iso_FlagPtr++;
                                        *Iso_FlagPtr = FLAG_ISO_ERROR;
                                    }
                                }
                            }
                        }
                    }
                }

                if(IsoDet_PrjConf.DoubleEndDetEn == FLAG_ENABLE)
                {
                    //
                    if (FLAG_ISO_ERROR == lby_flag) // if there is slight isolation fault,keep check asymmetric fault
                    {
                        gby_mode_convert_cnt = 0;
                    }
                    else
                    {
                        // check weather change mode
 /*                       
                        if(1 == gby_ChangeModeDelayFlag)
                        {
                            gby_ChangeModeDelayFlag = 0;
                            st = IsoIf_ChangeMosMode(Iso_Id,ISO_MODE_POS_ON_NEG_OFF);  // begin to check symmetry fault
                            if(st != ERR_OK)
                            {
                                return st;
                            }
                            gwd_ChangeModeWaitCnt = 0;
                        }
*/
                        gby_mode_convert_cnt ++;
                        if (gby_mode_convert_cnt >= ASYMMETRIC_CHECK_NUM)
                        {
//                            gby_ChangeModeDelayFlag = 1;
                            gby_mode_convert_cnt = 0;
                            
                            gby_ChangeModeDelayFlag = 0;
                            st = IsoIf_ChangeMosMode(Iso_Id,ISO_MODE_POS_ON_NEG_OFF);  // begin to check symmetry fault
                            if(st != ERR_OK)
                            {
                                return st;
                            }
                            gwd_ChangeModeWaitCnt = 0;
 /*
                            st = IsoIf_OpenMos(Iso_Id,OPT_MOS_POS);
                            if(st != ERR_OK)
                            {
                                return st;
                            }
                            st = IsoIf_OpenMos(Iso_Id,OPT_MOS_NEG);
                            if(st != ERR_OK)
                            {
                                return st;
                            }
*/                            
                        }
                    }
                }
                st = FLAG_ISO_DONE_ONCE;
            }

            else if(ISO_MODE_POS_ON_NEG_OFF == gby_flag_iso_mode)
            {
                if(0 == gby_ChangeModeDelayFlag)
                {
                    st = IsoIf_OpenMos(Iso_Id,OPT_MOS_POS);

                    if(st != ERR_OK)
                    {
                        return st;
                    }
                    st = IsoIf_OpenMos(Iso_Id,OPT_MOS_NEG);
                    if(st != ERR_OK)
                    {
                        return st;
                    }

                    glwd_iso_rslt_pos[1] = glwd_iso_rslt_pos[1]/RERULT_NUM;

                    /*  if(SUMV_RATIO_10mV == (SensorCfg.ratio_cfg & SUMV_RATIO_MASK))
                      {
                          glwd_iso_rslt_pos[1] = glwd_iso_rslt_pos[1]*POS_GAIN;
                      }
                      else
                      {
                          glwd_iso_rslt_pos[1] = glwd_iso_rslt_pos[1]*POS_GAIN1;
                      }*/
                    glwd_iso_rslt_pos[1] = glwd_iso_rslt_pos[1]*POS_GAIN;

                    glwd_iso_rslt_pos[1] = glwd_iso_rslt_pos[1]/4095;

                    gwd_u_mode_POS_ON = (uWord)glwd_iso_rslt_pos[1];   // U2
                    gwd_sumv_POS_ON = (uWord)(glwd_sumv_acc/RERULT_NUM); // U1

                    // indicate next cycle change mode
                    gby_ChangeModeDelayFlag = 1;
                }
                else
                {
                    gby_ChangeModeDelayFlag = 0;
                    st = IsoIf_ChangeMosMode(Iso_Id,ISO_MODE_POS_OFF_NEG_ON);
                    if(st != ERR_OK)
                    {
                        return st;
                    }
                    gwd_ChangeModeWaitCnt = 0;
                }

            }
            else if(ISO_MODE_POS_OFF_NEG_ON == gby_flag_iso_mode)
            {
                if(0 == gby_ChangeModeDelayFlag)
                {

                    st = IsoIf_OpenMos(Iso_Id,OPT_MOS_POS);
                    if(st != ERR_OK)
                    {
                        return st;
                    }
                    st = IsoIf_OpenMos(Iso_Id,OPT_MOS_NEG);
                    if(st != ERR_OK)
                    {
                        return st;
                    }

                    glwd_iso_rslt_neg[2] = glwd_iso_rslt_neg[2]/RERULT_NUM;
                    /* if(SUMV_RATIO_10mV == (SensorCfg.ratio_cfg&SUMV_RATIO_MASK))
                     {
                        glwd_iso_rslt_neg[2] = glwd_iso_rslt_neg[2]*NEG_GAIN;
                     }
                     else
                     {
                        glwd_iso_rslt_neg[2] = glwd_iso_rslt_neg[2]*NEG_GAIN1;
                     } */
                    glwd_iso_rslt_neg[2] = glwd_iso_rslt_neg[2]*NEG_GAIN;
                    glwd_iso_rslt_neg[2] = glwd_iso_rslt_neg[2]/4095;

                    gwd_u_mode_NEG_ON = (uWord)glwd_iso_rslt_neg[2];   // U3
                    gwd_sumv_NEG_ON = (uWord)(glwd_sumv_acc/RERULT_NUM); // U1'



                    /*if ( (( SUMV_RATIO_10mV ==(SensorCfg.ratio_cfg&SUMV_RATIO_MASK))&&
                        ( (gwd_sumv_POS_ON > (gwd_sumv_NEG_ON + ISO_CALC_DET_SUMV_THERD))
                        || (gwd_sumv_NEG_ON > (gwd_sumv_POS_ON + ISO_CALC_DET_SUMV_THERD))
                        || (gwd_sumv_POS_ON < ISO_CHECK_SUMV_THRD_10mV)) )  ||
                        ( (SUMV_RATIO_100mV == (SensorCfg.ratio_cfg & SUMV_RATIO_MASK))&&
                        (gwd_sumv_POS_ON > (gwd_sumv_NEG_ON + ISO_CALC_DET_SUMV_THERD1))
                        || (gwd_sumv_NEG_ON > (gwd_sumv_POS_ON + ISO_CALC_DET_SUMV_THERD1))
                        || (gwd_sumv_POS_ON <  ISO_CHECK_SUMV_THRD_100mV)*/
                    if((gwd_sumv_POS_ON > (gwd_sumv_NEG_ON + ISO_CALC_DET_SUMV_THERD1))
                            || (gwd_sumv_NEG_ON > (gwd_sumv_POS_ON + ISO_CALC_DET_SUMV_THERD1))
                            || (gwd_sumv_POS_ON < IsoDet_PrjConf.DetMinSumV ))


                    {
                        // sumv change to big, ignore this isolation calculation, keep iso state.
                        // sumv should more 100V.
                        /*
                        ECU_DELAYMS(1);
                        mode = ISO_MODE_POS_ON_NEG_OFF;
                        gby_ChangeModeDelayFlag = 1;
                        st = IsoIf_ChangeMosMode(Iso_Id,ISO_MODE_POS_ON_NEG_OFF);  // continue to check symmetry fault
                        if(st != ERR_OK)
                        {
                            return st;
                        } */
                        
                        gby_SwitchMosCnt++;
                        
                        if(gby_SwitchMosCnt >= SWITCH_MOS_CNT)
                        {
                            gby_SwitchMosCnt = 0;
                            gby_ChangeModeBak = ISO_MODE_POS_ON_NEG_ON; 

                            *Iso_FlagPtr = FLAG_ISO_NORMAL;
                            if (gby_ErrLevelNum > 1)
                            {
                                *(Iso_FlagPtr+1) = FLAG_ISO_NORMAL;
                            }                               
                        }
                        else
                        {
                            gby_ChangeModeBak = ISO_MODE_POS_ON_NEG_OFF;
                        }
                        gby_ChangeModeDelayFlag = 1;
                    }

                    else
                    {
                        /* clear flag firstly */
                        gby_SwitchMosCnt = 0;
                        
                        *Iso_FlagPtr = FLAG_ISO_NORMAL;
                        if (gby_ErrLevelNum > 1)
                        {
                            *(Iso_FlagPtr+1) = FLAG_ISO_NORMAL;
                        }

                        lwd_sumv = gwd_sumv_POS_ON;
                        lwd_sumv += gwd_sumv_NEG_ON;

                        lwd_sumv /= 2;

                        lwd_v_mode = gwd_u_mode_POS_ON;
                        lwd_v_mode += gwd_u_mode_NEG_ON;

                        /*if( (( SUMV_RATIO_10mV == (SensorCfg.ratio_cfg&SUMV_RATIO_MASK))&&
                            (lwd_sumv < (lwd_v_mode + (lwd_sumv/ISO_MODE1_THRED1_GAIN)) ) ) ||
                            (( SUMV_RATIO_100mV == (SensorCfg.ratio_cfg&SUMV_RATIO_MASK))&&
                            (lwd_sumv < (lwd_v_mode + (lwd_sumv/ISO_MODE1_THRED1_GAIN)) ) )  )*/
                        if(lwd_sumv < (lwd_v_mode + (lwd_sumv/ISO_MODE1_THRED1_GAIN)))
                        {
                            *Iso_FlagPtr = FLAG_ISO_ERROR;

                            gby_iso_location = MIDD_SIDE_ERR;
                            /*
                            ECU_DELAYMS(1);
                            st = IsoIf_ChangeMosMode(Iso_Id,ISO_MODE_POS_ON_NEG_OFF);  // continue to check symmetry fault
                            if(st != ERR_OK)
                            {
                                return st;
                            }
                            */
                            gby_ChangeModeBak = ISO_MODE_POS_ON_NEG_OFF;
                            gby_ChangeModeDelayFlag = 1;
                            /*  if( (( SUMV_RATIO_10mV == (SensorCfg.ratio_cfg & SUMV_RATIO_MASK))&&
                              (lwd_sumv < (lwd_v_mode + (lwd_sumv/ISO_MODE1_THRED2_GAIN)) ) ) ||
                              (( SUMV_RATIO_100mV == (SensorCfg.ratio_cfg & SUMV_RATIO_MASK))&&
                              (lwd_sumv < (lwd_v_mode + (lwd_sumv/ISO_MODE1_THRED2_GAIN)) ) )  ) */
                            if (lwd_sumv < (lwd_v_mode + (lwd_sumv/ISO_MODE1_THRED2_GAIN)))
                            {
                                Iso_FlagPtr ++;
                                *Iso_FlagPtr = FLAG_ISO_ERROR;
                            }
                        }
                        else
                        {
                            /*
                            ECU_DELAYMS(1);
                            st = IsoIf_ChangeMosMode(Iso_Id,ISO_MODE_POS_ON_NEG_ON); // continue to check asymmetric fault
                            if(st != ERR_OK)
                            {
                                return st;
                            }*/
                            gby_ChangeModeBak = ISO_MODE_POS_ON_NEG_ON;
                            gby_ChangeModeDelayFlag = 1;
                        }

                    }

                }
                else
                {
                    gby_ChangeModeDelayFlag = 0;
                    st = IsoIf_ChangeMosMode(Iso_Id,gby_ChangeModeBak); // continue to check asymmetric fault
                    if(st != ERR_OK)
                    {
                        return st;
                    }
                    gwd_ChangeModeWaitCnt = 0;
                }
                
                st = FLAG_ISO_DONE_ONCE;
            }
            // #endif//end of #if(0)
            glwd_iso_rslt_pos[0] = 0;
            glwd_iso_rslt_pos[1] = 0;
            glwd_iso_rslt_pos[2] = 0;

            glwd_iso_rslt_neg[0] = 0;
            glwd_iso_rslt_neg[1] = 0;
            glwd_iso_rslt_neg[2] = 0;

            glwd_sumv_acc = 0;

            
        }
        else
        {
            st = ERR_OK ;
        }
    }
    return st;
}

// sumv resolution must to  be 0.1V.
GeneralReturnType IsoDet_ExtMode2Judge(eIso_IdNumType Iso_Id,uByte* Iso_FlagPtr, uWord* Iso_RpPtr, uWord* Iso_RnPtr, uWord Sumv)
{
    GeneralReturnType st;
    uWord  lwd_iso_RP;
    uWord  lwd_iso_RN;

    uWord v_pos = 0, v_neg = 0, lwd_sumv;

    uLWord temp1,temp2;
    st = (GeneralReturnType)ERR_OK;

    lwd_sumv = Sumv;

    //read the value of v_pos and v_neg
    st = IsoIf_RdAdValue(Iso_Id,&v_pos, &v_neg);

    //check the value and calculate
    if (st != ERR_OK)
    {
        return st;
    }
    else
    {
        if (ISO_MODE_POS_ON_NEG_ON == gby_flag_iso_mode)      // pos and neg mos all ON
        {
            glwd_iso_rslt_pos[0] += v_pos;
            glwd_iso_rslt_neg[0] += v_neg;
        }
        else if (ISO_MODE_POS_ON_NEG_OFF == gby_flag_iso_mode)// pos is ON, while neg is OFF
        {
            glwd_iso_rslt_pos[1] += v_pos;
            glwd_iso_rslt_neg[1] += v_neg;
        }
        else if (ISO_MODE_POS_OFF_NEG_ON == gby_flag_iso_mode)// pos is OFF, while neg is ON
        {
            glwd_iso_rslt_pos[2] += v_pos;
            glwd_iso_rslt_neg[2] += v_neg;
        }

        glwd_sumv_acc += lwd_sumv;

        gby_flg_iso_cnt++;

        if (gby_flg_iso_cnt >= RERULT_NUM)
        {
            gby_flg_iso_cnt = 0;

            /* clear flag firstly */
            *Iso_FlagPtr = FLAG_ISO_NORMAL;
            if (gby_ErrLevelNum > 1)
            {
                *(Iso_FlagPtr+1) = FLAG_ISO_NORMAL;
            }

            if (ISO_MODE_POS_ON_NEG_ON ==gby_flag_iso_mode)
            {
                st = IsoIf_CloseMos(Iso_Id,OPT_MOS_POS);
                if(st != ERR_OK)
                {
                    return st;
                }
                st = IsoIf_OpenMos(Iso_Id,OPT_MOS_NEG);
                if(st != ERR_OK)
                {
                    return st;
                }
                gby_flag_mos_pos = FLAG_CLOSED;
                gby_flag_mos_neg = FLAG_OPEN;

                gby_flag_iso_mode = ISO_MODE_POS_ON_NEG_OFF;

            }
            else if(ISO_MODE_POS_ON_NEG_OFF == gby_flag_iso_mode)
            {
                glwd_iso_rslt_pos[1] = glwd_iso_rslt_pos[1]/RERULT_NUM;
                glwd_iso_rslt_pos[1] = glwd_iso_rslt_pos[1]*POS_GAIN;
                glwd_iso_rslt_pos[1] = glwd_iso_rslt_pos[1]/4095;

                gwd_u_mode_POS_ON = (uWord)glwd_iso_rslt_pos[1];  //0.1V

                gwd_sumv_POS_ON = (uWord)(glwd_sumv_acc/RERULT_NUM);

                st = IsoIf_OpenMos(Iso_Id,OPT_MOS_POS);
                if(st != ERR_OK)
                {
                    return st;
                }

                st = IsoIf_CloseMos(Iso_Id,OPT_MOS_NEG);
                if(st != ERR_OK)
                {
                    return st;
                }

                gby_flag_mos_pos = FLAG_OPEN;
                gby_flag_mos_neg = FLAG_CLOSED;

                gby_flag_iso_mode = ISO_MODE_POS_OFF_NEG_ON;

            }
            else if(ISO_MODE_POS_OFF_NEG_ON == gby_flag_iso_mode)
            {
                glwd_iso_rslt_neg[2] = glwd_iso_rslt_neg[2]/RERULT_NUM;
                glwd_iso_rslt_neg[2] = glwd_iso_rslt_neg[2]*NEG_GAIN;
                glwd_iso_rslt_neg[2] = glwd_iso_rslt_neg[2]/4095;

                gwd_u_mode_NEG_ON = (uWord)glwd_iso_rslt_neg[2];

                gwd_sumv_NEG_ON = (uWord)(glwd_sumv_acc/RERULT_NUM);

                st = IsoIf_CloseMos(Iso_Id,OPT_MOS_POS);
                if(st != ERR_OK)
                {
                    return st;
                }

                st = IsoIf_OpenMos(Iso_Id,OPT_MOS_NEG);
                if(st != ERR_OK)
                {
                    return st;
                }

                gby_flag_mos_pos = FLAG_CLOSED;
                gby_flag_mos_neg = FLAG_OPEN;

                gby_flag_iso_mode = ISO_MODE_POS_ON_NEG_OFF;

                temp1 = gwd_sumv_POS_ON;  //U1
                temp1 *= gwd_sumv_NEG_ON; //U1'

                temp2 = gwd_sumv_POS_ON;  //U1
                temp2 *= gwd_u_mode_NEG_ON;//U3

                temp1 = temp1 - temp2;


                temp2 = gwd_sumv_NEG_ON;  //U1'
                temp2 *= gwd_u_mode_POS_ON; //U2

                temp1 = temp1 - temp2;

                temp1 /= 100;
                temp2 /= 100;

                if (temp2 > 0)
                {
                    temp1 *= R_BRANCH;
                    temp1 /= temp2;

                    lwd_iso_RN = (uWord)temp1;
                }
                else
                {
                    lwd_iso_RN = 10000;   // 10M
                }

                temp1 = gwd_sumv_POS_ON;  //U1
                temp1 *= gwd_sumv_NEG_ON; //U1'

                temp2 = gwd_sumv_NEG_ON;  //U1'
                temp2 *= gwd_u_mode_POS_ON;//U2

                temp1 = temp1 - temp2;


                temp2 = gwd_sumv_POS_ON;  //U1
                temp2 *= gwd_u_mode_NEG_ON; //U3

                temp1 = temp1 - temp2;

                temp1 /= 100;
                temp2 /= 100;

                if (temp2 > 0)
                {
                    temp1 *= R_BRANCH;
                    temp1 /= temp2;

                    lwd_iso_RP = (uWord)temp1;
                }
                else
                {
                    lwd_iso_RP = 10000;   // 10M
                }

                * Iso_RpPtr = lwd_iso_RP; //resoult: 1K

                * Iso_RnPtr = lwd_iso_RN; //resoult: 1K

                temp1 = lwd_iso_RP;
                temp1 *= 1000;       //resoult: 1��
                temp1 /= gwd_sumv_POS_ON; // unit: 1��/0.1V
                temp1 *= 10;         // unit: 1��/0.1V

                temp2 = lwd_iso_RN;
                temp2 *= 1000;       //resoult: 1��
                temp2 /= gwd_sumv_NEG_ON; // unit: 1��/0.1V
                temp2 *= 10;         // unit: 1��/0.1V

                if (gwd_sumv_POS_ON > ISO_CHECK_SUMV_THRD)
                {
                    if ( (temp1 < ISO_FAULT_LEVEL1) || (temp2 < ISO_FAULT_LEVEL1) )
                    {
                        *Iso_FlagPtr = FLAG_ISO_ERROR;

                        if (gby_ErrLevelNum > 1)
                        {
                            if ( (temp1 < ISO_FAULT_LEVEL2) || (temp2 < ISO_FAULT_LEVEL2) )
                            {
                                Iso_FlagPtr++;
                                *Iso_FlagPtr = FLAG_ISO_ERROR;
                            }
                        }
                    }
                }
            }

            glwd_iso_rslt_pos[0] = 0;
            glwd_iso_rslt_pos[1] = 0;
            glwd_iso_rslt_pos[2] = 0;

            glwd_iso_rslt_neg[0] = 0;
            glwd_iso_rslt_neg[1] = 0;
            glwd_iso_rslt_neg[2] = 0;

            glwd_sumv_acc = 0;

            st = FLAG_ISO_DONE_ONCE;
        }
    }
    return st;
}

void IsoDet_Init(void)
{
    IsoDet_TmCnt.cnt = 0;

    gby_ErrcntIso500 = 0;
    gby_ErrcntIso100 = 0;

    (void)IsoIf_OpenMos(ISO1,OPT_MOS_POS);
    (void)IsoIf_OpenMos(ISO1,OPT_MOS_NEG);

//    (void)IsoIf_Init();
}

void IsoDet_MainCountCbk(void)
{
    (IsoDet_TmCnt.cnt < 255) ? (IsoDet_TmCnt.cnt++) : (IsoDet_TmCnt.cnt = 255);
}

void IsoDet_MainCbk(void)
{
    GeneralReturnType st;
    // uByte Iso_Flag[2];
    uWord lwd_Sumv;

    if(IsoDet_TmCnt.cnt >= TM_MS(10))
    {
        IsoDet_TmCnt.cnt = 0;

        if(FLAG_ENABLE == IsoDet_PrjConf.DetEn)
        {

            // power on delay 200mS Close MOS-FET
            IsoDet_TmCnt.delay_timecnt++;
            if(IsoDet_TmCnt.delay_timecnt >= 20)
            {
                IsoDet_TmCnt.delay_timecnt = 20 ;
                if(0 == gby_InitFlag )
                {
                    gby_InitFlag = 1;
                    (void)IsoIf_Init();
                    return;
                }
            }
            else
            {
                return;
            }

            lwd_Sumv = Bat_GetSumV(INT_SUMV);
            st = IsoDet_ExtMode1Judge(ISO1,gby_IsoFlagTst,lwd_Sumv);
            //st = IsoDet_Judge(ISO1,gby_IsoFlagTst,lwd_Sumv);

            if((st != ERR_OK) && (st != FLAG_ISO_DONE_ONCE))
            {
                (void)Err_UpdateHwErrDetail(ERR_HW_DETECT_ISO,1);
            }
            else
            {
                (void)Err_UpdateHwErrDetail(ERR_HW_DETECT_ISO,0);
            }
            if(FLAG_ISO_DONE_ONCE == st)
            {
                if (FLAG_ISO_ERROR == gby_IsoFlagTst[0])
                {
                    gby_ErrcntIso500++;
                    if (gby_ErrcntIso500 >= ERR_ISO_LEVEL2_CNT)
                    {
                        (void)Err_UpdateBatSysErrLevel(ERR_BATSYS_ISO_ERR,ERR_LEVEL_TWO);
                        
                        if(POS_SIDE_ERR == gby_iso_location)
                        {
                            (void)Err_UpdateBatSysErrLevel(ERR_BATSYS_ISO_POS,ERR_LEVEL_TWO);    
                        }
                        else if(NEG_SIDE_ERR == gby_iso_location)
                        {
                            (void)Err_UpdateBatSysErrLevel(ERR_BATSYS_ISO_NEG,ERR_LEVEL_TWO);
                        }
                        else
                        {
                            (void)Err_UpdateBatSysErrLevel(ERR_BATSYS_ISO_MID,ERR_LEVEL_TWO);
                        }
                        gby_ErrcntIso500 = ERR_ISO_LEVEL2_CNT;
                    }

                    if (FLAG_ISO_ERROR == gby_IsoFlagTst[1])
                    {
                        gby_ErrcntIso100++;

                        if (gby_ErrcntIso100 >= ERR_ISO_LEVEL3_CNT)
                        {
                            (void)Err_UpdateBatSysErrLevel(ERR_BATSYS_ISO_ERR,ERR_LEVEL_THREE);

                            if(POS_SIDE_ERR == gby_iso_location)
                            {
                                (void)Err_UpdateBatSysErrLevel(ERR_BATSYS_ISO_POS,ERR_LEVEL_THREE);    
                            }
                            else if(NEG_SIDE_ERR == gby_iso_location)
                            {
                                (void)Err_UpdateBatSysErrLevel(ERR_BATSYS_ISO_NEG,ERR_LEVEL_THREE);
                            }
                            else
                            {
                                (void)Err_UpdateBatSysErrLevel(ERR_BATSYS_ISO_MID,ERR_LEVEL_THREE);
                            }                            
                            gby_ErrcntIso100 = ERR_ISO_LEVEL3_CNT;
                        }
                    }
                    else
                    {
                        gby_ErrcntIso100 = 0;
                    }
                }
                else                 
                {
                    (void)Err_UpdateBatSysErrLevel(ERR_BATSYS_ISO_ERR,ERR_LEVEL_NORMAL);
                    
                    (void)Err_UpdateBatSysErrLevel(ERR_BATSYS_ISO_POS,ERR_LEVEL_NORMAL);
                    (void)Err_UpdateBatSysErrLevel(ERR_BATSYS_ISO_NEG,ERR_LEVEL_NORMAL);
                    (void)Err_UpdateBatSysErrLevel(ERR_BATSYS_ISO_MID,ERR_LEVEL_NORMAL);
                    gby_ErrcntIso500 = 0;
                    gby_ErrcntIso100 = 0;
                }
            }
        }
    }
}
