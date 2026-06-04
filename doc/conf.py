# -- Project information ------------------------------------------------------
project = "Zephyr Workshop"
copyright = "2026 Jonas Remmert, Licensed under CC BY-SA 4.0"
author = "Jonas Remmert"

import os
import sys
from docutils.parsers.rst import Directive

sys.path.insert(0, os.path.abspath('.'))

# Mock directive for zephyr-app-commands used in sample READMEs
class ZephyrAppCommands(Directive):
    has_content = True
    option_spec = {
        'zephyr-app': str,
        'host-os': str,
        'board': str,
        'goals': str,
        'compact': lambda x: x,
    }
    
    def run(self):
        return []

def setup(app):
    app.add_directive('zephyr-app-commands', ZephyrAppCommands)

# -- General configuration ----------------------------------------------------
extensions = []

exclude_patterns = [
    "_build",
    ".*",
    "**/.*",
    "Thumbs.db",
    ".DS_Store",
    "slides",
]

# -- Options for HTML output --------------------------------------------------
html_theme = "sphinx_rtd_theme"
html_title = project
# html_favicon = "_static/images/favicon.png"
html_static_path = ["_static"]
html_extra_path = ["slides_dist"]

html_theme_options = {
    # "logo_only": True,
    "prev_next_buttons_location": None
}

# -- Options for LaTeX/PDF output ---------------------------------------------
latex_engine = "xelatex"
latex_elements = {
    "fontpkg": r"""
\setmainfont{DejaVu Serif}
\setsansfont{DejaVu Sans}
\setmonofont{DejaVu Sans Mono}
""",
}
latex_documents = [
    ("index", "zephyr-workshop-docs.tex", project, author, "manual"),
]
