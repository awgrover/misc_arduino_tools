// test

#include <awg_combinators.h>
#include <array_size.h>

#include <AccelStepper.h>
// if you want debug output:
#define DEBUG
#include <AccelStepperMotorShield.h>

Adafruit_MotorShield MotorShield1 = Adafruit_MotorShield(0x61); // shield #1
// Make it just like a AccelStepper
AccelStepperMotorShield stepper1_1 = AccelStepperMotorShield(
                                       MotorShield1,
                                       1, // stepper block #1
                                       SINGLE // optional/default vs MICROSTEP, the stepperBlock.step-style
                                     );
AccelStepperMotorShield stepper1_2 = AccelStepperMotorShield(
                                       MotorShield1,
                                       2, // stepper block #2
                                       SINGLE // optional/default, the stepperBlock.step-style
                                     );

struct stepper_entry {
  AccelStepperMotorShield *stepper;
  int pattern_i;
  Timer *delaying;
};
stepper_entry steppers[] = { {&stepper1_1}, {&stepper1_2} };

struct BasePattern {
  virtual boolean is_done(stepper_entry &entry) = 0;
  virtual void start( stepper_entry &entry  ) = 0;
  virtual void print() = 0;
};
struct move_stepper : BasePattern {
  long pos;
  move_stepper(long pos) : pos(pos) { }
  void start( stepper_entry &entry ) {
    entry.stepper->moveTo(pos);
  }
  boolean is_done(stepper_entry &entry) {
    return ! entry.stepper->run();
  }
  void print() {
    Serial << "moveto " << pos;
  }
};
struct delaying : BasePattern {
  int msec;
  delaying(int msec) : msec(msec) {}
  void start( stepper_entry &entry ) {
    if (!entry.delaying) entry.delaying = new Timer(msec);
    entry.delaying->reset(msec);
  }
  boolean is_done(stepper_entry &entry) {
    return (*(entry.delaying))();
  }
  void print() {
    Serial << "delaying " << msec;
  }
};
BasePattern* pattern[] = { new move_stepper(50), new delaying(200), new move_stepper(-50), new delaying(200) };
int pattern_length = array_size( pattern );

void setup() {
  Serial.begin(115200);
  Serial << "BEGIN\n";
  for (stepper_entry &entry : steppers) {
    Serial << "Stepper begin " << ((long) &entry) << endl;
    entry.stepper->begin();
    entry.stepper->setAcceleration(1e8);
    entry.stepper->setMaxSpeed(300);
    pattern[ 0 ]->start(entry);
  }
}


void loop() {
  for (stepper_entry &entry : steppers) {
    //entry.stepper->run();

    BasePattern *entry_pattern = pattern[ entry.pattern_i ];


    if ( entry_pattern->is_done(entry) ) {
      entry.pattern_i = (entry.pattern_i + 1) % pattern_length;
      pattern[ entry.pattern_i ]->start(entry);
      Serial << millis() << " Stepper " << ((long) &entry) << " do ["
             << entry.pattern_i << "] ";
      pattern[ entry.pattern_i ]->print();
      Serial << endl;
    }
  }
}
