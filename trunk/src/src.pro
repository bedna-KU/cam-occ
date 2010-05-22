TEMPLATE = subdirs

#qocc MUST be last because it depends on the libraries already being built
SUBDIRS += intf g2m qocc

CONFIG += ordered
