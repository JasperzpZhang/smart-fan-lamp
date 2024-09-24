# -*- coding: UTF8 -*-

import time
import serial

PwrId = "001"

def ComOpen(comName):
    comPort = serial.Serial(comName)
    comPort.baudrate = 115200
    comPort.parity = serial.PARITY_NONE
    comPort.stopbits = serial.STOPBITS_ONE
    comPort.bytesize = serial.EIGHTBITS
    comPort.timeout = 0
    return comPort

def ComClose(comPort):
    comPort.close()

def CliExec(comPort, strCmd):
    print(strCmd)
    strSet = strCmd + "\r\n"
    datas = strSet.encode("gbk")
    for n in range(len(datas)):
        comPort.write([datas[n]])
        time.sleep(0.010)
    time.sleep(0.15)
    strResp = comPort.read(1024).decode('gbk')
    return strResp

def ConfigInfo(comPort,pwrType,realTime):
    strResp = ""
    if (pwrType == 1):
        strResp += CliExec(comPort, "cfg_set_max_cur 34")
        strResp += CliExec(comPort, "cfg_set_work_cur 31")
        strResp += CliExec(comPort, "cfg_set_adc_vol_para 5.8")
    elif (pwrType == 2):
        strResp += CliExec(comPort, "cfg_set_max_cur 45")
        strResp += CliExec(comPort, "cfg_set_work_cur 43.5")
        strResp += CliExec(comPort, "cfg_set_adc_vol_para 8.2")
    
    strResp += CliExec(comPort, f"cfg_set_pwr_id {PwrId}")
    strResp += CliExec(comPort, "cfg_set_comp_rate 0")
    strResp += CliExec(comPort, "cfg_set_pd_warn_l1 1300")
    strResp += CliExec(comPort, "cfg_set_pd_warn_l2 200")
    
    strResp += CliExec(comPort, f"cfg_set_pwr_type {pwrType}")
    strResp += CliExec(comPort, "cfg_set_temp_chan_num 6")
    strResp += CliExec(comPort, f"rtc_set {realTime}")
    strResp += CliExec(comPort, "cfg_set_trial_en 1") 
    strResp += CliExec(comPort, "cfg_set_trial_info 0 45")
    strResp += CliExec(comPort, "cfg_set_sys_debug 1")
    strResp += CliExec(comPort, "enable_manual_ctrl 1")
    
    strResp += CliExec(comPort, "cfg_set_mod_en 0  1")  # MANUAL
    strResp += CliExec(comPort, "cfg_set_mod_en 1  1")  # QBH
    strResp += CliExec(comPort, "cfg_set_mod_en 2  1")  # PD
    strResp += CliExec(comPort, "cfg_set_mod_en 3  1")  # TEMP1
    strResp += CliExec(comPort, "cfg_set_mod_en 4  1")  # TEMP2
    strResp += CliExec(comPort, "cfg_set_mod_en 5  1")  # CHAN1_CUR
    strResp += CliExec(comPort, "cfg_set_mod_en 6  1")  # CHAN2_CUR
    strResp += CliExec(comPort, "cfg_set_mod_en 7  1")  # CHAN3_CUR
    strResp += CliExec(comPort, "cfg_set_mod_en 8  1")  # WATER_PRESS
    strResp += CliExec(comPort, "cfg_set_mod_en 9  1")  # WATER_CHILLER
    strResp += CliExec(comPort, "cfg_set_mod_en 10 1")  # CHAN_CTRL

    strResp += CliExec(comPort, "cfg_set_cvs 1") 
    strResp += CliExec(comPort, "cfg_set_ccs 0") 
    
    strResp += CliExec(comPort, "cfg_set_aim_pwm_duty 680")
    strResp += CliExec(comPort, "cfg_set_a_pwm_duty 50")

    strResp += CliExec(comPort, "cfg_set_aim_mutex 1")

    strResp += CliExec(comPort, "cfg_show")
    return strResp

if __name__ == '__main__': 
    comName = 'COM12'
    comName = input("Please enter the COM port: ")
    pwrType = 2
    #pwrType = int(input("9KW ---> 1\r\n18KW ---> 2\r\nPlease enter the power type: "))
    realTime = time.strftime("%Y %m %d %H %M %S", time.localtime())
    # PwrId = input("please enter the PwrId: ")

    while True:
        try:
            comPort = ComOpen(comName)
            strResp = ""
            strResp += CliExec(comPort, "encli admin 1")
            strResp += ConfigInfo(comPort,pwrType,realTime)
            print("\r\nConfiguration parameters:")
            print(strResp)
        except Exception as e:
            print(f"An exception occurred: {e}")
        finally:
            ComClose(comPort)
        
        c = input("Enter 'e' to exit the current program, else repeat the current configuration!\n")
        if c.lower() == "e":
            break
        else:
            print("Reconfiguring")
