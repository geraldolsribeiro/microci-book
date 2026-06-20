QUARTO ?= quarto

all: render

preview:
	$(QUARTO) preview

render:
	$(QUARTO) render

clean:
	rm -rf _book .quarto
