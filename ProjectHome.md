Aug 2011: Switched from svn to git.

I'm spending most of my time on the [STEP Class Library](http://github.com/mpictor/StepClassLibrary) now.

If you want g-code visualization, I suggest [cutsim](http://github.com/aewallin/cutsim) by Anders Wallin. Anders found some of the classes in g2m to be useful - I'm glad someone is getting use out of this!

If you want CAD/CAM, I suggest HeeksCAD/HeeksCNC, now at http://github.com/Heeks

Another program that may interest you is [OpenShapeFactory](http://code.google.com/p/openshapefactory/)


---


I got to a point with cam-occ where I couldn't figure out where to go; creating accurate material removal models is fiendishly difficult for some motions. HLR gives me the tool's silhouette, which is useful - but it results in a set of lines that is usually more than a simple loop. Figuring out which lines belong and which don't is a pain.

Now git includes cmake build files as well as qmake.
To use cmake to create a bare-bones .deb package:
```
cd /path/to/project
cmake .
make
make package
```

6/17/10: doxygen documentation can be viewed [here](http://cam-occ.googlecode.com/git/doc/doxygen/html/index.html).

Unfortunately, gcode2model doesn't work with many file/tool combinations, due to OCC's boolean operations being very picky.

![http://cam-occ.googlecode.com/git/wiki/images/face-sc-sq-step.png](http://cam-occ.googlecode.com/git/wiki/images/face-sc-sq-step.png)


---

1-31-2009:
I'm working on cam-occ again, and it's now updated in SVN.  cam-occ has been re-organized. I hope that it's easier for others to work with.  I used QtOCC, so this version requires qt4.  I compiled it with OCC 6.3, but I don't think I used any new functionality - if you are on Debian, you can probably use the precompiled .debs.

Note - this is called version 2 because of major changes to the code, **not** because it has more features!  There is a tarball under Featured Downloads, but it is outdated.  There is a [README](http://code.google.com/p/cam-occ/wiki/README) which goes into more detail.

Next up, I'm going to try to do some face analysis stuff. (1-29-09)


---


Andreas Kain modified cam-occ for a presentation at a conference in mid-2008.  The source and PDF are on the downloads tab.  If you have questions for Andreas, he can be reached at andreas.kain`@`pe.mw.tum.de . The website for his research group is at http://www.pe.mw.tum.de



If you try to compile and get an error of the form
```
/some/path/Standard_Real.hxx: In function 'Standard_Integer RealSize()':
/some/path/Standard_Real.hxx:162: error: 'CHAR_BIT' was not declared in this scope
```
make sure that `<limits>` is included, and `HAVE_FSTREAM` and `HAVE_LIMITS` are defined.



---


Contact: mpictor`@`gmail.com