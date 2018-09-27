function runMod(str) {
	str = str.replace(/\r/g, "");
	var lines = str.split("\n");
	var newStr = "var __passage = \"\";";

	var inPassage = false;
	lines.forEach(function(line) {
		if (line == "START_PASSAGES") {
			newStr += "__passage = \"\";";
			inPassage = true;
		} else if (line == "---") {
			newStr += "submitPassage(__passage);\n";
			newStr += "__passage = \"\";";
		} else if (line == "END_PASSAGES") {
			newStr += "submitPassage(__passage);\n";
			inPassage = false;
		} else {
			if (inPassage) {
				line = line.replace(/\\/g, "\\\\");
				line = line.replace(/\"/g, "\\\"");
				newStr += "__passage += \"";
				newStr += line;
				newStr += "\\n\";";
			} else {
				newStr += line;
			}
		}
		newStr += "\n";
	});

	return newStr;
}
