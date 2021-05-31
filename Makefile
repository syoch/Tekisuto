TARGET		= Tekisuto

SRCDIR		= src
INCLUDE		= include
OBJDIR		= build

CXXFILES	= $(wildcard $(SRCDIR)/*.cpp)
CFILES		= $(wildcard $(SRCDIR)/*.c)
RESOURCES = $(wildcard $(SRCDIR)/*.rc)

OFILES		= \
	$(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(CXXFILES))) \
	$(patsubst %.c, $(OBJDIR)/%.o, $(notdir $(CFILES))) \
	$(patsubst %.rc, $(OBJDIR)/%.o, $(notdir $(RESOURCES)))

CFLAGS		= -O1 -I $(INCLUDE)
CXXFLAGS	= $(CFLAGS) -std=gnu++2a -Wno-psabi
LDFLAGS		= -Wl,--gc-sections,-s -mwindows -static-libgcc -static-libstdc++

all: $(TARGET)

clean:
	@rm -f $(OBJDIR)/*.o
	@rm -f $(TARGET)

re: clean all

$(OBJDIR)/%.o: $(SRCDIR)/%.rc $(wildcard $(SRCDIR)/*.h)
	@echo $(notdir $<)
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	@windres $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(wildcard $(SRCDIR)/*.h)
	@echo $(notdir $<)
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	@g++ $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(wildcard $(SRCDIR)/*.h)
	@echo $(notdir $<)
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	@gcc $(CFLAGS) -c $< -o $@

$(TARGET): $(OFILES)
	@echo linking...
	@g++ $(LDFLAGS) $^ -o $@
