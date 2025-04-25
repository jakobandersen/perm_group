import os
import sys

# -- General configuration ------------------------------------------------

needs_sphinx = '1.7.1'

sys.path.append(os.path.abspath('extensions'))
extensions = [#'sphinx.ext.intersphinx',
    'sphinx.ext.todo',
    'sphinx.ext.mathjax',
    'sphinx.ext.githubpages',
	'cpp_concepts']

cpp_index_common_prefix = ["perm_group::"] 

templates_path = ['_templates']

source_suffix = '.rst'

master_doc = 'index'

project = u'PermGroup'
copyright = u'2017-2025, Jakob Lykke Andersen'
author = u'Jakob Lykke Andersen'

with open("../../VERSION") as f:
	version = f.read()
	version = version.strip() # remove the newline
release = version

language = "en"

exclude_patterns = []

pygments_style = 'sphinx'

todo_include_todos = True


# -- Options for HTML output ----------------------------------------------

html_theme = 'classic'

html_theme_options = {
	"body_max_width": None 
}

# Example configuration for intersphinx: refer to the Python standard library.
intersphinx_mapping = {'https://docs.python.org/': None}
