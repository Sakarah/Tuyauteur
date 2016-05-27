# -*- Makefile -*-

lib_TARGETS = champion

# Tu peux rajouter des fichiers sources, headers, ou changer
# des flags de compilation.
champion-srcs = prologin.cc strategy.cpp utils.cpp pathfinding.cpp map.cpp
champion-dists = strategy.h utils.h pathfinding.h map.h
champion-cxxflags = -ggdb3 -Wall -std=c++11 -O2

# Evite de toucher a ce qui suit
champion-dists += prologin.hh
STECHEC_LANG=cxx
include ../includes/rules.mk
