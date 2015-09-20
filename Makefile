ifeq ($(OS),Windows_NT)
    EXE := .exe
else
    EXE := 
endif

# Project
PROJ 	= Coco
SRCS	= $(wildcard *.cpp) 
OBJS 	= $(SRCS:%.cpp=%.o)
DEPEND	= $(OBJS:%.o=%.d)

# Coco/R build
COCO_OUT 	= Parser.h Parser.cpp Scanner.h Scanner.cpp
COCO_DEP 	= Parser.frame Scanner.frame Copyright.frame

CXXFLAGS = -Wall -std=gnu++11 -MMD

# Build Project
all: $(PROJ)$(EXE)

-include $(DEPEND)

$(PROJ)$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)
	$(MAKE)

# Build Coco/R files - cannot be included in build dependency - circular reference
atg: $(COCO_OUT)

$(COCO_OUT): $(COCO_DEP)
ifeq ($(wildcard $(PROJ)$(EXE)),$(PROJ)$(EXE))
	./$(PROJ)$(EXE) $(PROJ).atg
	$(MAKE)
endif

# Test
test: $(PROJ)$(EXE) atg
	$(MAKE) -C Taste
	$(MAKE) -C Taste test

# Clean
clean:
	$(MAKE) -C Taste clean
	$(RM) $(PROJ)$(EXE) $(OBJS) $(DEPEND)
	touch $(COCO_DEP)

install:
	ln -s /usr/lib/coco-cpp/Coco $(DESTDIR)/usr/bin/cococpp
	install -m 0755 Coco $(DESTDIR)/usr/lib/coco-cpp
	install -m 0644 *frame $(DESTDIR)/usr/share/coco-cpp

