#!/bin/sh

svn ps svn:mime-type text/html *.map
svn ps svn:mime-type text/html *.html
svn ps svn:mime-type image/png *.png
svn ps svn:mime-type image/gif *.gif
svn ps svn:mime-type text/css *.css


