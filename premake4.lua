require "qt-support"

solution "OpenPref"
    configurations { "Debug", "Release" }

    project "OpenPref"
        kind "WindowedApp"
        language "C++"
        targetname "openpref"

        uses { "QtGui", "QtNetwork" }
        uidir "obj/_ui"
        mocdir "obj/_moc"
        rccdir "obj/_rcc"

        files {
            "src/**.h",
            "src/**.cpp",
            "src/**.ui",
            "openpref.qrc",
            "*.ts"
        }
        includedirs {
            "src",
            "src/**"
        }
        configuration "Debug"
            defines { "DEBUG" }
            flags { "Symbols" }

        configuration "Release"
            defines { "NDEBUG" }
            flags { "Optimize" }
