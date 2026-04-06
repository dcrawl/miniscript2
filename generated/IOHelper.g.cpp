// AUTO-GENERATED FILE.  DO NOT MODIFY.
// Transpiled from: IOHelper.cs

#include "IOHelper.g.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <algorithm>

namespace MiniScript {

TextStyle IOHelper::currentStyle = TextStyle::Normal;
void IOHelper::SetStyle(TextStyle style) {
	if (style == currentStyle) return;

	if (style == TextStyle::Normal) {
		std::cout << "\033[0m";
	} else if (style == TextStyle::Subdued) {
		std::cout << "\033[90m";
	} else if (style == TextStyle::Strong) {
		std::cout << "\033[1m";
	} else if (style == TextStyle::Error) {
		std::cout << "\033[31m";
	}

	currentStyle = style;
}
void IOHelper::Print(String message,TextStyle style) {
	SetStyle(style);
	std::cout << message.c_str() << std::endl;
}
String IOHelper::Input(String prompt,TextStyle promptStyle,TextStyle inputStyle) {
	SetStyle(promptStyle);

	std::cout << prompt.c_str();
	SetStyle(inputStyle);
	char *line = NULL;
	size_t len = 0;
	
	String result;
	int bytes = getline(&line, &len, stdin);
	if (bytes != -1) {
		line[strcspn (line, "\n")] = 0;   // trim \n
		result = line;
		free(line);
	}
	return result;
}
List<String> IOHelper::ReadFile(String filePath) {
	List<String> lines =  List<String>::New();
	std::ifstream file(filePath.c_str());
	if (!file.is_open()) {
		Print(String("Error: Could not open file ") + filePath);
		return lines;
	}
	
	std::string line;
	while (std::getline(file, line)) {
		line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
		lines.Add(String(line.c_str()));
	}
	file.close();
	return lines;
}

} // end of namespace MiniScript
