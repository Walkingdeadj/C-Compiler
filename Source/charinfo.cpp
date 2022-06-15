#include "charinfo.h"
#include "charstream.h"

void print_char(char c, ostream& os = cout) {
	switch (c) {
	case ' ':
		os << "SPACE";
		break;
	case '\n':
		os << "NEWLINE";
		break;
	case '\r':
		os << "RETURN";
		break;
	case '\t':
		os << "TAB";
		break;
	default:
		os << c;
		break;
	}
}


void CharInfo::print(ostream& os) const {
	print_info(os);
	os << " Char ";
	print_char(value, os);
}

void CharInfo::print_info(ostream& os) const
{
	os << string(2 * stream->level, '.');
	if (stream->is_macro_expand()) {
		os << "Macro " << stream->macroname;
	}
	else {
		os << "File " << stream->filename
			<< " Line "
			<< setw(5)
			<< right
			<< line_no
			;
	}
}

ostream& operator<<(ostream& os, const CharInfo& pos) {
	pos.print(os);

	return os;
}