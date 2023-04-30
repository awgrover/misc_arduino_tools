#pragma once
#include <limits.h>

/* Composable parsing classes, like parsing combinators

Usage:
  #include "Parsing.h"

  // e.g. top-level sequence of things
  static boolean do_print_help = false;
  Parsing::PrintHelp print_help( &do_print_help );
  static Parsing::BaseClass * const that_seq[] = { new Parsing::x,..., &print_help };
  static parse_that_seq Parsing::Sequence(F("$shortdesc $arg/$pattern"), that_seq, array_size(that_seq) )

    // See classes below for nesting, e.g.
    // Parsing::Alternate( "why", array-alts, size )
    // Parsing::Sequence( "why", array-seq, size )
    // e.g.
      static Parsing::BaseClass * const go_sequence[] = {
        new Parsing::Char(F("start G"), 'G'),
        new Parsing::Space(),
        new Parsing::Entier<unsigned int>( F("motor"), motor_command.motor_i, 999, ' ' ),
        new Parsing::Entier<unsigned int>( F("hz<"), motor_command.hz_e, 99999, '.' ),
        new Parsing::Decimal( F("hz>"), motor_command.hz_d, 0.0001, ' ' ),
        new Parsing::Alternate( F("+-"), plus_or_minus_alt, array_size( plus_or_minus_alt ) ),
        new Parsing::Entier<long>( F("steps"), motor_command._steps, LONG_MAX - 1, '\n' )
      };

  if ( parse_that_seq.consume( achar ) ) {
    it consumed it...
    if ( parse_that_seq.done ) {
      it ate all it will eat, done, success
      if (do_print_help) {
        print_help.print_help( commands_alt, array_size( commands_alt ) );
      }
      parse_that_seq.reset(); // to allow it to repeat
    }
    // still parsing
  }
  else {
    // x doesn't match, not-consumable
    if ( commands.error && ! commands.at_start ) {
      commands.say_error(x);
    }
    // else { shouldn't happen }
    commands.reset();
  }

*/

namespace Parsing {

/* some parsing classes:
  construct("why", &store, parsing parameter)
    DO NOT call ->reset() in the constructor
      only because it is too easy to have Serial debugs which lock the system
  .consume(char x)
    return true if char was consumed
    return false if char was not consumed
      check .error
  .error() -> char* if expected something that was bad, NULL if no error

  # top-level protocol
  if command.consume()
      if .done()
          // handled
          do something with the result (if necessary)
      if .error()
          if .at_start
              .say_error(x)
      command.reset()

  ALSO:
  if (print-help) {
    .print_help()
  }
*/

class BaseClass {

  public:
    const __FlashStringHelper* why; // should be const const *
    const __FlashStringHelper* error = NULL;
    boolean at_start = true;
    boolean done = false; // won't call again if done (after consume()->false

    BaseClass(const __FlashStringHelper* why) : why(why) {}

    virtual bool consume(char x) = 0; // do the parse of this char, call .() when done, respond to '?' as help
    virtual void reset() { // reset our vars
      //Serial << why << F(" reste") << endl;
      this->error = NULL;
      this->at_start = true;
      this->done = false;
      //Serial << why << F(" done reste") << endl;
    }
    virtual void operator()() {} // do some action when done

    virtual void say_error(char x) {
      Serial << F("# Bad input during ") << this->why << F(" ");
      if ( x < ' ' ) Serial << F("0x") << _HEX(x);
      else Serial << F("'") << x << F("'");
      Serial << F(" ") << this->error << endl;
    }

    virtual void print_help(int indent) = 0;
};

class String : public BaseClass {
  // Match a string
  public:

    const char * const string;
    const unsigned int string_len;

    unsigned str_i=0;

    String(const __FlashStringHelper* why, const char * const string) : BaseClass(why), string(string), string_len(strlen(string)) {
    }

    void reset() {
      BaseClass::reset();
      str_i=0;
    }

    bool consume(char x) {

      unsigned int was = str_i; // debug

      if ( x == string[str_i] ) {
        str_i += 1;
        if ( str_i != was ) {
          /*
          Serial 
          << F("  '") << x << F("' Looking[/") << string << F("] ") 
          << was << F("'") << string[was] << F("'")
          << F(" -> ") << str_i << F("'") << string[str_i] << F("'")
          << endl;
          */
        }

        if (str_i == string_len) 
          {
          Serial << why << F(" eos ") << endl;
          done = true;
          }

        return true;
      }
      else {
        this->error = F("Didn't match string");
        return false;
      }
    }

    void say_error(char x) {
      BaseClass::say_error(x);
      Serial << F(" [") << str_i << F("] '") << string << endl;
    }

    void print_help(int indent) {
      // no help
    }
};

class Char : public BaseClass {
    /* Single char parsing
      .which_char is the char consumed.
      &var is optional for storage
      at_start is always true
      does not report errors
    */

  public:

    // One specific char
    char *var;
    const char which_char;

#define WRONG_CHAR F("wrong character");

    Char(const __FlashStringHelper* why, const char which_char) : BaseClass(why), var(NULL), which_char(which_char) { }
    Char(const __FlashStringHelper* why, char &var, const char which_char) : BaseClass(why), var(&var), which_char(which_char) { }

    bool consume(char x) {
      if (this->done) {
        return false; // not an error, just done
      }
      else if ( x == this->which_char ) {
        this->done = true;
        if (var != NULL) {
          (*var) = x;
        }
        (*this)();
        return true; // consumed (and also done)
      }
      else {
        this->error = WRONG_CHAR;
        return false;
      }
    }

    void print_help(int indent) {
      // no help
    }

};

class TillDelimiter : public BaseClass {
    /* Consume till delimiter
      .delimiter is the char consumed.
      &var is optional for storage, does not store the delimiter, stores only up to max
      max is max length, including \0!
      at_start is always true
      does not report errors
    */

  public:

    // One specific char
    const char delimiter;
    char * const var;
    const unsigned int max;

    unsigned int var_i=0;

    TillDelimiter(const __FlashStringHelper* why, char delimiter, char *var=NULL,unsigned int max=0) 
      : BaseClass(why), delimiter(delimiter), var(var), max(max)
      { }

    bool consume(char x) {
      if ( x == delimiter ) {
        if ( max != 0 && var != NULL ) {
          var[max-1] = 0;
        }
        this->done = true;
        // (*this)();
      }
      else if ( max != 0 && var != NULL ) {
        if ( var_i < max-1 ) {
          Serial << why << F(" accumulate '") << x << F("' [") << var_i << F("/") << max << F("]") << endl;
          var[var_i] = x;
          var_i += 1;
        }
        else {
          Serial << F("Warning, excess chars dropped '") << x << F("' at [") << var_i << F("/") << max << F("]") << endl;
        }
      }
      else {
        // dropping
        Serial << F(">") << x << F("<") << endl;
      }
      return true; // we always consume, being done on delimiter
    }

    void reset() {
      BaseClass::reset();
      var_i = 0;
    }

    void print_help(int indent) {
      Serial << why << F(" 0x") << _HEX(int(delimiter)) << endl;
    }

};

class Space : public Char {
  public:
    Space() : Char(F("space"), ' ') {}
};

template <typename T>
class Entier : public BaseClass {
    // Parse till delimiter
  public:

    T *var;
    const T max_value;
    char delimiter;
    unsigned int ct = 0;

    // gah, how to do a static F()?
#define TOO_LARGE F("too large");
#define WRONG_DIGIT F("digit or delimiter");

    Entier(const __FlashStringHelper* why, T &var, T max_value, char delimiter ) : BaseClass(why), var(&var), max_value(max_value), delimiter(delimiter) {
      *(this->var) = 0;
    }

    void reset() {
      BaseClass::reset();
      ct = 0;
    }

    bool consume(char x) {
      if ( at_start) {
        *(this->var) = 0;
      }

      if ( x >= '0' && x <= '9' ) {
        this->at_start = false; // committed
        ct++;

        // Serial << F(" ## before ") << (*var) << F(" x '") << x << F("' -0 ") << (x-'0') << F(" var*10 ") << (*var*10) << F(" + ") << ( (*var) * 10 + (x-'0') ) << endl;

        return accumulate(x);
      }


      else if ( !at_start && x == this->delimiter ) {
        // delim after at least 1 digit
        this->done = true;
        check_done();
        return true;
      }
      else {
        this->error = WRONG_DIGIT;
        return false;
      }
    }

    virtual void check_done() {  }

    virtual boolean accumulate(char x) {
      // it's good, accumulate it

      *var = (*var) * 10 + (x - '0');
      // Serial << F("UI ") << (*var) << endl;

      if ( *var > this->max_value ) {
        this->error = TOO_LARGE;
        return false;
      }

      return true;
    }

    void say_error(char x) {
      BaseClass::say_error(x);
      Serial << F("#  max ") << this->max_value << F(" sofar ") << (*var) << endl;
    }

    void print_help(int indent) {
      // no help
    }
};

class Decimal : public Entier<float> {
    /* The decimal part of a number
      max_value is actually min value, i.e. 0.0001
    */

  public:
    float divider = 1.0;

    Decimal(const __FlashStringHelper* why, float &var, float max_value, char delimiter ) : Entier<float>(why, var, max_value, delimiter) {}

    void reset() {
      Entier<float>::reset();
      divider = 1.0;
    }

    virtual void check_done() {
      if ( *var < this->max_value ) {
        this->error = TOO_LARGE;
      }
    }

    boolean accumulate(char x) {
      // it's good, accumulate it
      divider = divider * 10;

      *var = (*var) + (x - '0') / divider ;
      //Serial << F("UI ") << (*var) << F(" / ") << divider << endl;

      return true;
    }

    void print_help(int indent) {
      // no help
    }
};


/* Parse and respond:
  subclass a Parsing, init with whatever needed, override ()
  if xxx.consume( char )
    if xxx.done
      stop calling xxx.consume
    else
      call xxx.consume again
  else
    if xxx.error
      xxx.say_error(char x)
    stop calling xxx.consume
    try char on next parser

  a discard parser is last
*/

class Discard : public BaseClass {
    // discard till eol
  public:
    int ct = 0;
    char first_seen = ' ';

    Discard() : BaseClass(F("Discarding till eol")) {
      this->at_start = false; // we are never "at start". i.e. we are consuming, should have no practical effect
    }

    void reset() {
      BaseClass::reset();
      this->ct = 0;
      this->at_start = false;
      this->first_seen = ' ';
    }

    bool consume(char x) {
      if (this->done) {
        return false; // not an error, just done
      }
      else if ( x == '\n' ) {
        //Serial << F("D saw eol") << endl;
        this->done = true;
        (*this)();
        return true; // consumed (and also done)
      }
      else {
        if (this->ct == 0) {
          first_seen = x;
        }
        this->ct++;
        return true; // we consume till eol
      }
    }

    virtual void operator()() {
      Serial << F("# Discarded from '") << first_seen << F("' ct ") << this->ct << endl;
    }

    void print_help(int indent) {
      // no help
    }

};

class Sequence : public BaseClass {
    /*
      the input has to be the sequence of parsers.
      probably subclass and implement a .() and some getter.

      static BaseClass (*sequence)[] = { new x, new y }
      Sequence( F("why"), sequence, array_size(sequence) )
      then treat it like a regular parser, but see the top-level note above

      Reports error if !at-start
    */
  public:
    // i'm incompetent at const and pointer
    BaseClass * const* sequence; // the actual [], the pointer is const
    unsigned int sequence_size;
    unsigned int sequence_i = 0;  // [i] into it (must be after sequence)

    Sequence( const __FlashStringHelper* why, BaseClass * const*sequence, unsigned int sequence_size)
      : BaseClass(why), sequence(sequence), sequence_size(sequence_size)
    {
    }

    void reset() {
      //Serial << F("SEQ reste") << endl;
      BaseClass::reset();
      sequence_i = 0;
      current()->reset(); // the [0]
      //Serial << F("SEQ done reste ") << sequence_i << endl;
    }

    inline BaseClass * current() {
      return sequence[ sequence_i ];
    }

    boolean consume(char x) {
      //Serial << F("  @[") << sequence_i << F("] ") << current()->why << F(" '") << x << F("'") << endl;
      if (done) {
        Serial << F("# Fail, already done: ") << why << endl;
      }
      if (at_start) {
        current()->reset(); // probably redundant
      }
      if ( current()->consume(x) ) {
        // we committed
        this->at_start = false;
        //Serial << F("Consumed Seq [") << sequence_i << F("] ") << current()->why << endl;

        if ( current()->done ) {
          // it's done, and handled it
          Serial << F("Done Seq [") << sequence_i << F("] ") << current()->why << endl;
          current()->reset(); // FIXME need to not reset this till end of sequence. so change to init before
          sequence_i++;
          Serial << F("   and that's done so should do ") << sequence_i << endl;
          if (sequence_i == sequence_size) {
            //Serial << F(" SEQ done ") << endl;
            this->done = true;
            (*this)(); // if there's anything to do, in a subclass
          }
        }
        return true; // and possibly done
      }

      else {
        // not expected
        this->error = F("At ");
        //Serial << F("# 1unexpected '" ) << x << F("' atst ") << why << endl;
        if ( ! at_start ) {
          current()->say_error(x);
          this->say_error(x);
        }
        current()->reset();
        return false;
      }
    }

    void say_error(char x) {
      BaseClass::say_error(x);
      Serial << F("#  sequence[") << sequence_i << F("]") << endl;
    }

    void print_help(int indent) {
      Serial << why << endl; // fixme: indent
      for (unsigned int i = 0; i < sequence_size; i++) {
        Parsing::BaseClass *parser = sequence[i];
        Serial << F("  ") << parser->why << endl;
        parser->print_help(indent+4 /*indent*/ );
      }
    }
};

class PrintHelp : public BaseClass {
    // we never set .error
    // Usage: put last in a sequence (typically) to respond to '?', sets the flag
  public:
    boolean *flag;

    PrintHelp(boolean *flag) : BaseClass(F("Print Help")), flag(flag) {}
    bool consume(char x) {
      if (x == '?') {
        Serial << "Help" << endl;
        this->done = true;
        *flag = true;
        return true;
      }
      else {
        return false;
      }
    }

    void reset() {
      BaseClass::reset();
      *flag = false;
    }

    void print_help( BaseClass &commands) {
      commands.print_help(0);
    }
    void print_help( BaseClass * commands) {
      commands->print_help(0);
    }
    void print_help(int indent) {
      for (; indent > 0; indent--) Serial << F(" ");
      Serial << F("? # print help") << endl;
    }
};


class Alternate : public BaseClass {
    /*
      the input has to be One of the parsers.
      probably subclass and implement a .() and some getter.

      static BaseClass (*alts)[] = { new x, new y }
      Alternate( F("why"), alts, array_size(alts) )
      then treat it like a regular parser, but see the top-level note above

      Reports error if anyone starts.
    */
  public:
    // i'm incompetent at const and pointer
    BaseClass * const *alt; // the actual [], the pointer is const
    unsigned int alt_size;
    unsigned int alt_i = 0;  // [i] into it (must be after alt)

    Alternate( const __FlashStringHelper* why, BaseClass * const *alt, unsigned int alt_size)
      : BaseClass(why), alt(alt), alt_size(alt_size)
    {
    }

    void check_done() {
      if ( current()->done ) {
        // it's done, and handled it
        Serial << F(" ALT done ") << endl;
        this->done = true;
        (*this)(); // if there's anything to do, in a subclass
      }
    }

    void reset() {
      //Serial << F("SEQ reste") << endl;
      BaseClass::reset();
      alt_i = 0;
      current()->reset(); // the [0]
      //Serial << F("SEQ done reste ") << alt_i << endl;
    }

    inline BaseClass * const current() {
      return alt[ alt_i ];
    }

    boolean consume(char x) {
      //Serial << F("  @[") << alt_i << F("] ") << current()->why << endl;
      if (done) {
        Serial << F("Fail, already done: ") << why << endl;
      }

      if (at_start) {
        // need to find the alt for the 1st char
        for ( alt_i = 0; alt_i < alt_size; alt_i++) {
          current()->reset();
          if ( current()->consume(x) ) {
            //Serial << F("Alt w/ ") << current()->why << endl;
            at_start = false; // committed
            check_done(); // we could be done w/1 char
            return true;
          }
        }

        // or it's an error
        error = F("was none of");
        return false;
      }

      // now just eat the chars
      if ( current()->consume(x) ) {

        check_done();
        return true; // and possibly done
      }

      else {
        // not expected
        this->error = F("At ");
        //Serial << F("# 2unexpected '" ) << x << F("' atst ") << why << endl;
        if ( ! at_start ) {
          current()->say_error(x);
          this->say_error(x);
        }
        return false;
      }
    }

    void say_error(char x) {
      BaseClass::say_error(x);
      for ( alt_i = 0; alt_i < alt_size; alt_i++) {
        Serial << F("#  ") << current()->why << endl;
      }
    }

    void print_help(int indent) {
      // no help
      Serial << why << endl; // fixme: indent
      for (unsigned int i = 0; i < alt_size; i++) {
        Parsing::BaseClass *parser = alt[i];
        Serial << F("  ") << parser->why << endl;
        parser->print_help(indent+4 /*indent*/ );
      }
    }
};

BaseClass * const ping_sequence[] = { new Char(F("start: #"), '#'), new Char(F("eol"), '\n') };

class Ping : public Sequence {
  // echos #
  public:

    // '#' -> Just respond with '#'
    Ping() : Sequence( F("ping #"), ping_sequence, array_size( ping_sequence ) ) {
    }

    virtual void operator()() {
      // maybe enhance with "echo till eol"
      Serial << F("#") << endl;
    }

    void reset() {
      //Serial << F("P reste") << endl;
      Sequence::reset();
      //Serial << F("P done reste") << endl;
    }
    void print_help(int indent) {
      for (; indent > 0; indent--) Serial << F(" ");
      Serial << F("#") << endl;
    }
};

using SimpleCallback =  void (*)();
using SingleCharCallback = void (*)(const char x);

Char eol(F("eol"), '\n'); // really should be const, but i'm incompetent at const * []

class SingleCommand : public Sequence {
  // single char, \n, with callback
  public:

    Parsing::BaseClass *eol_terminated[2] = { NULL, &eol }; // we'll fixup the null
    SimpleCallback callback;
    const char which_char; // redundant, but suks

    SingleCommand(const __FlashStringHelper* why, const char command, SimpleCallback callback) : Sequence(why, eol_terminated, 2), callback(callback), which_char(command) {
      // it's ok to set the [0] after Sequence() construct, because construct doesn't look at it
      eol_terminated[0] = new Char(F("command"), command);
    }

    virtual void operator()() {
      (*callback)();
    }
    
    void print_help(int indent) {
      for (; indent > 0; indent--) Serial << F(" ");
      Serial << which_char << endl;
    }

};

class OneOf : public BaseClass {
    // one of the chars
  public:
    SingleCharCallback callback;
    const char* oneof;
    char matched = ' ';

    OneOf(const __FlashStringHelper* why, const char* oneof, SingleCharCallback callback) : BaseClass(why), callback(callback), oneof(oneof) {
    }

    bool consume(char x) {
      for (const char * c = oneof; *c != 0; c++) {
        if (*c == x) {
          matched = x;
          at_start = false;
          done = true;
          (*this)();
          return true;
        }
      }
      error = F("no match");
      return false;
    }
    void reset() {
      this->matched = ' ';
    }
    virtual void operator()() {
      (*callback)(matched);
    }
    void print_help(int indent) {
      // no help
    }
};

class Loop : public BaseClass {
    // Repeat the Parsing class forever
  public:
    BaseClass * const parser;
    boolean do_help;

    Loop(const __FlashStringHelper* why, BaseClass * const parser, boolean do_help=false) 
      : BaseClass(why), parser(parser), do_help(do_help) {
    }

    bool consume(char achar) {
      if ( parser->consume( achar ) ) {
        Serial << why << F(".consumed '") << achar << F("'") << endl;
        if ( parser->done ) {
          Serial << why << F(".done") << endl;
          parser->reset(); // to allow it to repeat
        }
        return true; // still parsing, never done (till error)
      }
      else if (do_help && achar == '?') {
        parser->reset(); // to allow it to repeat
        print_help(0);
        return true;
      }
      else {
        // x doesn't match, not-consumable
        if ( parser->error && ! parser->at_start ) {
          parser->say_error(achar);
          parser->reset();
          return false;  // We do exit on error
        }
        else {
          // not consumed, fall off end
        }

        parser->reset();
        return false;
      }
    }

    void reset() {
      parser->reset();
    }
    void print_help(int indent) {
      Serial << why << endl;
      parser->print_help(indent);
      Serial << "? : print help" << endl;
    }
};
};
