# -*- coding: utf-8 -*-
from PyQt4.QtGui import *
import sys

app = QApplication(sys.argv)
w = QTextBrowser()
w.show()

css = open("template.css", "r").read();
w.document().setDefaultStyleSheet(css)

html = open("template.html", "r").read();
w.document().setHtml(html)

app.exec_()
