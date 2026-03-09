// Test outer and globals from a nested function.

g = "global G"

outerFunc = function
	o = "outer O"
	innerFunc = function
		print "inner sees g: " + g
		print "inner sees o: " + o
		outer.o += " (updated)"
		globals.g += " (also updated)"
		print "globals.g: " + globals.g
		print "outer.o: " + outer.o
	end function
	innerFunc
end function

outerFunc
print "Back in main, g: " + g
