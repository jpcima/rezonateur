#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------

# Disable stripping by default
SKIP_STRIPPING ?= true

# --------------------------------------------------------------
# Project name, used for binaries

NAME = rezonateur

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	RezonateurPlugin.cpp \
	RezonateurShared.cpp \
	sources/Rezonateur.cpp \
	sources/svf/VAStateVariableFilter.cpp

FILES_UI  = \
	RezonateurUI.cpp \
	RezonateurShared.cpp \
	Artwork.cpp \
	components/ResponseView.cpp \
	components/KnobSkin.cpp \
	components/SkinIndicator.cpp \
	components/SkinSlider.cpp \
	components/SkinToggleButton.cpp \
	sources/Rezonateur.cpp \
	sources/svf/VAStateVariableFilter.cpp \
	sources/utility/cairo++.cpp

# --------------------------------------------------------------
# Do some magic

UI_TYPE = cairo
include ../../dpf/Makefile.plugins.mk

BUILD_CXX_FLAGS += -Isources -Ithirdparty/blink -Ithirdparty/caps

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(HAVE_JACK),true)
TARGETS += jack
endif

TARGETS += lv2
TARGETS += vst

all: $(TARGETS)

Artwork.cpp Artwork.hpp: $(wildcard artwork/*)
	../../dpf/utils/res2c.py Artwork artwork

# --------------------------------------------------------------
