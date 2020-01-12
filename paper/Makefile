fuzz2020.pdf: 
	# compile tex files
	echo latex -f main.tex
	latex -f main.tex
	
	# compile reference.bib and main.aux to get main.bbl
	echo bibtex main
	bibtex main
	
	# compile with main.bbl to update main.dvi
	echo latex -f main.tex
	latex -f main.tex
	latex -f main.tex
	
	# convert dvi file to pdf file
	echo dvipdfmx main.dvi -o fuzz2020.pdf
	dvipdfmx main.dvi -o fuzz2020.pdf

clean: 
	rm main.aux
	rm main.bbl
	rm main.blg
	rm main.dvi
	rm main.log
	rm main.out
