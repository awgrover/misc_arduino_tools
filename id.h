// print out useful id

/*
Right click on the project->properties->arduino->tab "compile options" put

-DBOARD=\"${A.BUILD.BOARD}\"

BOARD is now a string define
*/

void id() {
  Serial.print(F( "Start " __FILE__ " " __DATE__ " " __TIME__ " gcc " __VERSION__ " ide "));
  Serial.println(ARDUINO);


}
