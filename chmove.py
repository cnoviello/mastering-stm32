#!/usr/bin/python
#
# This tool is used to "rename" chapter examples. 
#
import sys, os

DRYRUN = 0

srcCH = "ch12"
dstCH = "ch13"

srcCHdes = "Chapter 12"
dstCHdes = "Chapter 13"

print "Retrieving projects list...."
projectDirs = [d for d in os.listdir(os.getcwd()) if d.startswith("nucleo-")]

for projectDir in projectDirs:
	cproject = open(os.path.join(os.getcwd(), projectDir, ".cproject"), "r+")
	cprojectData = cproject.read()
	cprojectData = cprojectData.replace(srcCH, dstCH)
	cprojectData = cprojectData.replace(srcCH.upper(), dstCH.upper())
	cprojectData = cprojectData.replace(srcCHdes, dstCHdes)
	cproject.seek(0)
	if not DRYRUN:
		print "Changing projects: ", cproject.name
		cproject.write(cprojectData)
	cproject.close()

	try:
		if not DRYRUN:
			os.rename(os.path.join(os.getcwd(), projectDir, "src", srcCH), os.path.join(os.getcwd(), projectDir, "src", dstCH))
			print "Renaming directory: ", os.path.join(os.getcwd(), projectDir, "src", srcCH)
	except OSError:
		pass
