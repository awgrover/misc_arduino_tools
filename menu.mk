# Assuming a case block for menu-selections,
# Updates the print-menu block.
# Updates a basic case-block for menu actions.
#
# setup
# % cd $ard-proj-dir
# % make -f ~/dev/personal/awgArduino/menu.mk setup
# gives some instructions
#
# Usage:
# % make # edits the .ino and updates the display portion of the menu block
#
# In your .ino file, create a menu-block manually (see below for format), OR:
# to auto-generate a menu-block (template):
# * Choose where you want the menu handling block, usually the top of loop()
# * copy the menu.template file into there
#
# Finds "cases", and creates a print/read section of code:
# Will update the block delimited by:
# // menu made by:..
# ...
# // end menu
# The body will be:
#	case '?' :
#		print "$casea $desc"
#		...
#	case 0xFF ... to show prompt
#	case 0xFE ... read one char into menu_selection
#	default ... set menu_selection = '?'

ino=$(shell /bin/ls -1 *.ino | head -n 1)

menublock := \/\/ menu-block
menustart := \/\/ menu made by:
menuend := \/\/ end menu

.PHONY : menu
menu : .$(ino).menu
	@grep "$(menustart)" $(ino) >/dev/null || (echo 'In $(ino), expected: $(menustart)' ; false)
	@grep "$(menuend)" $(ino) >/dev/null || (echo 'In $(ino), expected: $(menuend)' ; false)
	@ cp $(ino) $(ino).bak
	awk '/$(menustart)/ {print; system("cat $<")}; /$(menuend)/ {print}; /$(menustart)/,/$(menuend)/ {next}; {print}' $(ino).bak > $(ino)
	@echo Edited $(ino)

# ignores default "." otherwise
PHONY : .$(ino).menu
.$(ino).menu : $(ino)
	perl -n -e '/case ('"'"'(.)'"'"'|([0-9]))\s*:\s*\/\/(.+)/i && do {print "Serial.println(F(\"$$2$$3 $$4\"));\n"}' $< > $@

.PHONY : setup
setup : Makefile menublock.cpp.x

Makefile :
	cp $(lastword $(MAKEFILE_LIST)) $@

menublock.cpp.x : FORCE
	cp $(dir $(lastword $(MAKEFILE_LIST)))/menu.template $@
	# Insert $@ into your .ino
	# Delete $@

.PHONY : FORCE
FORCE :

