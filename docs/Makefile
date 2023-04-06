# MIT License
#
# Copyright (C) 2023 Advanced Micro Devices, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.
#

.PHONY: all install xml rst html clean cleanhtml cleanall check_tools

all: .stamp/sphinx

check_tools:
ifeq (, $(shell which doxygen 2>/dev/null))
	@echo "ERROR: No doxygen in PATH" && false
endif
ifeq (, $(shell which doxyrest 2>/dev/null))
	@echo "ERROR: No doxyrest in PATH" && false
endif
ifeq (, $(shell which sphinx-build 2>/dev/null))
	@echo "ERROR: No sphinx-build in PATH" && false
endif

install: .stamp/sphinx
ifeq (, $(HTML_DEST_DIR))
	@echo "ERROR: HTML_DEST_DIR is not set." && false
endif
	rsync -av --delete --exclude .git --exclude .nojekyll --exclude .buildinfo \
		 build/html/ $(HTML_DEST_DIR) 1>/dev/null

.stamp/doxygen: | check_tools
	rm -rf xml
	doxygen Doxyfile
	mkdir -p .stamp
	touch .stamp/doxygen

DOXYREST_FRAME = $(dir $(shell which doxyrest))/../doxyrest_frame

.stamp/doxyrest: .stamp/doxygen | check_tools
	rm -rf src/rst
	mkdir src/rst
	doxyrest xml/index.xml -o src/rst/index.rst \
		-F $(DOXYREST_FRAME) -f c_index.rst.in -Dg_verbatimToCodeBlock
	touch .stamp/doxyrest

.stamp/sphinx: .stamp/doxyrest Makefile.sphinx | check_tools
	make -f Makefile.sphinx html
	touch .stamp/sphinx

xml:
	rm -f .stamp/doxygen
	make .stamp/doxygen

rst:
	rm -f .stamp/doxyrest
	make .stamp/doxyrest

html:
	rm -rf .stamp/sphinx build/doctrees build/html
	make .stamp/sphinx

clean:
	rm -rf .stamp

cleanhtml:
	rm -rf .stamp/sphinx build/html build/doctrees

cleanall:
	rm -rf .stamp xml src/rst build/html build/doctrees
