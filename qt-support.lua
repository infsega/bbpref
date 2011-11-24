--
-- qt-support.lua
-- Support for Qt Framework for Premake
-- Copyright (c) 2011 Konstantin Tokarev
--
-- Known limitations:
--      don't support static Qt builds yet
--      only gmake
--      no Windows support yet
--      QTDIR is hardcoded into Makefile
--      no dependencies for ui-generated headers (they should depend on headers
--          of used widgets)
--

local _s = string.format

premake.qt = {}

--
-- Helper functions
--

function premake.qt.massinsert(t, ...)
    t = t or {}
    for _,v in ipairs {...} do
        table.insert(t, v)
    end
end

function premake.qt.joinTables(t1, t2)
    for k,v in ipairs(t2) do
        table.insert(t1, v)
    end
    return t1
end

function premake.qt.dedup(t)
    unique = {}
    for _, v in ipairs(t) do
        if not table.contains(unique, v) then
            table.insert(unique, v)
        end
    end
    t = nil
    return unique
end


-- function premake.qt.isstaticlibrary(fname)
--     local ext = path.getextension(fname):lower()
--     if os.is("windows") and ext == ".lib" then
--         return true
--     end
--     return ext == ".a"
-- end
-- 
-- function premake.qt.issharedlibrary(fname)
--     local ext = path.getextension(fname):lower()
--     if os.is("windows") and ext == ".dll" then -- Not sure if it will work for Qt
--         return true
--     end
--     if os.is("macosx") and ext == ".dylib" then
--         return true
--     end
--     return ext == ".so"
-- end



--
-- This table contains "type detector" functions as keys,
-- and lists of code generation callbacks as values
--
premake.qt.codegencallbacks = {}

function premake.qt.registercodegenerator(typedetector, generator)
    if not premake.qt.codegencallbacks[typedetector] then
        premake.qt.codegencallbacks[typedetector] = {}
    end
    table.insert(premake.qt.codegencallbacks[typedetector], generator)
end

--
-- Type detectors
--

function premake.qt.ismocableheader(fname)
    if not path.iscppheader(fname) then
        return false
    end
    local f = io.open(fname)
    for line in f:lines(fname) do
        if line:find("Q_OBJECT") then
            print("Mocable: "..fname)
            io.close(f)
            return true
        end
    end
    io.close(f)
    return false
end

function premake.qt.ismocablecppfile(fname)
    if not path.iscppfile(fname) then
        return false
    end
    local f = io.open(fname)
    for line in f:lines(fname) do
        if line:find("#%s*include%s*\""..path.getbasename(fname).."%.moc\"") then
            print("Mocable: "..fname)
            io.close(f)
            return true
        end
    end
    io.close(f)
    return false
end

--Test
--local lines = { "#include \"abc.moc\"", "#     include \"abc.moc\"", "#include abc.moc", "#include \"abc_moc\"" }
--for _,v in ipairs(lines) do
--    if v:find("#%s*include%s*\""..path.getbasename("src/abc.cpp").."%.moc\"") then
--        print "Mocable"
--    else
--        print "Not mocable"
--    end
--end
--

function premake.qt.typematcher(extension)
    return function(fname)
        local ext = path.getextension(fname):lower()
        return ext == extension
    end
end

premake.qt.isuifile = premake.qt.typematcher(".ui")
premake.qt.isqrcfile = premake.qt.typematcher(".qrc")
premake.qt.istsfile = premake.qt.typematcher(".ts")



-- Callbacks for code generation
-- @param footer Table containing generated lines of Makefile
-- @param file Current file from project for which code should be generated

function premake.qt.moc(footer, file)
    local moc_source = _s('$(MOCDIR)/moc_%s.cpp',  _MAKE.esc(path.getbasename(file)))
    table.insert(footer, _s('%s: %s', moc_source, _MAKE.esc(file)))
    table.insert(footer, '\t@echo moc '..path.getname(file))
    table.insert(footer, '\t$(SILENT) $(QTMOC) $(MOCFLAGS) -nw -o "$@" "$<"')
    return moc_source
end

function premake.qt.moc_i(footer, file)
    local moc_source = _s('$(MOCDIR)/%s.moc',  _MAKE.esc(path.getbasename(file)))
    table.insert(footer, _s('%s: %s', moc_source, _MAKE.esc(file)))
    table.insert(footer, '\t@echo moc '..path.getname(file))
    table.insert(footer, '\t$(SILENT) $(QTMOC) $(MOCFLAGS) -i -o "$@" "$<"')
    return moc_source
end

function premake.qt.uic(footer, file)
    local ui_header = _s('$(UIDIR)/ui_%s.h',  _MAKE.esc(path.getbasename(file)))
    table.insert(footer, _s('%s: %s', ui_header, _MAKE.esc(file)))
    table.insert(footer, '\t@echo uic '..path.getbasename(file))
    table.insert(footer, '\t$(SILENT) $(QTUIC) "$<" -o "$@"')
    return ui_header
end

function premake.qt.qrc_depends(file)
    local qrcfile = io.open(file)
    local qrc = qrcfile:read("*a")
    return string.gmatch(qrc, "<file[^>]*>([^<]+)</file>")
end

function premake.qt.rcc(footer, file)
    local qrc_source = _s('$(RCCDIR)/qrc_%s.cpp',  _MAKE.esc(path.getbasename(file)))
    table.insert(footer, _s('%s: %s \\', qrc_source, _MAKE.esc(file)))
    for dep in premake.qt.qrc_depends(file) do
        table.insert(footer, " "..path.join(path.getdirectory(file), dep).." \\")
    end
    table.insert(footer, '\n\t@echo rcc '..path.getname(file))
    table.insert(footer, '\t$(SILENT) $(QTRCC) -name '.._MAKE.esc(path.getbasename(file))..' -o "$@" "$<"')
    return qrc_source
end

function premake.qt.lrelease(footer, file)
    local qm_file = _s('$(QMDIR)/%s.qm',  _MAKE.esc(path.getbasename(file)))
    table.insert(footer, _s('%s: %s', qm_file, _MAKE.esc(file)))
    table.insert(footer, '\t@echo lrelease '..path.getbasename(file))
    table.insert(footer, '\t$(SILENT) $(QTLRELEASE) "$<" -qm "$@"')
    return qm_file
end

-- Register callbacks
premake.qt.registercodegenerator(premake.qt.ismocableheader, premake.qt.moc)
premake.qt.registercodegenerator(premake.qt.ismocablecppfile, premake.qt.moc_i)
premake.qt.registercodegenerator(premake.qt.isuifile, premake.qt.uic)
premake.qt.registercodegenerator(premake.qt.isqrcfile, premake.qt.rcc)
premake.qt.registercodegenerator(premake.qt.istsfile, premake.qt.lrelease)

-- Write Makefile rules for code generation
function premake.qt.writecodegenrules(prj)
    print ("Found Qt project " .. prj.name)
    local footer = {}
    if not prj.qt_generated_files then
        prj.qt_generated_files = {}
    end

    for _,file in ipairs(prj.files) do
        -- Exclude generated files from code generation
        if not table.contains(prj.qt_generated_files, file) then
            -- Iterate over premake.qt.codegencallbacks
            -- If some of registered type detectors returns true on the file,
            -- run all associated callbacks
            for typedetector,generators in pairs(premake.qt.codegencallbacks) do
                if typedetector(file) then
                    for _,generator in ipairs(generators) do
                        local filename = generator(footer, file)
                        table.insert(prj.files, filename)
                        table.insert(prj.qt_generated_files, filename)
                    end
                end
            end
        end
    end
    table.insert(footer, "\n")
    table.insert(footer, "prebuild: $(MOCDIR) $(UIDIR) $(RCCDIR) $(QMDIR) "
        .. table.concat(prj.qt_generated_files, " \\\n "))
    premake.qt.joinTables(prj.makeprjfooter, footer)
end

-- Find Qt4 and set up additional Makefile rules
function premake.qt.useqt(prj)
    if prj.use_qt_guard then
        return
    end
    prj.use_qt_guard = true

    --
    --We use the next Qt search strategy:
    --1. qtdir in project configuration
    --2. first $(QMAKE) executable in PATH
    --Maybe we should allow using of pkg-config for it.
    --
    premake.qt.massinsert(prj.makeprjheader,
        "QMAKE ?= qmake",
        "QT_BIN_DEFAULT     := $(shell $(QMAKE) -query QT_INSTALL_BINS)",
        "QT_LIB_DEFAULT     := $(shell $(QMAKE) -query QT_INSTALL_LIBS)",
        "QT_INCLUDE_DEFAULT := $(shell $(QMAKE) -query QT_INSTALL_HEADERS)"
        -- Do we need mkspecs, plugins, other stuff?
    )

    -- TODO: Replace with premake.make_mkdirrule
    premake.qt.massinsert(prj.makeprjfooter,
        "$(MOCDIR):",
        "\t$(SILENT) mkdir -p $(MOCDIR)",
        "$(UIDIR):",
        "\t$(SILENT) mkdir -p $(UIDIR)",
        "$(RCCDIR):",
        "\t$(SILENT) mkdir -p $(RCCDIR)",
        "$(QMDIR):",
        "\t$(SILENT) mkdir -p $(QMDIR)",
        ""
    )

    -- TODO: Replace with cross-platform
    prj.makeprjclean = prj.makeprjclean or {}
    -- QMDIR may be equal TARGETDIR which is not removed
    table.insert(prj.makeprjclean, "$(SILENT) rm -rf $(MOCDIR) $(UIDIR) $(RCCDIR)")

    print("Uses Qt!")
end

--
-- Adds variable @a varname with value @value to Makefile
--
function premake.qt.makevariable(cfg, varname, value)
    cfg.qt_vars = cfg.qt_vars or {}
    if not table.contains(cfg.qt_vars, varname) then
        table.insert(cfg.makeprjconfig, varname.. " = "..value)
        table.insert(cfg.qt_vars, varname)
    end
end


--
-- Qt modules
--

function premake.qt.usemodule(cfg, moduleName, moduleDefines)
    if os.is("macosx") then
        local fwname = moduleName..".framework"
        local fwheaders = path.join(fwname, "Headers")
        table.insert(cfg.links, fwname)
        table.insert(cfg.includedirs, path.join("$(QT_LIB)", fwheaders))
    else
        table.insert(cfg.links, moduleName)
    end
    table.insert(cfg.includedirs, "$(QT_INCLUDE)/" .. moduleName)
    table.insert(cfg.defines, moduleDefines)
    print("Uses "..moduleName.."!")
end

function premake.qt.useqtcore(cfg)
    -- Typically, Qt is installed in the next layout:
    -- "QTDIR"/
    --          bin/
    --          include/
    --          lib/
    --          mkspecs/
    --          ...
    -- Linux distros can shuffle stuff randomly, but QTDIR with
    -- proper symlinks usually exists.
    --
    -- AFAIK single exception is Qt Mac installation done by installer
    --

    if cfg.qtdir then
        premake.qt.makevariable(cfg, "QTDIR", cfg.qtdir)
        premake.qt.makevariable(cfg, "QT_BIN", "$(QTDIR)/bin")
        premake.qt.makevariable(cfg, "QT_INCLUDE", "$(QTDIR)/include")
        premake.qt.makevariable(cfg, "QT_LIB", "$(QTDIR)/lib")
    else
        premake.qt.makevariable(cfg, "QT_BIN", "$(QT_BIN_DEFAULT)")
        premake.qt.makevariable(cfg, "QT_INCLUDE", "$(QT_INCLUDE_DEFAULT)")
        premake.qt.makevariable(cfg, "QT_LIB", "$(QT_LIB_DEFAULT)")
    end
    premake.qt.makevariable(cfg, "QTMOC", "$(QT_BIN)/moc")
    premake.qt.makevariable(cfg, "QTUIC", "$(QT_BIN)/uic")
    premake.qt.makevariable(cfg, "QTRCC", "$(QT_BIN)/rcc")
    premake.qt.makevariable(cfg, "QTLRELEASE", "$(QT_BIN)/lrelease")
    premake.qt.makevariable(cfg, "MOCDIR", cfg.mocdir or "$(OBJDIR)")
    premake.qt.makevariable(cfg, "UIDIR", cfg.uidir or "$(OBJDIR)")
    premake.qt.makevariable(cfg, "RCCDIR", cfg.rccdir or "$(OBJDIR)")
    premake.qt.makevariable(cfg, "QMDIR", cfg.qmdir or "$(TARGETDIR)")

    if os.is("macosx") then
        premake.qt.makevariable(cfg, "MOCFLAGS", "$(INCLUDES) $(DEFINES) -D__APPLE__ -D__GNUC__")
    else
        premake.qt.makevariable(cfg, "MOCFLAGS", "$(INCLUDES) $(DEFINES)")
    end

    premake.qt.massinsert(cfg.includedirs,
        "$(QT_INCLUDE)",
        "$(UIDIR)",
        "$(MOCDIR)"
    )
    table.insert(cfg.libdirs, "$(QT_LIB)")
    -- Add rpath
    if not os.is("windows") and not os.is("macosx") then
        table.insert(cfg.defines, "_REENTRANT")
        premake.qt.massinsert(cfg.linkoptions, "-Wl,-rpath,$(QT_LIB)", "-lpthread")
    end

    premake.qt.usemodule(cfg, "QtCore", "QT_CORE_LIB")
--
--  if QtCore is shared
--  Waiting for proper os.findlib to search custom paths and static libs
--    if cfg.qtdir and os.findlib("QtCore", path.join(cfg.qtdir, "lib") ...
    table.insert(cfg.defines, "QT_SHARED")
--  else
--  defines("QT_STATIC")
--  end
end

function premake.qt.useqtgui(cfg)
    premake.qt.usemodule(cfg, "QtGui", "QT_GUI_LIB")
end

function premake.qt.useqtnetwork(cfg)
    premake.qt.usemodule(cfg, "QtNetwork", "QT_NETWORK_LIB")
end

function premake.qt.useqtopengl(cfg)
    premake.qt.usemodule(cfg, "QtOpenGL", "QT_OPENGL_LIB")

    -- untested
    if os.is("windows") then
        premake.qt.massinsert(cfg.links, "glu32", "opengl32", "gdi32", "user32")
    elseif os.is("macosx") then
        table.insert(cfg.links, "AGL.framework")
    else
        table.insert(cfg.links, "GL") -- GL ES?
    end
end

function premake.qt.useqtsql(cfg)
    premake.qt.usemodule(cfg, "QtSql", "QT_SQL_LIB")
end

function premake.qt.useqtsvg(cfg)
    premake.qt.usemodule(cfg, "QtSvg", "QT_SVG_LIB")
end

function premake.qt.useqtxml(cfg)
    premake.qt.usemodule(cfg, "QtXml", "QT_XML_LIB")
end

function premake.qt.useqtwebkit(cfg)
    premake.qt.usemodule(cfg, "QtWebKit", "QT_WEBKIT_LIB")
end

function premake.qt.useqt3support(cfg)
    premake.qt.usemodule(cfg, "Qt3Support", "QT_QT3SUPPORT_LIB")
end

local qt_uses_table = {
    ["QtCore"] = premake.qt.useqtcore,
    ["QtGui"] = premake.qt.useqtgui,
    ["QtNetwork"] = premake.qt.useqtnetwork,
    ["QtOpenGL"] = premake.qt.useqtopengl,
    ["QtSql"] = premake.qt.useqtsql,
    ["QtSvg"] = premake.qt.useqtsvg,
    ["QtXml"] = premake.qt.useqtxml,
    ["QtWebKit"] = premake.qt.useqtwebkit,
    ["Qt3Support"] = premake.qt.useqt3support
}

local qt_module_depends = {
    ["QtGui"] = { "QtCore" },
    ["QtNetwork"] = { "QtCore" },
    ["QtOpenGL"] = { "QtCore" },
    ["QtSql"] = { "QtCore" },
    ["QtSvg"] = { "QtCore" },
    ["QtXml"] = { "QtCore" },
    ["QtWebKit"] = { "QtGui", "QtNetwork" },
    ["Qt3Support"] = { "QtCore" }
}


function premake.qt.runuses(cfg)
    for _,u in ipairs(cfg.uses) do
        local f = qt_uses_table[u]
        if f then
            f(cfg)
        end
    end
end


--
-- Inject our code after baking but before gmake action
--

-- Rename gmake action into _gmake
assert(premake.action.list.gmake, "Your Premake does not have gmake action!")
premake.action.list.gmake, premake.action.list._gmake = nil, premake.action.list.gmake
premake.action.list._gmake.trigger = "_gmake"

-- Add new gmake action
newaction {
    trigger     = "gmake",
    description = premake.action.get("_gmake").description,
    shortname   = premake.action.get("_gmake").shortname,
    valid_kinds = premake.action.get("_gmake").valid_kinds,
    valid_languages = premake.action.get("_gmake").valid_languages,
    valid_tools = premake.action.get("_gmake").valid_tools,

    onproject = function(prj)
        local cc = premake.gettool(prj)
        local platforms = premake.filterplatforms(prj.solution, cc.platforms, "Native")
        for _, platform in ipairs(platforms) do
            for cfg in premake.eachconfig(prj, platform) do
                cfg.links = cfg.links or {}
                cfg.includedirs = cfg.includedirs or {}
                cfg.defines = cfg.defines or {}
                cfg.linkoptions = cfg.linkoptions or {}

                -- Detect Qt use, insert module dependencies
                if table.contains(cfg.uses, "Qt") then
                    premake.qt.useqt(prj)
                    if prj.kind == "WindowedApp" then
                        table.insert(cfg.uses, "QtGui")
                    else
                        table.insert(cfg.uses, "QtCore")
                    end
                end
                for qtmod, depends in pairs(qt_module_depends) do
                    if table.contains(cfg.uses, qtmod) then
                        premake.qt.useqt(prj)
                        for _, dep in ipairs(depends) do
                            table.insert(cfg.uses, dep)
                        end
                    end
                end
            end

            -- Configuration block of Makefile
            for cfg in premake.eachconfig(prj, platform) do
                premake.qt.runuses(cfg)

                cfg.includedirs = premake.qt.dedup(cfg.includedirs)
                cfg.defines = premake.qt.dedup(cfg.defines)
                cfg.links = premake.qt.dedup(cfg.links)
                cfg.linkoptions = premake.qt.dedup(cfg.linkoptions)
            end
        end

        -- Makefile footer
        premake.qt.writecodegenrules(prj)
    end,

    execute = function()
        premake.action.call("_gmake")
    end
}


--
-- Public Qt-specific API (tries to be consistent with QMake)
--
newapis {
    -- Root of Qt installation
    qtdir =
    {
        kind = "path",
        scope = "config"
    },

    -- Directory where moc_*.cpp and *.moc files are written (by default objdir)
    mocdir =
    {
        kind = "path",
        scope = "config"
    },

    -- Directory where ui_*.h files are written (by default objdir)
    uidir =
    {
        kind = "path",
        scope = "config"
    },

    -- Directory where qrc_*.cpp files are written (by default objdir)
    rccdir =
    {
        kind = "path",
        scope = "config"
    },

    -- Directory where *.qm files are written (by default targetdir)
    qmdir =
    {
        kind = "path",
        scope = "config"
    }
}

