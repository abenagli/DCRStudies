#!/usr/bin/env python
import os, re
import commands
import math, time
import sys
import argparse
import subprocess

parser = argparse.ArgumentParser(description='This script splits CMSSW tasks in multiple jobs and sends them on HERCULES')

parser.add_argument("-l",  "--label",          required=True,     type=str,   help="job label")
parser.add_argument("-b",  "--baseFolder",     required=True,     type=str,   help="base folder")
parser.add_argument("-e",  "--exeName",        required=True,     type=str,   help="absolute path of executable")
parser.add_argument("-o",  "--outputFileName", required=True,     type=str,   help="outputFileName")
parser.add_argument("-c",  "--configFile",     required=True,     type=str,   help="config file")
parser.add_argument("-q",  "--queue",          default="1nh",     type=str,   help="hercules queue to use")
parser.add_argument("-s",  "--submit",                                        help="submit jobs", action='store_true')
parser.add_argument("-v",  "--verbose",                                       help="increase output verbosity", action='store_true')
parser.add_argument("-p1", "--nPhE",           required=False,    type=float, help="number of signal ph.e.")
parser.add_argument("-p2", "--DCR",            required=False,    type=float, help="dark count rate (in GHz)")
parser.add_argument("-p3", "--SPTR",           required=False,    type=float, help="single photon time resolution (in ns)")
parser.add_argument("-p4", "--inFileName1pe",  required=False,    type=str,   help="csv file containing template 1 p.e. pulse shape")
parser.add_argument("-p5", "--nToys",          required=False,    type=int,   help="number of toys")

args = parser.parse_args()



print 
print 'START'
print 

currDir = os.getcwd()

print

try:
   subprocess.check_output(['mkdir','jobs'])
except subprocess.CalledProcessError as e:
   print e.output
try:
   subprocess.check_output(['mkdir','jobs/'+args.label])
except subprocess.CalledProcessError as e:
   print e.output


##### creates directory and file list for job #######
jobDir = currDir+'/jobs/'+args.label+'/'
os.system('mkdir '+jobDir)
os.chdir(jobDir)
   
##### creates config file #######
with open(args.baseFolder+'/'+args.configFile) as fi:
   contents = fi.read()
   with open(jobDir+"/config.cfg", "w") as fo:
      fo.write(contents)
   command = 'sed -i \"s%^outputFileName .*$%outputFileName '+args.outputFileName+'%\" '+jobDir+'/config.cfg'
   os.system(command)
   if args.nPhE:
      command = 'sed -i \"s%^nPhE .*$%nPhE '+str(args.nPhE)+'%\" '+jobDir+'/config.cfg'
      os.system(command)
   if args.DCR:
      command = 'sed -i \"s%^DCR .*$%DCR '+str(args.DCR)+'%\" '+jobDir+'/config.cfg'
      os.system(command)
   if args.SPTR:
      command = 'sed -i \"s%^SPTR .*$%SPTR '+str(args.SPTR)+'%\" '+jobDir+'/config.cfg'
      os.system(command)
   if args.inFileName1pe:
      command = 'sed -i \"s%^inFileName1pe .*$%inFileName1pe '+str(args.inFileName1pe)+'%\" '+jobDir+'/config.cfg'
      os.system(command)
   if args.nToys:
      command = 'sed -i \"s%^nToys .*$%nToys '+str(args.nToys)+'%\" '+jobDir+'/config.cfg'
      os.system(command)
   
##### creates jobs #######
with open('job.sh', 'w') as fout:
   fout.write("#!/bin/sh\n")
   fout.write("echo\n")
   fout.write("echo 'START---------------'\n")
   fout.write("echo 'current dir: ' ${PWD}\n")
   fout.write("cd "+str(args.baseFolder)+"\n")
   fout.write("echo 'current dir: ' ${PWD}\n")
   fout.write("source scripts/setup.sh\n")
   fout.write("unbuffer "+args.exeName+" "+jobDir+"/config.cfg\n")
   fout.write("echo 'STOP---------------'\n")
   fout.write("echo\n")
   fout.write("echo\n")
   os.system("chmod 755 job.sh")

###### sends bjobs ######
if args.submit:
   command = "bsub -q "+args.queue+" -cwd "+jobDir+" job.sh"
   print command
   os.system(command)
   print "job " + str(args.label) + " submitted"
os.chdir("../..")
   
print
print "your jobs:"
os.system("bjobs")
print
print 'END'
print
