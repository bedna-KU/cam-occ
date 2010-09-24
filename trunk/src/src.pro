TEMPLATE = subdirs

#I must either compile everything into one huge lib, or split up like this: cannot
#go inbetween because of circular dependencies
#qocc MUST be last because it depends on the other libs
#uio must be first
SUBDIRS += contrib uio g2m qocc

CONFIG += ordered
