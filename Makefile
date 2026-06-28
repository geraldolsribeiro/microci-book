all:
	microCI | bash

help:
	@echo 'Targets:'
	@echo '  all                     Run the default build'
	@echo '  clean                   Remove build artifacts'
	@echo '  optimize-png            Optimize PNGs in book/images'
	@echo '  optimize-png-dry-run    Preview PNG optimization results'
	@echo '  optimize-png-file       Optimize one PNG: make optimize-png-file IMG=path'
	@echo '  local-deps              Install local dependencies'

clean:
	rm -rf _book

optimize-png:
	@find book/images -name '*.png' -type f | while read -r img; do \
		tmp="$$(mktemp --suffix=.png)"; \
		orig_size="$$(stat -c%s "$$img")"; \
		if pngquant --quality=65-80 --speed 1 --force --output "$$tmp" -- "$$img"; then \
			new_size="$$(stat -c%s "$$tmp")"; \
			saved=$$((orig_size - new_size)); \
			if [ "$$saved" -gt 0 ] && [ $$((saved * 100 / orig_size)) -ge 10 ]; then \
				mv "$$tmp" "$$img"; \
				printf 'optimized %s (%s -> %s bytes)\n' "$$img" "$$orig_size" "$$new_size"; \
			else \
				rm -f "$$tmp"; \
				printf 'skipped %s (reduction < 10%%)\n' "$$img"; \
			fi; \
		else \
			rm -f "$$tmp"; \
			printf 'failed %s\n' "$$img"; \
		fi; \
	done

optimize-png-dry-run:
	@find book/images -name '*.png' -type f | while read -r img; do \
		$(MAKE) optimize-png-file IMG="$$img" DRY_RUN=1; \
	done

optimize-png-file:
	@img="$(IMG)"; \
	if [ -z "$$img" ]; then echo 'IMG is required'; exit 1; fi; \
	tmp="$$(mktemp --suffix=.png)"; \
	orig_size="$$(stat -c%s "$$img")"; \
	if pngquant --quality=65-80 --speed 1 --force --output "$$tmp" -- "$$img"; then \
		new_size="$$(stat -c%s "$$tmp")"; \
		saved=$$((orig_size - new_size)); \
		if [ "$(DRY_RUN)" = "1" ]; then \
			if [ "$$saved" -gt 0 ] && [ $$((saved * 100 / orig_size)) -ge 10 ]; then \
				printf 'would optimize %s (%s -> %s bytes)\n' "$$img" "$$orig_size" "$$new_size"; \
			else \
				printf 'would skip %s (reduction < 10%%)\n' "$$img"; \
			fi; \
			rm -f "$$tmp"; \
		elif [ "$$saved" -gt 0 ] && [ $$((saved * 100 / orig_size)) -ge 10 ]; then \
			mv "$$tmp" "$$img"; \
			printf 'optimized %s (%s -> %s bytes)\n' "$$img" "$$orig_size" "$$new_size"; \
		else \
			rm -f "$$tmp"; \
			printf 'skipped %s (reduction < 10%%)\n' "$$img"; \
		fi; \
	else \
		rm -f "$$tmp"; \
		printf 'failed %s\n' "$$img"; \
	fi

book/images/%.png: book/images/%.webp
	convert $< png:- | pngquant --quality=65-80 --speed 1 - > $@

local-deps:
	sudo apt install asciidoctor ruby-asciidoctor-pdf pngquant imagemagick
