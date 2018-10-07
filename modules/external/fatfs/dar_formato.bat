
..\..\tools\astyle --style=allman --recursive --add-brackets --align-reference=name --indent-classes --max-instatement-indent=90 --indent=spaces=4 --indent-classes --indent-switches --indent-col1-comments --max-instatement-indent=90 --pad-paren-in *.h 
..\..\tools\astyle --style=allman --recursive --add-brackets --align-reference=name --indent-classes --max-instatement-indent=90 --indent=spaces=4 --indent-classes --indent-switches --indent-col1-comments --max-instatement-indent=90 --pad-paren-in *.c 

del *.orig /s
del *.bak /s
