# GLQuake Makefile
# Converted from Visual Studio project file
# Uses Microsoft compiler toolchain (cl.exe, link.exe, ml.exe)

# Configuration (can be overridden: make CONFIG=Debug)
CONFIG ?= Release

# Directories
ifeq ($(CONFIG),Debug)
    OUTDIR = GL_Debug
    INTDIR = GL_Debug
    TARGET = $(OUTDIR)/GLQuake-Debug.exe
else
    OUTDIR = GL_Release
    INTDIR = GL_Release
    TARGET = $(OUTDIR)/GLQuake.exe
endif

# Tools
CC = cl.exe
AS = ml.exe
LINK = link.exe
RC = rc.exe

# Include directories
INCLUDES = /I"glquake" /I"shared"

# Preprocessor definitions
ifeq ($(CONFIG),Debug)
    DEFINES = /DWIN32 /D_DEBUG /D_WINDOWS /DGLQUAKE
    CFLAGS = /nologo /MDd /W3 /Od /Zi /Fd"$(INTDIR)/" $(INCLUDES) $(DEFINES)
    LINKFLAGS = /nologo /subsystem:windows /debug /out:"$(TARGET)" /ignorealldefaultlibraries
else
    DEFINES = /DWIN32 /DNDEBUG /D_WINDOWS /DGLQUAKE
    CFLAGS = /nologo /MD /W3 /O2 /Oy- /GS /Gy $(INCLUDES) $(DEFINES)
    LINKFLAGS = /nologo /subsystem:windows /out:"$(TARGET)" /ignorealldefaultlibraries
endif

# Libraries
LIBS = comctl32.lib msvcrt.lib oldnames.lib opengl32.lib winmm.lib wsock32.lib

# Source files
C_SOURCES = \
    glquake/OpenGL/gl_draw.c \
    glquake/OpenGL/gl_mesh.c \
    glquake/OpenGL/gl_model.c \
    glquake/OpenGL/gl_refrag.c \
    glquake/OpenGL/gl_rlight.c \
    glquake/OpenGL/gl_rmain.c \
    glquake/OpenGL/gl_rmisc.c \
    glquake/OpenGL/gl_rsurf.c \
    glquake/OpenGL/gl_screen.c \
    glquake/OpenGL/gl_vidnt.c \
    glquake/OpenGL/gl_warp.c \
    shared/Client/cl_demo.c \
    shared/Client/cl_input.c \
    shared/Client/cl_main.c \
    shared/Client/cl_parse.c \
    shared/Client/cl_tent.c \
    shared/Console/conproc.c \
    shared/Console/console.c \
    shared/Host/host.c \
    shared/Host/host_cmd.c \
    shared/Misc/chase.c \
    shared/Misc/cmd.c \
    shared/Misc/common.c \
    shared/Misc/crc.c \
    shared/Misc/cvar.c \
    shared/Misc/in_win.c \
    shared/Misc/keys.c \
    shared/Misc/mathlib.c \
    shared/Misc/menu.c \
    shared/Misc/sbar.c \
    shared/Misc/sys_win.c \
    shared/Misc/view.c \
    shared/Misc/wad.c \
    shared/Misc/world.c \
    shared/Misc/zone.c \
    shared/Network/net_dgrm.c \
    shared/Network/net_loop.c \
    shared/Network/net_main.c \
    shared/Network/net_win.c \
    shared/Network/net_wins.c \
    shared/Progs/pr_cmds.c \
    shared/Progs/pr_edict.c \
    shared/Progs/pr_exec.c \
    shared/Render/r_part.c \
    shared/Server/sv_main.c \
    shared/Server/sv_move.c \
    shared/Server/sv_phys.c \
    shared/Server/sv_user.c \
    shared/Sound/cd_win.c \
    shared/Sound/snd_dma.c \
    shared/Sound/snd_mem.c \
    shared/Sound/snd_mix.c \
    shared/Sound/snd_win.c

# Assembly source files (GAS format, need conversion)
ASM_SOURCES = \
    shared/Asm/math.s \
    shared/Asm/snd_mixa.s \
    shared/Asm/sys_wina.s \
    shared/Asm/worlda.s

# Resource files
RC_SOURCES = shared/winquake.rc

# Object files
C_OBJECTS = $(C_SOURCES:.c=.obj)
C_OBJECTS := $(addprefix $(INTDIR)/,$(subst /,_,$(C_OBJECTS)))

ASM_OBJECTS = $(ASM_SOURCES:.s=.obj)
ASM_OBJECTS := $(addprefix $(INTDIR)/,$(subst /,_,$(ASM_OBJECTS)))

RC_OBJECTS = $(INTDIR)/winquake.res

ALL_OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS) $(RC_OBJECTS)

# Default target
all: $(TARGET)

# Create output directories
$(OUTDIR):
	@if not exist "$(OUTDIR)" mkdir "$(OUTDIR)"

$(INTDIR):
	@if not exist "$(INTDIR)" mkdir "$(INTDIR)"

# Link target
$(TARGET): $(OUTDIR) $(INTDIR) $(ALL_OBJECTS)
	$(LINK) $(LINKFLAGS) $(ALL_OBJECTS) $(LIBS)

# Compile C source files
$(INTDIR)/%.obj: %.c
	$(CC) $(CFLAGS) /Fo"$@" /c "$<"

# Handle C files with path separators (convert / to _)
$(INTDIR)/glquake_OpenGL_%.obj: glquake/OpenGL/%.c
	$(CC) $(CFLAGS) /Fo"$@" /c "$<"

$(INTDIR)/shared_Client_%.obj: shared/Client/%.c
	$(CC) $(CFLAGS) /Fo"$@" /c "$<"

$(INTDIR)/shared_Console_%.obj: shared/Console/%.c
	$(CC) $(CFLAGS) /Fo"$@" /c "$<"

$(INTDIR)/shared_Host_%.obj: shared/Host/%.c
	$(CC) $(CFLAGS) /Fo"$@" /c "$<"

$(INTDIR)/shared_Misc_%.obj: shared/Misc/%.c
	$(CC) $(CFLAGS) /Fo"$@" /c "$<"

$(INTDIR)/shared_Network_%.obj: shared/Network/%.c
	$(CC) $(CFLAGS) /Fo"$@" /c "$<"

$(INTDIR)/shared_Progs_%.obj: shared/Progs/%.c
	$(CC) $(CFLAGS) /Fo"$@" /c "$<"

$(INTDIR)/shared_Render_%.obj: shared/Render/%.c
	$(CC) $(CFLAGS) /Fo"$@" /c "$<"

$(INTDIR)/shared_Server_%.obj: shared/Server/%.c
	$(CC) $(CFLAGS) /Fo"$@" /c "$<"

$(INTDIR)/shared_Sound_%.obj: shared/Sound/%.c
	$(CC) $(CFLAGS) /Fo"$@" /c "$<"

# Assembly files - convert from GAS to MASM format and assemble
$(INTDIR)/shared_Asm_%.obj: shared/Asm/%.s
	@echo Processing assembly file $<
	cl /EP > "$(INTDIR)/$*.spp" "$<"
	gas2masm\gas2masm < "$(INTDIR)/$*.spp" > "$(INTDIR)/$*.asm"
	$(AS) /c /Cp /coff /Fo"$@" /Zm /Zi "$(INTDIR)/$*.asm"
	@del "$(INTDIR)/$*.spp"

# Resource file compilation
$(INTDIR)/winquake.res: shared/winquake.rc
	$(RC) /fo"$@" /I"shared" "$<"

# Clean targets
clean:
	@if exist "$(OUTDIR)" rmdir /s /q "$(OUTDIR)"
	@if exist "$(INTDIR)" rmdir /s /q "$(INTDIR)"

clean-debug:
	@if exist "GL_Debug" rmdir /s /q "GL_Debug"

clean-release:
	@if exist "GL_Release" rmdir /s /q "GL_Release"

clean-all: clean-debug clean-release

# Rebuild
rebuild: clean all

# Debug and Release specific targets
debug:
	$(MAKE) CONFIG=Debug

release:
	$(MAKE) CONFIG=Release

# Help
help:
	@echo GLQuake Makefile Usage:
	@echo.
	@echo   make            - Build release configuration
	@echo   make debug      - Build debug configuration
	@echo   make release    - Build release configuration
	@echo   make clean      - Clean current configuration
	@echo   make clean-all  - Clean all configurations
	@echo   make rebuild    - Clean and rebuild current configuration
	@echo.
	@echo   CONFIG=Debug    - Set configuration to Debug
	@echo   CONFIG=Release  - Set configuration to Release (default)

# Phony targets
.PHONY: all clean clean-debug clean-release clean-all rebuild debug release help

# Dependencies (simplified - in a real build system you'd generate these)
$(C_OBJECTS): $(wildcard shared/*.h) $(wildcard glquake/*.h)