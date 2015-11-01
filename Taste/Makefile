ifeq ($(OS),Windows_NT)
    EXE := .exe
else
    EXE := 
endif

# Project
PROJ 	= Taste
SRCS	= $(wildcard *.cpp) 
OBJS 	= $(SRCS:%.cpp=%.o)
DEPEND	= $(OBJS:%.o=%.d)

# Coco/R build
COCO_R		= ..
COCO		= $(COCO_R)/coco$(EXE)
COCO_OUT 	= Parser.h Parser.cpp Scanner.h Scanner.cpp
COCO_DEP 	= $(COCO_R)/Parser.frame $(COCO_R)/Scanner.frame $(COCO_R)/Copyright.frame

# Google Test
GTEST_OBJ	= t/gtest/gtest-all.o t/gtest/gtest_main.o
GTEST_DEPEND= $(GTEST_OBJ:%.o=%.d)

# test library with all module code
T_LIB		= t/libtestlib.a
T_LIB_OBJS	= $(OBJS:$(PROJ).o=) $(GTEST_OBJ)
T_SRCS		= $(wildcard t/*.cpp)
T_OBJS 		= $(T_SRCS:%.cpp=%.o)
T_DEPEND	= $(T_OBJS:%.o=%.d)
T_EXES		= $(T_OBJS:%.o=%$(EXE))
T_OUTS		= $(T_OBJS:%.o=%.out)

CXXFLAGS = -Wall -std=gnu++11 -MMD -I. -It -It/gtest

# Build Project
all: $(T_LIB) $(PROJ)$(EXE)

-include $(DEPEND)
-include $(GEST_DEPEND)

$(T_LIB) : $(T_LIB_OBJS)
	ar rcs $@ $(T_LIB_OBJS)

$(PROJ)$(EXE): $(COCO_OUT) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

t/%$(EXE) : t/%.o $(T_LIB)
	$(CXX) -static $(CXXFLAGS) $< -o $(<:.o=$(EXE)) -Lt -ltestlib

# Build Coco/R files
all: $(COCO_OUT)

$(COCO_OUT): $(PROJ).atg $(COCO_DEP) $(COCO)
	$(COCO) -frames $(COCO_R) $<
	$(MAKE)

$(COCO): $(wildcard $(COCO_R)/*.cpp) $(wildcard $(COCO_R)/*.h)
	$(MAKE) -C $(COCO_R)

# Test
test: $(PROJ)$(EXE) $(T_OUTS)
	perl -S prove t/*.t

t/%.out : t/%$(EXE)
	$<
	touch $@

# Clean
clean:
	$(RM) $(PROJ)$(EXE) $(OBJS) $(DEPEND) $(T_LIB) $(T_OBJS) $(T_DEPEND) $(T_EXES) $(T_OUTS) $(GTEST_OBJS) $(GTEST_DEPEND) t/*.out
