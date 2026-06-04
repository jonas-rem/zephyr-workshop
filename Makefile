.PHONY: docs html slides slides-build doc-clean

NPM_CACHE ?= .npm-cache

docs: doc-clean slides-build
	uv run sphinx-build -b html doc doc/_build/html
	uv run sphinx-build -b latex doc doc/_build/latex
	$(MAKE) -C doc/_build/latex all-pdf

html: doc-clean slides-build
	uv run sphinx-autobuild doc doc/_build/html --open-browser --port 8001 --ignore "slides/*" --ignore "_build/*" --ignore "slides_dist/*"

slides: doc/slides/node_modules/.installed
	npm --prefix doc/slides --cache $(NPM_CACHE) run dev

slides-build: doc/slides/node_modules/.installed
	rm -rf doc/slides_dist
	npm --prefix doc/slides --cache $(NPM_CACHE) run build -- --base ./ --out ../slides_dist/zephyr-workshop_slides
	npm --prefix doc/slides --cache $(NPM_CACHE) run export -- --output ../slides_dist/zephyr-workshop_slides/zephyr-workshop_slides.pdf

doc/slides/node_modules/.installed: doc/slides/package.json
	npm --prefix doc/slides --cache $(NPM_CACHE) install --no-save --package-lock=false
	touch $@

doc-clean:
	rm -rf doc/_build
