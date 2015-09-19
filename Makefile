ifeq ($(OS),Windows_NT)
    EXE := .exe
else
    EXE := 
endif

PROJ 	= Coco
SRCS	= $(wildcard *.cpp) 
OBJS 	= $(SRCS:%.cpp=%.o)
DEPEND	= $(OBJS:%.o=%.d)

CXXFLAGS = -Wall -std=gnu++11 -MMD

all: $(PROJ)$(EXE)

-include $(DEPEND)

$(PROJ)$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

clean:
	$(RM) $(PROJ)$(EXE) $(OBJS) $(DEPEND)

install:
	ln -s /usr/lib/coco-cpp/Coco $(DESTDIR)/usr/bin/cococpp
	install -m 0755 Coco $(DESTDIR)/usr/lib/coco-cpp
	install -m 0644 *frame $(DESTDIR)/usr/share/coco-cpp

