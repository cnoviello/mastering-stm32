#!/usr/bin/python
#
# This tool is used to "rename" chapter examples. 
#
import sys, os

DRYRUN = 0

srcCH = "ch10"
dstCH = "ch11"

srcCHdes = "Chapter 10"
dstCHdes = "Chapter 11"

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

	if not DRYRUN:
		print "Renaming directory: ", os.path.join(os.getcwd(), projectDir, "src", srcCH)
		os.rename(os.path.join(os.getcwd(), projectDir, "src", srcCH), os.path.join(os.getcwd(), projectDir, "src", dstCH))

