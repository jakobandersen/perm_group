#!/bin/bash

if [ "x$1" = "xclean" ]; then
	rm -rf build doctrees source/reference
	exit 0
fi

# the main logic for building is based on the auto-generated Makefile from Sphinx

sphinxBuild=${1:-sphinx-build}
topSrcDir=${2:-..}
topBuildDir=${3:-..}

which $sphinxBuild &> /dev/null
if [ $? -ne 0 ]; then
	echo "Error: '$sphinxBuild' was not found."
	exit 1
fi

function doBuild {
	allOpts="-d $topBuildDir/doc/doctrees $topSrcDir/doc/source"
	mkdir -p $topSrcDir/doc/source/_static
	$sphinxBuild -b html $allOpts $topBuildDir/doc/build/html
	echo "$sphinxBuild -b html $allOpts $topBuildDir/doc/build/html"
}

function outputRST {
	mkdir -p $topSrcDir/doc/source/$(dirname $1)
	cat | sed "s/	/    /g" > $topSrcDir/doc/source/$1.rst
}

function filterCPP {
	cat | awk '
BEGIN {
	inClass = 0
	lineNum = 0
	inNestedClass = 0
	nestedLineNum = 0
	lastClass = ""
	lastNested = ""
}
{
	if($0 ~ /^[\t]*\/\/ rst: .. py:class::/) {
		sub(/^[\t]*\/\/ rst: .. py:class:: /, "")
		print ""
		print "Class ``" $0 "``"
		print "--------------------------------------------------------------------------------------------------------------------------------"
		print ""
		print ".. py:class:: " $0
	} else if($0 ~ /^[\t]*\/\/ rst:/) {
		sub(/^[\t]*\/\/ rst:[ 	]?/, "")
		if(inClass)
			if(inNested)
				nestedRST[nestedLineNum++] = $0
			else
				normalRST[lineNum++] = $0
		else
			print
	} else if($0 ~ /^[\t]*\/\* rst:/) {
		sub(/^[\t]*\/\* rst: ?/, "")
		sub(/\*\/[\t]*\\?/, "")
		if(inClass)
			if(inNested)
				nestedRST[nestedLineNum++] = $0
			else
				normalRST[lineNum++] = $0
		else
			print
	} else if($0 ~ /^[\t]*\/\/ rst-class:/) {
		sub(/^[\t]*\/\/ rst-class: /, "")
		lastClass = $0
		sub(/ : .*/, "", lastClass)
		print ""
		print "Class ``" lastClass "``"
		print "--------------------------------------------------------------------------------------------------------------------------------"
		print ""
		print ".. class:: " $0
		print "    "
	} else if($0 ~ /^[\t]*\/\/ rst-class-start:/) {
		inClass	= 1
		print ""
		print "Synopsis"
		print "^^^^^^^^"
		print ""
		print ".. code-block:: c++"
		print "    "
	} else if($0 ~/^[\t]*\/\/ rst-class-end:/) {
		inClass = 0
		if(lineNum > 0) {
			print ""
			print "Details"
			print "^^^^^^^"
			print ""
			print ".. cpp:namespace:: perm_group"
			print ""
			print ".. cpp:namespace-push:: " lastClass
			print ""
			for(i = 0; i < lineNum; i++) print normalRST[i]
			print ""
			print ".. cpp:namespace:: perm_group"
			print ""
			lineNum = 0
		}
		if(nestedLineNum > 0) {
			for(i = 0; i < nestedLineNum; i++) print nestedRST[i]
			print ""
			nestedLineNum = 0
		}
	} else if($0 ~/^[\t]*\/\/ rst-nested:/) {
		sub(/^[\t]*\/\/ rst-nested: /, "")
		if(!inClass) {
			print "Nested class outside class!" | "cat 1>&2"
			exit 1
		}
		inNested = 1
		nestedRST[nestedLineNum++] = ""
		nestedRST[nestedLineNum++] = "Class ``" $0 "``"
		nestedRST[nestedLineNum++] = "---------------------------------------------------------------"
		nestedRST[nestedLineNum++] = ""
		nestedRST[nestedLineNum++] = ".. class:: " $0
		nestedRST[nestedLineNum++] = ""
		lastNested = $0
	} else if($0 ~/^[\t]*\/\/ rst-nested-start:/) {
		nestedRST[nestedLineNum++] = ""
	} else if($0 ~/^[\t]*\/\/ rst-nested-end:/) {
		nestedRST[nestedLineNum++] = ""
		inNested = 0
	} else if(inClass) {
		if(!($0 ~/^[\t]*$/))
			print "\t"$0
	}
}
'
}

function getHeaders {
	cd $topSrcDir/include/perm_group
	find . -iname "*.hpp" | sed -e "s/^.\///" -e "s/.hpp$//" | sort
}

function makeIndex {
	function indexFiles {
		cat << "EOF"
	installation
	changes
	reference/index
EOF
	}
	function data {
		cat << "EOF"
################################################
PermGroup
################################################

.. toctree::
	:maxdepth: 1
	:numbered:
	
EOF
		indexFiles
cat << "EOF"


.. _overview:

Overview
========

This is the documentation for the PermGroup library,
a library for working efficiently with permutation groups.


Indices and Tables
==================

* :ref:`genindex`
EOF
cat << "EOF"

Table of Contents
=================

.. toctree::
	:maxdepth: 4
	
EOF
		indexFiles
	}
	data | outputRST index
}

function makeReference {
	function data {
		local f=$1
		echo ".. _cpp-$f:"
		echo ""
		echo "**********************************************************"
		echo "$f.hpp" | sed "s/^.*\///"
		echo "**********************************************************"
		echo ""
		echo "Full path: ``$f.hpp``"
		echo ""
		cat << "EOF"
.. default-domain:: cpp
.. default-role:: cpp:expr

.. cpp:namespace:: perm_group

EOF
		cat $topSrcDir/include/perm_group/$f.hpp | filterCPP
	}
	for f in $(getHeaders); do
		data $f | outputRST reference/$f
	done
	function getFolders {
		getHeaders | grep "/" | sed "s/\/.*//" | uniq
	}
	function folderToc {
		echo $1
		cat << "EOF"
==============================================================================

.. toctree::
	:maxdepth: 2

EOF
		getHeaders | grep "^$1" | sed -e "s/^$1\///" -e "s/^/	/"
	}
	for f in $(getFolders); do
		folderToc $f | outputRST reference/$f/index	
	done

	function dataToc {
		cat << "EOF"
Reference
=========

.. toctree::
	:maxdepth: 2

EOF
		(
			getHeaders | grep -v "/"
			getFolders | sed 's/$/\/index/'
		) | sort | sed 's/^/	/'
	}
	dataToc | outputRST reference/index
}

makeIndex
makeReference
doBuild
