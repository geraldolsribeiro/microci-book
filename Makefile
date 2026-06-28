all:
	docker run --rm -v $(shell pwd):/documents/ asciidoctor/docker-asciidoctor asciidoctor-pdf \
    -D _book book/index.adoc

clean:
	rm -rf _book

book/images/%.png: book/images/%.webp
	convert $< png:- | pngquant --quality=65-80 --speed 1 - > $@

# pngquant --quality=65-80 --speed 1 --ext -compressed.png cover.png

local-deps:
	sudo apt install asciidoctor ruby-asciidoctor-pdf
