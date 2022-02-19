#pragma once

class BeginRun {
    // protocol for .begin() and .run() on a system

  public:
    virtual void begin() = 0; // FIXME: should probably be return-T|F, and you should fail on F
    virtual boolean run() = 0; // should return true if you are "running"
    virtual void finish_loop() {}; // default to nothing at end of loop

    class Noop; // the noop version, subclass of BeginRun
};

class BeginRun::Noop : public BeginRun {
    // because we can't have a zero-length array
  public:
    virtual void begin() {};
    virtual boolean run() {
      delay(20);  // so integrated usb can respond to upload
      return false;
    };
};
