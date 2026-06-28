QUARTO ?= quarto

all: render

preview:
	$(QUARTO) preview --host 0.0.0.0

render:
	$(QUARTO) render

clean:
	rm -rf _book .quarto

# rsvg-convert
# lualatex
deps:
	sudo apt install -y librsvg2-bin texlive-latex-base texlive-luatex texlive-latex-recommended texlive-latex-extra
