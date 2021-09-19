#pragma once

/*
  Subcalss AccelStepper to use MotorShield instead of pins:
  Then this works just like a AccelStepper and can be substituted for it.

  All the book-keeping that Adafruit_MotorShield/Adafruit_StepperMotor does
  is wasted for this application:
  A "raw" one-step would be much more efficient.

  Usage:
  Use it like AccelStepper,
    But, do call .begin()
    Change the stepperBlock step_style with .step_style = SINGLE|DOUBLE|INTERLEAVE|MICROSTEP
    Assign .steps_per_revolution if you use:
      to-be-implemented.
    Implements .disableOutputs() as stepperBlock->release()
    Get the Adafruit_MotorShield as .shield
    Get the Adafruit_StepperMotor as ->stepperBlock

  #include <AccelStepper.h>
  // if you want debug output:
  #define DEBUG
  #include <AccelStepperMotorShield.h>

  Adafruit_MotorShield MotorShield1 = Adafruit_MotorShield(); // shield #1
  // Make it just like a AccelStepper
  AccelStepperMotorShield stepper1_2 = AccelStepperMotorShield(
                                    MotorShield1,
                                    2, // stepper block #2
                                    SINGLE // optional/default, the stepperBlock.step-style
                                  );

  void setup() {
    stepper1_2.begin();
    stepper1_2.step_style = MICROSTEP;
    stepper1_2.steps_per_revolution = 20;
    stepper1_2.disableOutputs(); same as .stepperBlock->release()
    stepper1_2.stepperBlock->any Adafruit_StepperMotor method
    stepper1_2.shield.any Adafruit_MotorShield method
    }
*/

#include <Adafruit_MotorShield.h>
#include <AccelStepper.h> //Stepper motor Library

#ifndef DEBUG
  #define DEBUG 0
#else
  // a weird way of saying: defined, but no explicit value
  #define DEBUG 1
#endif

class AccelStepperMotorShield : public AccelStepper {
    static void dumy() {} // nop

  public:
    Adafruit_StepperMotor *stepperBlock;
    Adafruit_MotorShield &shield;
    int step_style; // e.g. SINGLE MICROSTEP
    unsigned short steps_per_revolution; // the stepperBlock's isn't accessible
    #ifdef DEBUG
    int stepperBlock_i; // because actual stepperBlock_i is private. thanks.
    #endif

    AccelStepperMotorShield(Adafruit_MotorShield &shield, const int stepperBlock, const int step_style = SINGLE, const int steps = 200)
      : shield(shield),
        step_style(step_style),
        stepperBlock(shield.getStepper(200, stepperBlock)), // step/rev is not relevant
        steps_per_revolution(steps),
        // we can't use .forward and .backward
        // because we can't construct a function pointer for them
        // because it would have to refer to this->stepperBlock
        // but we want as-if forward/backward behavior (aka "custom"):
        AccelStepper(&dumy, &dumy) // signal "doing custom step()"
        #ifdef DEBUG
        ,stepperBlock_i(stepperBlock)
        #endif
    {} // can't call shield.begin() at constructor time

    void begin() {
      if (DEBUG) {
        Serial.print(F("ASMS Begin shield &")); Serial.print((long) &shield);
        Serial.print(F(" block "));Serial.print(stepperBlock_i);
        Serial.println(F(" steps/rev"));
      }
      shield.begin();
    }

    void set_step_style(int step_style) {
      // update it
      this->step_style = step_style;
    }

    // RPS methods (revolutions per second)
    void setMaxRPS(const float rpm) {
      setMaxSpeed( steps_per_revolution * rpm);
    }
    void setRPS(const float rpm) {
      setSpeed( steps_per_revolution * rpm);
    }
    void moveRevolutions(const float rev) { 
      if (DEBUG) { Serial.println(F("ASMS +/- revs "));Serial.println(rev); }
      move(steps_per_revolution * rev); 
    }
    void moveToRevolution(const float rev) { 
      if (DEBUG) { Serial.println(F("ASMS to rev "));Serial.println(rev); }
      moveTo(steps_per_revolution * rev); 
    }
    
    // The overrides
    void step(long step) {
      // we know we are only doing forward/backward, so just do it
      if (speed() > 0) stepperBlock->onestep(FORWARD, step_style);
      else stepperBlock->onestep(BACKWARD, step_style);
    }

    void disableOutputs() {
      // This doesn't work for me
      if (DEBUG) Serial.println(F("ASMS disable-outputs"));
      stepperBlock->release();  // translates to "free spin"
    }
    
};
