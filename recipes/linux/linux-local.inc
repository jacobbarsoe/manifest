DESCRIPTION = "linux for use local files in src/linux"

SRC_URI = "git://${TOPDIR}/src/linux;${SRC_REV};protocol=file"
S = "${SRCDIR}/linux"

inherit kernel
