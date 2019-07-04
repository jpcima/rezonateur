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
	components/ResponseView.cpp \
	sources/Rezonateur.cpp \
	sources/svf/VAStateVariableFilter.cpp

# --------------------------------------------------------------
# Do some magic

UI_TYPE = cairo
include ../../dpf/Makefile.plugins.mk

BUILD_CXX_FLAGS += -Isources -Ithirdparty/blink

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(HAVE_JACK),true)
TARGETS += jack
endif

TARGETS += lv2_sep
TARGETS += vst

all: $(TARGETS)

# --------------------------------------------------------------
