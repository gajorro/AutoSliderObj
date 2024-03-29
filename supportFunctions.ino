// Support functions.

// This is the configuration function for the three dSPIN parts. Read the inline
//  comments for more info.
void dSPINConfig(int acc, int myspeed, int kval)
{

  //pinMode(10, OUTPUT); 
  boardA.resetDev();
  //Serial.println(boardA.getParam(CONFIG));
 if (boardA.getParam(CONFIG) == 0x2E98) 
  //Serial.println("start");
   if (boardA.getParam(CONFIG) == 0x2E88) 
  //Serial.println("start88");
  boardA.configSyncPin(SYNC_PIN, SYNC_FS);// BUSY pin low during operations;
  boardA.setParam(STEP_MODE, STEP_SEL_1_128);
  boardA.setMaxSpeed(myspeed);        // 10000 steps/s max
  //boardA.setMinSpeed(1);
  boardA.setFullSpeed(400);       // microstep below 10000 steps/s
  boardA.setAcc(acc);             // accelerate at 10000 steps/s/s
  boardA.setDec(acc);
  boardA.setSlewRate(SR_530V_us);   // Upping the edge speed increases torque.
  boardA.setOCThreshold(OC_4500mA);  // OC threshold 750mA
  boardA.setPWMFreq(PWM_DIV_1, PWM_MUL_2); // 31.25kHz PWM freq
  boardA.setOCShutdown(OC_SD_ENABLE); // don't shutdown on OC
  boardA.setVoltageComp(VS_COMP_ENABLE); // don't compensate for motor V
  boardA.setSwitchMode(SW_USER);    // Switch is not hard stop
  boardA.setOscMode(INT_16MHZ_OSCOUT_16MHZ); // for boardA, we want 16MHz
                                    //  internal osc, 16MHz out. boardB and
                                    //  boardC will be the same in all respects
                                    //  but this, as they will bring in and
                                    //  output the clock to keep them
                                    //  all in phase.
  boardA.setAccKVAL(kval);           // We'll tinker with these later, if needed.
  boardA.setDecKVAL(kval);
  boardA.setRunKVAL(kval);
  boardA.setHoldKVAL(16);           // This controls the holding current; keep it low.
  //boardA.setLoSpdOpt(true);

}
void setBasicParams(int acc, int myspeed, int kval)
{
 if (boardA.getParam(CONFIG) == 0x2E98) 
  //Serial.println("start");
   if (boardA.getParam(CONFIG) == 0x2E88) 
  //Serial.println("start88");
  boardA.configSyncPin(SYNC_PIN, SYNC_FS);// BUSY pin low during operations;
  boardA.setParam(STEP_MODE, STEP_SEL_1_128);
  //boardA.setMinSpeed(1);
  boardA.setMaxSpeed(myspeed);        // 10000 steps/s max
  boardA.setFullSpeed(400);       // microstep below 10000 steps/s
  boardA.setAcc(acc);             // accelerate at 10000 steps/s/s
  boardA.setDec(acc);
  boardA.setSlewRate(SR_530V_us);   // Upping the edge speed increases torque.
  boardA.setOCThreshold(OC_4500mA);  // OC threshold 750mA
  boardA.setPWMFreq(PWM_DIV_1, PWM_MUL_2); // 31.25kHz PWM freq
  boardA.setOCShutdown(OC_SD_ENABLE); // don't shutdown on OC
  boardA.setVoltageComp(VS_COMP_ENABLE); // don't compensate for motor V
  boardA.setSwitchMode(SW_USER);    // Switch is not hard stop
  boardA.setOscMode(INT_16MHZ_OSCOUT_16MHZ); // for boardA, we want 16MHz
                                    //  internal osc, 16MHz out. boardB and
                                    //  boardC will be the same in all respects
                                    //  but this, as they will bring in and
                                    //  output the clock to keep them
                                    //  all in phase.
  boardA.setAccKVAL(kval);           // We'll tinker with these later, if needed.
  boardA.setDecKVAL(kval);
  boardA.setRunKVAL(kval);
  boardA.setHoldKVAL(16);           // This controls the holding current; keep it low.
  //boardA.setLoSpdOpt(true);
}
