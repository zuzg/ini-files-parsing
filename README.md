# ini-files-parsing
uni project to parse ini files into structures, search keys and evaluate expressions

## input
1. PATH-TO-INI-FILE.ini section.key -> returns value of given key in given section
2. PATH-TO-INI-FILE.ini expression "section1.key1 * section2.key2" -> returns value of given operation (+ - * /), concatenate in case of strings

## what it does
parse ini file into dynamic structures (linked list) and report invalid keys and section names (i.e. not alphanumeric signs)
