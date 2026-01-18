# -- Project information ------------------------------------------------------
project = "Zephyr Workshop"
copyright = "2026 Jonas Remmert"
author = "Jonas Remmert"

# -- General configuration ----------------------------------------------------
extensions = []

exclude_patterns = ["_build", "Thumbs.db", ".DS_Store", ".tox", "**/.tox", "slides"]

# -- Options for HTML output --------------------------------------------------
html_theme = "sphinx_rtd_theme"
html_title = project
# html_favicon = "_static/images/favicon.png"
html_static_path = ["_static"]
html_extra_path = ["slides/dist"]

html_theme_options = {
    # "logo_only": True,
    "prev_next_buttons_location": None
}
