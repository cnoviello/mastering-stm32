#!/usr/bin/python
#
# This tool is used to change include paths from absolute to relative
#
import sys, os

DRYRUN = 0


paths = { "/Users/cnoviello/STM32Toolchain/mastering-stm32/%s/include": "&quot;../include&quot;",
          "/Users/cnoviello/STM32Toolchain/mastering-stm32/%s/system/include/stm32f4xx": "&quot;../system/include/stm32f4xx&quot;", 
	      "/Users/cnoviello/STM32Toolchain/mastering-stm32/%s/system/include": "&quot;../system/include&quot;",
	      "/Users/cnoviello/STM32Toolchain/mastering-stm32/%s/system/include/cmsis/device": "&quot;../system/include/cmsis/device&quot;",
	      "/Users/cnoviello/STM32Toolchain/mastering-stm32/%s/system/include/cmsis": "&quot;../system/include/cmsis&quot;"
}

print "Retrieving projects list...."
projectDirs = [d for d in os.listdir(os.getcwd()) if d.startswith("nucleo-")]

for projectDir in projectDirs:
	print "Fixing %s..." % projectDir
	cproject = open(os.path.join(os.getcwd(), projectDir, ".cproject"), "r+")
	cprojectData = cproject.read()
	for path in sorted(paths, reverse=True):
		cprojectData = cprojectData.replace((path % projectDir), paths[path])
	cproject.close()
	if not DRYRUN:
		cproject = open(os.path.join(os.getcwd(), projectDir, ".cproject"), "w+")
		cproject.write(cprojectData)
	cproject.close()