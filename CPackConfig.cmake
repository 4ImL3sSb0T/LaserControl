# CPack配置文件
set(CPACK_GENERATOR "ZIP")

# Windows平台特定设置
if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_PACKAGE_NAME "LaserControl")
    set(CPACK_NSIS_DISPLAY_NAME "Laser Control Application")
    set(CPACK_NSIS_PACKAGE_NAME "LaserControl")
    set(CPACK_NSIS_CONTACT "wansheng06@outlook.com")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
endif()

# Linux平台特定设置
if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB;RPM;TBZ2")

    # DEB包设置
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Wan Sheng")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Laser Control Application")
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6")

    # RPM包设置
    set(CPACK_RPM_PACKAGE_DESCRIPTION "Laser Control Application")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

# macOS平台特定设置
if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "LaserControl")
endif()

include(CPack)
