#pragma once

/*
  Every will tell you _at_ every n millis().
  Timer will tell you (once) when n millis() has gone by.

  * Every n millis
  
    static Every t1(100);  // every 100 msec
    
    if ( t1() ) { do it; }

    * Details
      The "every" objet has to be static/global, obviously because it needs to remember the "last" time.
      
      The object + "()" is magic: returns a boolean meaning "expired?" (and restarts for the next interval.

      The initial event does not happen immediately, it happens in n msec. If you want an immediate first
      event, supply "true" for the "now" argument in the constructors: Every(100, true). 

      The interval  can be up to 2^32 msecs (full range of millis()). Sadly, that's 4 bytes.

      Takes 8 bytes of RAM for "Every" object.
      
      Resists drift by "re-aligning" when it detects that the interval has expired. E.g. if it should
      happen every 100 msec, but you don't test till 30msec late (i.e. at 130msec), it will fire, and
      re-align to fire at 200msec. Thus, it's not an interval, it's "_on_ every n msec". For small
      amounts of drift, this is probably nice. For larger amounts, might be confusing (see Timer). 

      There's no suspend/stop. That would add at least 1 boolean more memory!
      
  * Toggle every n millis

    // "blink" example using EveryToggle
    EveryToggle t1(200); // adds the .state() method
    void setup() { pinMode(LED_BUILTIN, OUTPUT); }

    void loop() {
      if ( t1() ) {
        digitalWrite( LED_BUILTIN, t1.state ); // state goes true,false,true,false...
        }
    }

  * Count from 0..n-1, 0..n-1, etc
    static EveryCount(100, 10); // 0..9
    
    if ( t1() ) { Serial.println( t1.state ); } // prints 0,1,2..9, with delay of 100 between

  * Cycle through a sequence
    const char abcd[] = { 'a', 'b', 'c', 'd' }; // the sequence
    // adds the .sequence() method
    static EverySequence t1(150, 4,abcd); // have to say "4" to say how long the sequence is

    // Prints a,b,c,d,a,b,c,d, with a delay of 100 between
    if ( t1() ) { Serial.println( t1.sequence() ) };

  * Use lambda, or a function, or functor
 
    boolean happened = t1( &doit } );
    boolean happened = t1( someobject } ); // if it has a: void operator()()
    boolean happened = t1( []() { do it; } ); // "inline" function

    * Details
      You can't use a lambda that has a capture. But, you can refer to global/static objects.

      functions/ambdas/functors will be called with no arguments.
      
  * Resetting
    if (tmetoreset) t1.reset(); // next in 100 msec from this call
    // t1.reset(true); // the next t1() will be true ("immediate")

  * Changing interval
 
    t1.interval = 1000; // I lied, change it

  * How long ago was last
    ...
    millis() - t1.last;
    
  * Timer
    static Timer t1(100);  // once, in 100 msec
    if ( t1() ) { do it; }

    // using lambda
    boolean happened = t1( []() { do it; } ); 





  2. every n, every {n, n2, n3}
  next(n) // change n
  toggle -> 3 state: nothing, on, off
    lambdas?
  3. subclass it, mixin, ...

  A timeline: n1,n2,n3 => event1,event2,event3
  Special case of pred1,pred2,... => event1,event2
*/

#include <Streaming.h>
#define DEBUG Serial << '[' << millis() << "] "


class Every {
  public:
    // everthing public
    unsigned long last; // last time we fired
    unsigned long n_msec; // "delay" till next firing

    Every(uint16_t n_msec, bool now = false) : n_msec(n_msec) {
      last = millis(); // so, would wait for n_msec

      if (now) {
        last -= n_msec; // adjust to "already expired"
      }
    }

    virtual boolean operator()() {
      // lots of this class means lots of calls to millis()
      unsigned long now = millis(); // minimize drift due to this fn
      unsigned long diff = now - last;
      
      if (diff >= n_msec) {
        unsigned long drift = diff % n_msec;
        //Serial << "drift " << last << " now " << now << " d: " << drift << endl;
        last = now;
        last -= drift;
        return true;
      }
      else {
        return false;
      }
    }
 
    template <typename T>
    boolean operator()(T lambdaF ) {
      // simple lambda: []() { do something };
      boolean hit = (*this)();
      if (hit) lambdaF();
      return hit;
    }

    virtual void reset(boolean now=false) {
      last = millis();
      if (now) last -= n_msec;
    }
    void reset(unsigned long interval, boolean now=false) { n_msec=interval; reset(now); }
    
};

class EveryCount : public Every { // 0..n-1
  public:
   int count;
   int state = -1; // because if(every()) will +1 befor you get sequence
    EveryCount(unsigned int n_msec, int count, bool now = false)
      : Every{n_msec, now}, count(count) {}

    
   using Every::operator(); // in every subclass if you add a ()
   
   boolean operator()() {
      if (Every::operator()()) {
        state = ( state + 1 ) % count;
        return true;
      }
      return false;
    }

    
    /*
     * Yikes, don't know how to receive a lambda with args, the base class's matches before we do
    // lambda will get the state!
    template <typename T>
    boolean operator()(T lambdaF(const bool state) ) {
      // return value is ignored from the lambda
      boolean hit = (*this)();
      if (hit) (*lambdaF)(this->state);
      return hit;
    }
    */

};

class EveryToggle : public Every { // not really a ...Sequence
  public:

   boolean state = false; // because if(every()) will ! befor you get sequence
    EveryToggle(unsigned int n_msec, bool now = false)
      : Every{n_msec, now} {}

    
   using Every::operator(); // in every subclass if you add a ()
   
   boolean operator()() {
      if (Every::operator()()) {
        state = !state;
        return true;
      }
      return false;
    }

    
    /*
     * Yikes, don't know how to receive a lambda with args, the base class's matches before we do
    // lambda will get the state!
    template <typename T>
    boolean operator()(T lambdaF(const bool state) ) {
      // return value is ignored from the lambda
      boolean hit = (*this)();
      if (hit) (*lambdaF)(this->state);
      return hit;
    }
    */

};

template <typename T>
class Every2Sequence : public Every {
    // has sequence -> ....
  public:
    int seq_count;
    const T *_sequence;
    unsigned int sequence_i = -1; // because if(every()) will increment before you get sequence()

    constexpr static int _toggle[] = {0, 1};

    // captures the sequence!
    Every2Sequence(unsigned int n_msec, const int seq_count, const T sequence[], bool now = false)
      : Every{n_msec, now}, seq_count(seq_count), _sequence(sequence)
    {}

    // for toggles, it's just:
    Every2Sequence(unsigned int n_msec, bool now = false)
      : Every{n_msec, now}, seq_count(2), _sequence(_toggle)
    {}

    T sequence() {
      return _sequence[sequence_i];
    }

    boolean operator()() {
      boolean hit = Every::operator()();
      //DEBUG << "test " << hit << endl;
      if (hit) {
        sequence_i = (sequence_i + 1) % seq_count;
      }
      return hit;
    }
};
